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

// Texture page reader (ported from texpage.cpp : 508-759).

#ifndef PAGENAME_LEN
#define PAGENAME_LEN 35
#endif

#include "gametexture.h" // texture flags
#include "texpage.h"

#include <cstring>
#include <cstdint>
#include <string>
#include <QtGlobal>

#include "manage.h"
#include "log.h"     // LOG_ERROR
#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Texture page (ported from texpage.cpp : 508-759)
//-----------------------------------------------------------------------------

static void mng_InitTexturePage(mngs_texture_page *texpage) {
  // Member-wise reset (NOT memset — the struct now holds std::string members
  // that memset would corrupt).
  *texpage = mngs_texture_page{};
  texpage->proc_thickness = 4;
  texpage->proc_heat = 200;
  texpage->proc_light = 1;
  texpage->num_proc_elements = 0;
  texpage->proc_evaluation_time = 0;
  texpage->osc_time = 0;
  texpage->osc_value = 8;

  texpage->tex_struct.alpha = 1.0;
  texpage->tex_struct.speed = 1.0;
  texpage->tex_struct.reflectivity = .5;
  texpage->tex_struct.corona_type = 0;
  texpage->tex_struct.slide_v = 0;
  texpage->tex_struct.slide_u = 0;
  texpage->tex_struct.bumpmap = -1;
  texpage->tex_struct.sound = -1;
  texpage->tex_struct.sound_volume = 1.0;

  texpage->bitmap_name.clear();
  texpage->destroy_name.clear();
  texpage->sound_name.clear();
}

// Reads a texture page from an open file.  Returns 0 on error.
byte_istream& operator>>(byte_istream& input, mngs_texture_page& data) {
  int16_t version = 0;
  input >> version;

  input >> data.tex_struct.name
        >> data.bitmap_name
        >> data.destroy_name;

  input >> data.tex_struct.r
        >> data.tex_struct.g
        >> data.tex_struct.b
        >> data.tex_struct.alpha
        >> data.tex_struct.speed
        >> data.tex_struct.slide_u
        >> data.tex_struct.slide_v
        >> data.tex_struct.reflectivity
        >> data.tex_struct.corona_type
        >> data.tex_struct.damage;
  {
    uint32_t raw_flags = 0;
    input >> raw_flags;
    std::memcpy(&data.tex_struct.flags, &raw_flags, sizeof(raw_flags));
  }

  if (data.tex_struct.flags.procedural) {
    data.proc_palette.fill(0);
    for (int i = 0; i < 255; i++) {
      uint16_t val = 0;
      input >> val;
      data.proc_palette[i] = val;
    }

    input >> data.proc_heat
          >> data.proc_light
          >> data.proc_thickness
          >> data.proc_evaluation_time;

    if (version >= 6)
      input >> data.osc_time >> data.osc_value;

    int16_t npe = 0;
    input >> npe;
    data.num_proc_elements = npe;

    if (data.num_proc_elements > MAX_PROC_ELEMENTS) {
      LOG_ERROR("Warning! Too many procedural elements!");
      Q_ASSERT(false);
    }

    data.proc_type.resize(data.num_proc_elements);
    data.proc_frequency.resize(data.num_proc_elements);
    data.proc_speed.resize(data.num_proc_elements);
    data.proc_size.resize(data.num_proc_elements);
    data.proc_x1.resize(data.num_proc_elements);
    data.proc_y1.resize(data.num_proc_elements);
    data.proc_x2.resize(data.num_proc_elements);
    data.proc_y2.resize(data.num_proc_elements);

    for (int i = 0; i < data.num_proc_elements; i++)
      input >> data.proc_type[i]
            >> data.proc_frequency[i]
            >> data.proc_speed[i]
            >> data.proc_size[i]
            >> data.proc_x1[i]
            >> data.proc_y1[i]
            >> data.proc_x2[i]
            >> data.proc_y2[i];
  }

  if (data.tex_struct.flags.procedural) {
    if (data.num_proc_elements == 0)
      data.tex_struct.flags.procedural = false;
  }

  if (std::string(data.destroy_name).compare(0, 7, "INVALID") == 0)
    data.destroy_name.clear();

  if (version >= 5) {

    if (version < 7) {
      // Kill buggy version of sound resolving code
      int s;
      input >> s;
      data.tex_struct.sound = s;
      data.tex_struct.sound = -1;
      data.sound_name.clear();
    } else {
      input >> data.sound_name;
    }

    input >> data.tex_struct.sound_volume;
  } else {
    data.tex_struct.sound = -1;
    data.tex_struct.sound_volume = 1.0;
  }

  return input;
}

// Writes a texture page in the current (TEXPAGE_VERSION) format, the exact
// mirror of operator>> (same field order and encodings).
byte_ostream& operator<<(byte_ostream& output, const mngs_texture_page& data) {
  output << static_cast<int16_t>(TEXPAGE_VERSION);
  output << data.tex_struct.name
         << data.bitmap_name
         << data.destroy_name
         << data.tex_struct.r
         << data.tex_struct.g
         << data.tex_struct.b
         << data.tex_struct.alpha
         << data.tex_struct.speed
         << data.tex_struct.slide_u
         << data.tex_struct.slide_v
         << data.tex_struct.reflectivity
         << data.tex_struct.corona_type
         << data.tex_struct.damage;
  {
    uint32_t raw_flags = 0;
    std::memcpy(&raw_flags, &data.tex_struct.flags, sizeof(raw_flags));
    output << raw_flags;
  }

  if (data.tex_struct.flags.procedural) {
    for (int i = 0; i < 255; i++)
      output << data.proc_palette[i];

    output << data.proc_heat
           << data.proc_light
           << data.proc_thickness
           << data.proc_evaluation_time
           << data.osc_time
           << data.osc_value
           << static_cast<int16_t>(data.num_proc_elements);
    for (int i = 0; i < data.num_proc_elements; i++)
      output << data.proc_type[i]
             << data.proc_frequency[i]
             << data.proc_speed[i]
             << data.proc_size[i]
             << data.proc_x1[i]
             << data.proc_y1[i]
             << data.proc_x2[i]
             << data.proc_y2[i];
  }

  output << data.sound_name
         << data.tex_struct.sound_volume;
  return output;
}

int mng_ReadNewTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  int i;

  mng_InitTexturePage(texpage);

  infile >> *texpage;

  texpage->tex_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  if (!Old_table_method)
    return mng_ReadNewTexturePage(infile, texpage);
  return 0; // old command-based table not supported in mini build
}
