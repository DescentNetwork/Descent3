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
#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Texture page (ported from texpage.cpp : 508-759)
//-----------------------------------------------------------------------------
/*
static void mng_InitTexturePage(mngs_texture_page *texpage) {
  // Member-wise reset (NOT memset — the struct now holds std::string members
  // that memset would corrupt).
  *texpage = mngs_texture_page{};
  texpage->tex_struct = {}
  texpage->bitmap_name.clear();
  texpage->destroy_name.clear();
  texpage->sound_name.clear();
}
*/
// Reads a texture page from an open file.  Returns 0 on error.
byte_istream& operator >>(byte_istream& input, mngs_texture_page& data)
{
  input >> version_t { 7 }
        >> data.tex_struct.name
        >> data.bitmap_name
        >> data.destroy_name
        >> data.tex_struct;

  if (data.tex_struct.flags.procedural)
  {
    input >> data.procs;
    if(data.procs.data.empty())
      data.tex_struct.flags.procedural = false;
  }

  if (data.destroy_name == "INVALID")
    data.destroy_name.clear();

  return input
         >> data.sound_name
         >> data.tex_struct.sound_volume;
}

// Writes a texture page in the current (TEXPAGE_VERSION) format, the exact
// mirror of operator>> (same field order and encodings).
byte_ostream& operator <<(byte_ostream& output, const mngs_texture_page& data)
{
  output << version_t { 7 }
         << data.tex_struct.name
         << data.bitmap_name
         << data.destroy_name
         << data.tex_struct;

  if (data.tex_struct.flags.procedural)
    output << data.procs;

  return output
         << data.sound_name
         << data.tex_struct.sound_volume;
}

bool mng_ReadNewTexturePage(posix_istream &infile, mngs_texture_page *texpage)
{
  //mng_InitTexturePage(texpage);
  *texpage = {};
  infile >> *texpage;

  return true; // successfully read
}

bool mng_ReadTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  if (!Old_table_method)
    return mng_ReadNewTexturePage(infile, texpage);
  return false; // old command-based table not supported in mini build
}
