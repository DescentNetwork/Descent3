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

// sound_info is a page sub-record (its name interleaves with the page-level
// raw_name on disk), so the page operators inline all of its fields in the
// original write order: flags, loop_start, loop_end, outer_cone_volume,
// inner_cone_angle, outer_cone_angle, max_distance, min_distance,
// import_volume.

byte_istream& operator>>(byte_istream& input, mngs_sound_page& data) {
  int16_t version = 0;
  input >> version;

  return input
         >> data.sound_struct.name
         >> data.raw_name
         >> data.sound_struct.flags
         >> data.sound_struct.loop_start
         >> data.sound_struct.loop_end
         >> data.sound_struct.outer_cone_volume
         >> data.sound_struct.inner_cone_angle
         >> data.sound_struct.outer_cone_angle
         >> data.sound_struct.max_distance
         >> data.sound_struct.min_distance
         >> data.sound_struct.import_volume;
}

byte_ostream& operator<<(byte_ostream& output, const mngs_sound_page& data) {
  return output
         << static_cast<int16_t>(SOUNDPAGE_VERSION)
         << data.sound_struct.name
         << data.raw_name
         << data.sound_struct.flags
         << data.sound_struct.loop_start
         << data.sound_struct.loop_end
         << data.sound_struct.outer_cone_volume
         << data.sound_struct.inner_cone_angle
         << data.sound_struct.outer_cone_angle
         << data.sound_struct.max_distance
         << data.sound_struct.min_distance
         << data.sound_struct.import_volume;
}

int mng_ReadNewSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  infile >> *soundpage;

  // This is a valid new page
  soundpage->sound_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  if (!Old_table_method)
    return mng_ReadNewSoundPage(infile, soundpage);
  return 0; // old command-based table not supported in mini build
}
