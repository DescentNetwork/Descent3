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

// Megacell page reader (ported from megapage.cpp : 175-192).

#include "megapage.h"

#include <cstdint>
#include <cstring>
#include <string>

#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Megacell page (ported from megapage.cpp : 175-192)
//-----------------------------------------------------------------------------

int mng_ReadNewMegacellPage(posix_istream &infile, mngs_megacell_page *megacellpage) {
  int i;
  *megacellpage = mngs_megacell_page{};
  /* int version = */ int16_t v; infile >> v;

  infile >> megacellpage->megacell_struct.name;

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    infile >> megacellpage->cellname[i];

  infile >> megacellpage->megacell_struct.width;
  infile >> megacellpage->megacell_struct.height;
  // This is a valid new page
  megacellpage->megacell_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadMegacellPage(posix_istream &infile, mngs_megacell_page *megacellpage) {
  if (!Old_table_method)
    return mng_ReadNewMegacellPage(infile, megacellpage);
  return 0; // old command-based table not supported in mini build
}
