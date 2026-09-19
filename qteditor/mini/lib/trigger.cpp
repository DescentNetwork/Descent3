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

#include "trigger.h"

#include <cstring>

// Level-file (TRIG chunk) serialization for the trigger type.  The current
// on-disk layout stores the trigger name as a null-terminated string then the
// room/face/shorts and the two packed flag bitfields as raw uint16s.  Each
// read is the exact mirror of its write so triggers round-trip.

byte_istream& operator>>(byte_istream& input, trigger& data) {
  input >> data.name;
  int16_t room = 0;
  input >> room;
  data.roomnum = room;
  int16_t face = 0;
  input >> face;
  data.facenum = face;
  uint16_t flags_raw = 0;
  input >> flags_raw;
  std::memcpy(&data.flags, &flags_raw, sizeof(flags_raw));
  uint16_t activator_raw = 0;
  input >> activator_raw;
  std::memcpy(&data.activator, &activator_raw, sizeof(activator_raw));
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const trigger& data) {
  output << data.name;
  output << static_cast<int16_t>(data.roomnum) << static_cast<int16_t>(data.facenum);
  uint16_t flags_raw = 0;
  std::memcpy(&flags_raw, &data.flags, sizeof(flags_raw));
  output << flags_raw;
  uint16_t activator_raw = 0;
  std::memcpy(&activator_raw, &data.activator, sizeof(activator_raw));
  return output << activator_raw;
}