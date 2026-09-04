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

#include "gametexture.h" // texture, TF_PROCEDURAL
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
int mng_ReadNewTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  int i;

  mng_InitTexturePage(texpage);

  int16_t version = 0;
  infile >> version;

  infile >> texpage->tex_struct.name;
  infile >> texpage->bitmap_name;
  infile >> texpage->destroy_name;

  infile >> texpage->tex_struct.r;
  infile >> texpage->tex_struct.g;
  infile >> texpage->tex_struct.b;
  infile >> texpage->tex_struct.alpha;

  infile >> texpage->tex_struct.speed;

  infile >> texpage->tex_struct.slide_u;
  infile >> texpage->tex_struct.slide_v;
  infile >> texpage->tex_struct.reflectivity;

  infile >> texpage->tex_struct.corona_type;
  infile >> texpage->tex_struct.damage;
  {
    uint32_t raw_flags = 0;
    infile >> raw_flags;
    std::memcpy(&texpage->tex_struct.flags, &raw_flags, sizeof(raw_flags));
  }

  if (texpage->tex_struct.flags.procedural) {
    for (i = 0; i < 255; i++) {
      uint16_t val = 0;
      infile >> val;
      texpage->proc_palette[i] = val;
    }

    infile >> texpage->proc_heat;
    infile >> texpage->proc_light;
    infile >> texpage->proc_thickness;
    infile >> texpage->proc_evaluation_time;

    if (version >= 6) {
      infile >> texpage->osc_time;
      infile >> texpage->osc_value;
    }

    int16_t npe = 0;
    infile >> npe;
    texpage->num_proc_elements = npe;

    if (texpage->num_proc_elements > MAX_PROC_ELEMENTS) {
      LOG_ERROR("Warning! Too many procedural elements!");
      Q_ASSERT(false);
    }

    for (i = 0; i < texpage->num_proc_elements; i++) {
      infile >> texpage->proc_type[i];
      infile >> texpage->proc_frequency[i];
      infile >> texpage->proc_speed[i];
      infile >> texpage->proc_size[i];
      infile >> texpage->proc_x1[i];
      infile >> texpage->proc_y1[i];

      infile >> texpage->proc_x2[i];
      infile >> texpage->proc_y2[i];
    }
  }

  if (texpage->tex_struct.flags.procedural) {
    if (texpage->num_proc_elements == 0)
      texpage->tex_struct.flags.procedural = false;
  }

  if (std::string(texpage->destroy_name).compare(0, 7, "INVALID") == 0)
    texpage->destroy_name.clear();

  if (version >= 5) {

    if (version < 7) {
      // Kill buggy version of sound resolving code
      int s;
      infile >> s;
      texpage->tex_struct.sound = s;
      texpage->tex_struct.sound = -1;
      texpage->sound_name.clear();
    } else {
      infile >> texpage->sound_name;
    }

    infile >> texpage->tex_struct.sound_volume;
  } else {
    texpage->tex_struct.sound = -1;
    texpage->tex_struct.sound_volume = 1.0;
  }

  texpage->tex_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  if (!Old_table_method)
    return mng_ReadNewTexturePage(infile, texpage);
  return 0; // old command-based table not supported in mini build
}
