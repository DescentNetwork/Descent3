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

#ifndef _IFF_H
#define _IFF_H

#include <cstddef>
#include <cstdint>
#include <posix_stream.h>
#include "bitmap.h"

// Error codes for read & write routines

enum class iff_error : uint8_t {
  no_error = 0,      // everything is fine, have a nice day
  no_mem = 1,        // not enough mem for loading or processing
  unknown_form = 2,  // IFF file, but not a bitmap
  not_iff = 3,       // this isn't even an IFF file
  no_file = 4,       // cannot find or open file
  bad_bm_type = 5,   // tried to save invalid type, like BM_RGB15
  corrupt = 6,       // bad data in file
  form_anim = 7,     // this is an anim, with non-anim load rtn
  form_bitmap = 8,   // this is not an anim, with anim load rtn
  too_many_bms = 9,  // anim read had more bitmaps than room for
  unknown_mask = 10, // unknown masking type
  read_error = 11,   // error reading from file
  bm_mismatch = 12,  // bm being loaded doesn't match bm loaded into
};

// Type values for iff bitmaps
enum class iff_bitmap_type : int16_t {
  pbm = 0,
  ilbm = 1,
};

// Loads an IFF file, returning bitmap handle or -1 if error
int bm_iff_alloc_file(posix_istream &ifile);

// Loads a tga or ogf file into a bitmap...returns handle to bm or -1 on error
int bm_tga_alloc_file(posix_istream &infile, char *name, bitmap_format format = bitmap_format::standard);

// Allocs and loads a bitmap from a fully-resident in-memory payload.
// Returns the handle of the loaded bitmap, or -1 on error.
int bm_LoadBitmapFromMemory(const uint8_t *data, size_t size, const char *fname, bitmap_format format, int mipped);

// Loads a pcx file and converts it to 16 bit.  Returns bitmap handle or -1 on error
int bm_pcx_alloc_file(struct CFILE* infile);

// Pages in bitmap index n.  Returns 1 if successful, 0 if not
int bm_page_in_file(int n);

int bm_iff_read_animbrush(const char *ifilename, int *bm_list);

#endif
