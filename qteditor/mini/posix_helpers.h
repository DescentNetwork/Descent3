/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * posix_stream adapters that mirror the CFILE read/write helpers the D3
 * binary formats rely on, so existing code can move from CFILE to
 * posix_istream / posix_ostream without changing the on-disk layout.
 *
 * The D3 page/level formats store strings as *variable-length*,
 * NUL-terminated byte sequences (written by cf_WriteString), NOT fixed-width
 * padding.  fixed_string_t reads a fixed byte count, so it would corrupt those
 * files.  These helpers preserve the exact legacy behaviour using posix_stream's
 * byte read/write primitives.
 */

#include <cstdint>
#include <string>
#include <posix_stream.h>

// Read a variable-length, NUL-terminated string (binary mode) into `out`.
// Stops at the first NUL byte or EOF.  Mirrors cf_ReadString.
static inline void psReadString(posix_istream &in, std::string &out, size_t maxLen = 0) {
  out.clear();
  while (true) {
    uint8_t c = in.get();
    if (in.eof() || c == 0)
      break;
    if (maxLen == 0 || out.size() < maxLen)
      out.push_back(static_cast<char>(c));
  }
}


inline byte_istream& operator >>(byte_istream& input, std::string& data)
{
  data.clear();
  for(uint8_t c = input.get(); c; c = input.get())
    data.push_back(static_cast<char>(c));
  return input;
}

inline byte_ostream& operator <<(byte_ostream& output, const std::string& data)
{
  output.write(data.data(), data.size());
  output.put(0);
  return output;
}
