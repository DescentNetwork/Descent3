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

// Door page reader (ported from doorpage.cpp : 261-292).

#include "doorpage.h"

#include <cstdint>
#include <string>

#include "manage.h"
#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Door page (ported from doorpage.cpp : 243-292)
//-----------------------------------------------------------------------------

// On-disk order (new format, DOORPAGE_VERSION 3) is: name, image_name,
// total_open_time, total_close_time, total_time_open, flags, hit_points,
// open_sound_name, close_sound_name, module_name.  The door fields are
// interleaved with the page-level image/sound names, so the page operators
// inline them all rather than using a door sub-record operator.

byte_istream& operator>>(byte_istream& input, mngs_door_page& data) {
  int16_t version = 0;
  input >> version;

  return input
         >> data.door_struct.name
         >> data.image_name
         >> data.door_struct.total_open_time
         >> data.door_struct.total_close_time
         >> data.door_struct.total_time_open
         >> data.door_struct.flags
         >> data.door_struct.hit_points
         >> data.open_sound_name
         >> data.close_sound_name
         >> data.door_struct.module_name;
}

byte_ostream& operator<<(byte_ostream& output, const mngs_door_page& data) {
  return output
         << static_cast<int16_t>(DOORPAGE_VERSION)
         << data.door_struct.name
         << data.image_name
         << data.door_struct.total_open_time
         << data.door_struct.total_close_time
         << data.door_struct.total_time_open
         << data.door_struct.flags
         << data.door_struct.hit_points
         << data.open_sound_name
         << data.close_sound_name
         << data.door_struct.module_name;
}

int mng_ReadNewDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  infile >> *doorpage;

  // This is a valid new page
  doorpage->door_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  if (!Old_table_method)
    return mng_ReadNewDoorPage(infile, doorpage);
  return 0; // old command-based table not supported in mini build
}
