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

// Table-file serialization for the anim_entry / anim_elem animation glue
// embedded in the generic page layout.  Only the from/to/spc triplet of each
// anim_entry is stored on disk (anim_sound_index and used are runtime-only).

#include "objinfo.h"

//-----------------------------------------------------------------------------
// Animation glue (generic pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, anim_entry& data) {
  return input >> data.from >> data.to >> data.spc;
}

byte_ostream& operator<<(byte_ostream& output, const anim_entry& data) {
  return output << data.from << data.to << data.spc;
}

byte_istream& operator>>(byte_istream& input, anim_elem& data) {
  for (anim_entry& e : data.elem)
    input >> e;
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const anim_elem& data) {
  for (const anim_entry& e : data.elem)
    output << e;
  return output;
}