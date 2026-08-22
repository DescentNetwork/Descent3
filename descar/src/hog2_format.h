#ifndef HOG2_FORMAT_H
#define HOG2_FORMAT_H

// C++
#include <list>
#include <string_view>
#include <algorithm>

// POSIX++
#include <cstdint>
#include <cstring>

// Project
#include <posix_stream.h>
#include <fixed_string.h>

template<typename T>
static inline T lowercase(T s)
{
  std::transform(std::begin(s), std::end(s), std::begin(s), [](unsigned char c) { return std::tolower(c); });
  return s;
}

namespace hog2
{
  using namespace std::literals::string_view_literals;
  struct header_t
  {
    fixed_string_t<4> magic = "HOG2"sv;
    uint32_t entry_count = 0;                     // number of file entries in header
    uint32_t file_data_offset = sizeof(header_t); // offset in file to filedata.
    fixed_string_t<56> reserved = { '\xff' }; // filled with 0xff
  };
  static_assert(sizeof(header_t) == 68, "header_t is the incorrect size. 68 bytes was expected.");

  struct entry_t
  {
    fixed_string_t<36> name = { '\0' };   // file name (36 characters)
    uint32_t flags = 0;             // extra info
    uint32_t len = 0;               // length of file
    uint32_t timestamp = 0;         // time of file.
  };
  static_assert(sizeof(entry_t) == 48, "entry_t is the incorrect size. 48 bytes was expected.");

  class archive_t
  {
  public:
    using const_iterator = std::list<entry_t>::const_iterator;
    const_iterator begin(void) const { return std::cbegin(m_file_entries); }
    const_iterator end(void) const { return std::cend(m_file_entries); }

    void addEntry(entry_t& entry)
    {
      m_file_entries.push_back(entry);
      m_header.entry_count++;
      m_header.file_data_offset += 48;

      m_file_entries.sort([](entry_t& a, entry_t& b)
                          { return lowercase(a.name) < lowercase(b.name); });
    }

    void removeEntry(const_iterator entry_iter)
    {
      m_file_entries.erase(entry_iter);
      m_header.entry_count--;
      m_header.file_data_offset -= 48;
    }

    std::size_t fileOffset(const_iterator entry_iter)
    {
      std::size_t offset = m_header.file_data_offset;
      for (const_iterator pos = begin(); pos != entry_iter && pos != end(); pos++)
        offset += pos->len;

      return offset;
    }

  private:
    header_t m_header;
    std::list<entry_t> m_file_entries;

    friend byte_ostream& operator <<(byte_ostream& output, const archive_t& archive);
    friend byte_istream& operator >>(byte_istream& input, archive_t& archive);
  };

} // namespace D3

#endif // HOG2_FORMAT_H
