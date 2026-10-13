/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Shared string/field helpers used by the per-page readers (genericpage.cpp,
// weaponpage.cpp, texpage.cpp, soundpage.cpp, doorpage.cpp, shippage.cpp,
// megapage.cpp) and gamedata_loader.cpp.  Defined `inline` in this header so
// each translation unit can use them without duplicating definitions (no ODR
// issues).

#ifndef GAMEDATA_HELPERS_H
#define GAMEDATA_HELPERS_H

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <string>
#include <posix_stream.h>

struct version_t { uint16_t ver = 0; };

inline byte_istream& operator>>(byte_istream& input, version_t&& data)
{
  uint16_t ver = 0;
  input >> ver;
  Q_ASSERT(ver == data.ver);
  return input;
}

inline byte_ostream& operator << (byte_ostream& input, const version_t& data)
{
  return input << data.ver;
}

// Case-insensitive prefix equality (replaces strnicmp(a.c_str(), b, strlen(b))).
inline bool ieq_prefix(const std::string &a, const char *b) {
  size_t n = std::strlen(b);
  if (a.size() < n)
    return false;
  for (size_t i = 0; i < n; ++i) {
    if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
      return false;
  }
  return true;
}
#endif
