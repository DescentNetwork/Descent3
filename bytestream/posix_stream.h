#ifndef ENDIANFILESTREAM_H
#define ENDIANFILESTREAM_H

// C++
#include <filesystem>
#include <ios>
#include <system_error>
#include <type_traits>

// Project
#include "portable_endian.h"

struct byte_ostream
{
  virtual byte_ostream& put(const uint8_t data) = 0;
  virtual byte_ostream& write(const void* s, size_t length) = 0;
};

struct byte_istream
{
  virtual uint8_t get(void) = 0;
  virtual byte_istream& read(void* s, size_t length) = 0;
};

constexpr const size_t file_buffer_size = 0x8000;

class posix_stream_base
{
public:
  posix_stream_base(void) = default;
  posix_stream_base(posix_stream_base&& other) { operator=(std::move(other)); }
  posix_stream_base(const std::filesystem::path& path,
                    std::ios_base::openmode mode)
  {
    open(path, mode);
  }
  posix_stream_base(void* memory, size_t length, std::ios_base::openmode mode);
  ~posix_stream_base(void);

  posix_stream_base& operator=(posix_stream_base&& other);

  constexpr bool good(void) const { return int(m_code) == 0; }
  constexpr std::errc error(void) const { return m_code; }

  bool open(const std::filesystem::path& path, std::ios_base::openmode mode);
  bool close(void);

  bool seek(off_t off, std::ios_base::seekdir dir = std::ios_base::beg) const;
  off_t tell(void) const;

  size_t size(void) const;
  bool eof(void) const;

  constexpr bool is_open(void) const { return m_fp != nullptr; }

protected:
  mutable std::errc m_code = std::errc(0);
  FILE* m_fp = nullptr;

private:
  char* m_buffer = nullptr;
};

struct posix_istream : posix_stream_base, byte_istream
{
  using posix_stream_base::posix_stream_base;
  using posix_stream_base::operator=;

  uint8_t get(void);
  byte_istream& read(void* data, size_t length);
  byte_istream& operator>>(int8_t& data)
  {
    data = get();
    return *this;
  }
  byte_istream& operator>>(uint8_t& data)
  {
    data = get();
    return *this;
  }
};

struct posix_ostream : posix_stream_base, byte_ostream
{
  using posix_stream_base::posix_stream_base;
  using posix_stream_base::operator=;

  byte_ostream& put(const uint8_t data);
  byte_ostream& write(const void* data, size_t length);

  byte_ostream& operator<<(int8_t& data) { return put(data); }
  byte_ostream& operator<<(uint8_t& data) { return put(data); }
};

template <typename T,
          std::enable_if_t<(std::is_arithmetic<T>::value && sizeof(T) > 1),
                           bool> = true>
byte_istream& operator>>(byte_istream& input, T& data)
{
  T sdata;
  input.read(&sdata, sizeof(T));
  data = le_to_host(sdata);
  return input;
}

template <typename T,
          std::enable_if_t<(std::is_arithmetic<T>::value && sizeof(T) > 1),
                           bool> = true>
byte_ostream& operator<<(byte_ostream& output, const T data)
{
  T sdata = host_to_le(data);
  return output.write(&sdata, sizeof(T));
}

#endif // ENDIANFILESTREAM_H
