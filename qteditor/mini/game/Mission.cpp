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

// Serialization for the level_info chunk stored contiguously in a level file's
// INFO chunk.  All four fields are null-terminated strings in the exact order
// the engine reads and writes them (LoadLevel.cpp WriteObject/ReadObject).

#include "Mission.h"
#include "posix_stream.h"

byte_istream& operator>>(byte_istream& input, level_info& data) {
  return input >> data.name >> data.designer >> data.copyright >> data.notes;
}

byte_ostream& operator<<(byte_ostream& output, const level_info& data) {
  return output << data.name << data.designer << data.copyright << data.notes;
}