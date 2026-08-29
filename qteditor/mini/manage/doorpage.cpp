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
// Door page (ported from doorpage.cpp : 261-292)
//-----------------------------------------------------------------------------

int mng_ReadNewDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  int16_t version = 0;
  infile >> version;

  // door name/image are fixed_string_t (std::string) but stored as
  // variable-length NUL-terminated strings on disk; read them via the
  // variable-length helper, not the fixed-width stream operator.
  infile >> doorpage->door_struct.name;
  infile >> doorpage->image_name;

  infile >> doorpage->door_struct.total_open_time;
  infile >> doorpage->door_struct.total_close_time;
  infile >> doorpage->door_struct.total_time_open;

  infile >> doorpage->door_struct.flags;

  if (version >= 3)
    infile >> doorpage->door_struct.hit_points;
  else
    doorpage->door_struct.hit_points = 0;

  infile >> doorpage->open_sound_name;
  infile >> doorpage->close_sound_name;

  if (version >= 2)
    infile >> doorpage->door_struct.module_name;
  else
    doorpage->door_struct.module_name[0] = '\0';

  // This is a valid new page
  doorpage->door_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  if (!Old_table_method)
    return mng_ReadNewDoorPage(infile, doorpage);
  return 0; // old command-based table not supported in mini build
}
