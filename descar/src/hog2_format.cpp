// Project
#include "hog2_format.h"

namespace hog2
{
  byte_ostream& operator <<(byte_ostream& output, const header_t& header)
  {
    return output
           << header.magic
           << header.entry_count
           << header.file_data_offset
           << header.reserved;
  }

  byte_istream& operator >>(byte_istream& input, header_t& header)
  {
    input >> header.magic
          >> header.entry_count
          >> header.file_data_offset
          >> header.reserved;

    if (header.magic != std::array<char, 4>{'H', 'O', 'G', '2'})
      throw std::invalid_argument("HOG2 magic number not found. Not a hog file.");

    return input;
  }

  byte_ostream& operator <<(byte_ostream& output, const entry_t& entry)
  {
    return output
           << entry.name
           << entry.flags
           << entry.len
           << entry.timestamp;
  }

  byte_istream& operator >>(byte_istream& input, entry_t& entry)
  {
    return input
           >> entry.name
           >> entry.flags
           >> entry.len
           >> entry.timestamp;
  }

  byte_ostream& operator <<(byte_ostream& output, const archive_t& archive)
  {
    output << archive.m_header;
    for (const entry_t& entry : archive.m_file_entries)
      output << entry;

    return output;
  }

  byte_istream& operator >>(byte_istream& input, archive_t& archive)
  {
    input >> archive.m_header;
    for(uint32_t i = archive.m_header.entry_count; i > 0; i--)
    {
      entry_t entry;
      input >> entry;
      archive.m_file_entries.emplace_back(entry);
    }

    return input;
  }

}
