#include "posix_stream.h"

// C++
#include <iostream>

// POSIX++
#include <cassert>
#include <cerrno>
#include <cstdio>

constexpr const char* translate_mode(std::ios_base::openmode mode)
{
  const bool read = mode & std::ios_base::in;
  const bool write = mode & std::ios_base::out;
  const bool append = mode & std::ios_base::app;
  const bool truncate = mode & std::ios_base::trunc;

  if (read)
  {
    if (!write) return "rb"; // read
    else if (append)
      return "a+b"; // read/write/append
    else if (truncate)
      return "w+b"; // read/write/truncate
    else
      return "r+b"; // read/write
  }
  else if (write)
  {
    if (append) return "ab"; // write/append
    else                     // if(truncate)
      return "wb";           // write/truncate
  }

  return nullptr;
}

posix_stream_base::posix_stream_base(void* memory, size_t length,
                                     std::ios_base::openmode mode)
{
  m_fp = fmemopen(memory, length, translate_mode(mode));
  m_code = std::errc(errno);
}

posix_stream_base::~posix_stream_base(void)
{
  if (is_open() && !close()) delete[] m_buffer, m_buffer = nullptr;
}

posix_stream_base& posix_stream_base::operator=(posix_stream_base&& other)
{
  if (m_fp != nullptr) fclose(m_fp);

  m_fp = other.m_fp;
  other.m_fp = nullptr;

  if (m_buffer != nullptr) delete[] m_buffer;

  m_buffer = other.m_buffer;
  other.m_buffer = nullptr;

  m_code = other.m_code;

  return *this;
}

bool posix_stream_base::open(const std::filesystem::path& path,
                             std::ios_base::openmode mode)
{
  if (is_open())
  {
    m_code = std::errc::io_error;
    return false;
  }

  m_fp = fopen(path.c_str(), translate_mode(mode));

  if (m_fp != nullptr)
  {
    m_buffer = new char[file_buffer_size];
    setvbuf(m_fp, m_buffer, _IOFBF, file_buffer_size);
  }

  m_code = std::errc(errno);

  return is_open();
}

bool posix_stream_base::close(void)
{
  int value = fclose(m_fp);
  if (!value)
  {
    m_fp = nullptr;
    if (m_buffer != nullptr) delete[] m_buffer, m_buffer = nullptr;
  }

  m_code = std::errc(errno);
  return !value;
}

static_assert(std::ios_base::beg == SEEK_SET, "bad value");
static_assert(std::ios_base::cur == SEEK_CUR, "bad value");
static_assert(std::ios_base::end == SEEK_END, "bad value");

bool posix_stream_base::seek(off_t off, std::ios_base::seekdir dir) const
{
  int value = fseeko(m_fp, off, static_cast<int>(dir));
  m_code = std::errc(errno);
  return value != -1;
}

off_t posix_stream_base::tell(void) const
{
  off_t value = ftello(m_fp);
  m_code = std::errc(errno);
  return value;
}

size_t posix_stream_base::size(void) const
{
  size_t rvalue = 0;
  off_t original = 0;

  if ((original = tell()),
      good() && seek(0, std::ios_base::end) && (rvalue = tell()), good())
    seek(original);
  return rvalue;
}

bool posix_stream_base::eof(void) const { return feof(m_fp); }

byte_istream& posix_istream::read(void* data, size_t length)
{
  size_t bytes_read = fread(data, sizeof(uint8_t), length, m_fp);
  assert(bytes_read == length);
  return *this;
}

uint8_t posix_istream::get(void)
{
  int value = fgetc(m_fp);
  m_code = std::errc(errno);
  return value;
}

byte_ostream& posix_ostream::put(uint8_t data)
{
  fputc(data, m_fp);
  m_code = std::errc(errno);
  return *this;
}

byte_ostream& posix_ostream::write(const void* data, size_t length)
{
  fwrite(data, sizeof(uint8_t), length, m_fp);
  m_code = std::errc(errno);
  return *this;
}
