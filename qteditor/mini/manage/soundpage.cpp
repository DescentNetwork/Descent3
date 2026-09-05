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

// Sound page reader (ported from soundpage.cpp : 222-255).

#include "ssl_lib.h" // sound_info
#include "soundpage.h"

#include <cstdint>
#include <string>

#include "manage.h"
#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Sound page (ported from soundpage.cpp : 222-255)
//-----------------------------------------------------------------------------

int mng_ReadNewSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  /* int version = */ int16_t v; infile >> v;
  // read in name,rawfile name
  infile >> soundpage->sound_struct.name;
  infile >> soundpage->raw_name;
  infile >> soundpage->sound_struct.flags;

  infile >> soundpage->sound_struct.loop_start;
  infile >> soundpage->sound_struct.loop_end;
  infile >> soundpage->sound_struct.outer_cone_volume;
  infile >> soundpage->sound_struct.inner_cone_angle;
  infile >> soundpage->sound_struct.outer_cone_angle;
  infile >> soundpage->sound_struct.max_distance;
  infile >> soundpage->sound_struct.min_distance;
  infile >> soundpage->sound_struct.import_volume;
  // The full engine has a DEMO-only import_volume adjustment block here; it is
  // compiled out unless DEMO is defined (never in the mini build).

  // This is a valid new page
  soundpage->sound_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  if (!Old_table_method)
    return mng_ReadNewSoundPage(infile, soundpage);
  return 0; // old command-based table not supported in mini build
}
