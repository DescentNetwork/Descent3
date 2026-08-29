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

// Ported from the full engine's bitmap/iff.cpp and bitmap/tga.cpp.
//
// The mini editor links only Qt + OpenGL + a HOG-capable cfile, so this file
// implements the OGF/IFF and TGA bitmap decoders (bm_iff_alloc_file /
// bm_tga_alloc_file) plus the file dispatch (bm_GetFileType /
// bm_AllocLoadFileBitmap) against the mini bm_* primitives.  Decoding the
// texel data here is what makes GameTextures[].bm_handle point at a real,
// resident bitmap with valid width/height, fixing textured-face rendering.

#define COMPRESS 1
#define WRITE_TINY 0
#define MIN_COMPRESS_WIDTH 65

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <vector>

#include <QtGlobal>

#include <posix_stream.h>

#include "mem.h"
#include "iff.h"
#include "byteswap.h"
#include "cfile.h"
#include "pstypes.h"
#include "bitmap.h"
#include "log.h"
#include "grdefs.h"


// Byte-oriented readers over an in-memory posix_istream (fmemopen).  These
// mirror the legacy CFILE read helpers exactly: the 2-arg IFF variants use
// BIG-endian shorts/ints (classic IFF storage), while the TGA/OGF path uses
// little-endian.  The `>>` operators already convert LE->native; big-endian
// values are byte-swapped explicitly via D3::convert_be, matching the old
// cf_ReadShort(f, false) / cf_ReadInt(f, false) behaviour.
namespace {
inline int8_t rdByte(posix_istream &in) { return (int8_t)in.get(); }
inline int16_t rdShortLE(posix_istream &in) {
  int16_t v = 0;
  in >> v;
  return v;
}
inline int32_t rdIntLE(posix_istream &in) {
  int32_t v = 0;
  in >> v;
  return v;
}
inline int16_t rdShortBE(posix_istream &in) {
  int16_t v = 0;
  in.read(&v, sizeof(v));
  return D3::convert_be(v);
}
inline int32_t rdIntBE(posix_istream &in) {
  int32_t v = 0;
  in.read(&v, sizeof(v));
  return D3::convert_be(v);
}
} // namespace

// Compression types
#define cmpNone 0
#define cmpByteRun1 1

// Masking types
#define mskNone 0
#define mskHasMask 1
#define mskHasTransparentColor 2

// Palette entry structure
struct pal_entry {
  uint8_t r, g, b;
};

// structure of the header in the file
struct iff_bitmap_header {
  int16_t w, h;                  // width and height of this bitmap
  int16_t x, y;                  // generally unused
  int16_t type;                  // see types above
  int16_t transparentcolor;      // which color is transparent (if any)
  int16_t pagewidth, pageheight; // width & height of source screen
  uint8_t nplanes;               // number of planes (8 for 256 color image)
  uint8_t masking, compression;  // see constants above
  uint8_t xaspect, yaspect;      // aspect ratio (usually 5/6)
  pal_entry palette[256];        // the palette for this bitmap
  uint8_t *raw_data;             // ptr to array of data
  int16_t row_size;              // offset to next row
};

int16_t iff_transparent_color;
int16_t iff_has_transparency; // 0=no transparency, 1=iff_transparent_color is valid

#define MAKE_SIG(a, b, c, d) (((int32_t)(a) << 24) + ((int32_t)(b) << 16) + ((c) << 8) + (d))

#define IFF_SIG_FORM 1
#define IFF_SIG_ILBM 2
#define IFF_SIG_BODY 3
#define IFF_SIG_BMHD 4
#define IFF_SIG_CMAP 5
#define IFF_SIG_UNKNOWN 6
#define IFF_SIG_PBM 7
#define IFF_SIG_ANIM 8
#define IFF_SIG_DELTA 9
#define IFF_SIG_ANHD 10

static int bm_iff_get_sig(posix_istream &f);
static int bm_iff_parse_bmhd(posix_istream &ifile, uint32_t len, iff_bitmap_header *bmheader);
static int bm_iff_parse_body(posix_istream &ifile, int len, iff_bitmap_header *bmheader);
static void bm_iff_skip_chunk(posix_istream &ifile, uint32_t len);
static int bm_iff_parse_delta(posix_istream &ifile, int len, iff_bitmap_header *bmheader);
static int bm_iff_parse_file(posix_istream &ifile, iff_bitmap_header *bmheader, iff_bitmap_header *prev_bm);
static void bm_iff_convert_8_to_16(int dest_bm, iff_bitmap_header *iffbm);

int bm_iff_get_sig(posix_istream &f) {
  char s[4];
  int i;

  for (i = 0; i < 4; i++)
    s[i] = (char)rdByte(f);

  if (!strncmp("ILBM", s, 4))
    return IFF_SIG_ILBM;
  if (!strncmp("BODY", s, 4))
    return IFF_SIG_BODY;
  if (!strncmp("CMAP", s, 4))
    return IFF_SIG_CMAP;
  if (!strncmp("BMHD", s, 4))
    return IFF_SIG_BMHD;
  if (!strncmp("FORM", s, 4))
    return IFF_SIG_FORM;
  if (!strncmp("PBM ", s, 4))
    return IFF_SIG_PBM;
  if (!strncmp("ANIM", s, 4))
    return IFF_SIG_ANIM;
  if (!strncmp("DLTA", s, 4))
    return IFF_SIG_DELTA;
  if (!strncmp("ANHD", s, 4))
    return IFF_SIG_ANHD;

  return (IFF_SIG_UNKNOWN);
}
int bm_iff_parse_bmhd(posix_istream &ifile, uint32_t len, iff_bitmap_header *bmheader) {
  len = len;

  bmheader->w = rdShortBE(ifile);
  bmheader->h = rdShortBE(ifile);
  bmheader->x = rdShortBE(ifile);
  bmheader->y = rdShortBE(ifile);

  bmheader->nplanes = rdByte(ifile);
  bmheader->masking = rdByte(ifile);
  bmheader->compression = rdByte(ifile);
  rdByte(ifile); /* skip pad */

  bmheader->transparentcolor = rdShortBE(ifile);
  bmheader->xaspect = rdByte(ifile);
  bmheader->yaspect = rdByte(ifile);

  bmheader->pagewidth = rdShortBE(ifile);
  bmheader->pageheight = rdShortBE(ifile);

  iff_transparent_color = bmheader->transparentcolor;

  iff_has_transparency = 0;

  if (bmheader->masking == mskHasTransparentColor)
    iff_has_transparency = 1;

  else if (bmheader->masking != mskNone && bmheader->masking != mskHasMask)
    return IFF_UNKNOWN_MASK;

  return IFF_NO_ERROR;
}

//  the buffer pointed to by raw_data is stuffed with a pointer to decompressed pixel data
int bm_iff_parse_body(posix_istream &ifile, int len, iff_bitmap_header *bmheader) {
  uint8_t *p = bmheader->raw_data;
  int width = 0, depth = 0, done = 0;

  if (bmheader->type == TYPE_PBM) {
    width = bmheader->w;
    depth = 1;
  } else if (bmheader->type == TYPE_ILBM) {
    width = (bmheader->w + 7) / 8;
    depth = bmheader->nplanes;
  }

  if (bmheader->compression == cmpNone) // no compression
  {
    for (int y = 0; y < bmheader->h; y++) {
      int x;

      for (x = 0; x < width * depth; x++)
        *p++ = rdByte(ifile);

      if (bmheader->masking == mskHasMask) {
        for (int i = 0; i < width; i++)
          rdByte(ifile); // skip mask!
      }

      if (bmheader->w & 1)
        rdByte(ifile);
    }

  } else if (bmheader->compression == cmpByteRun1) // compression
  {
    uint8_t *data_end = p + (bmheader->h * depth * width);
    uint8_t mask = (bmheader->masking == mskHasMask);
    int cur_width = 0, skip_mask = 0;
    int command;
    int plane = 0;

    while (!done) {
      if (p >= data_end) {
        done = 1;
        continue;
      }
      if (cur_width == width) {
        plane++;
        if ((plane == depth && !mask) || (plane == depth + 1 && mask)) {
          skip_mask = 0;
          plane = 0;
        }

        if (mask && plane == depth)
          skip_mask = 1;

        cur_width = 0;
      }

      command = rdByte(ifile);
      if (command >= 0 && command <= 127) {
        if (!skip_mask) {
          for (int i = 0; i < command + 1; i++)
            *p++ = rdByte(ifile);
        } else {
          for (int i = 0; i < (command + 1); i++)
            rdByte(ifile);
        }

        cur_width += (command + 1);
      } else if (command >= -127 && command < 0) {
        int run = (-command) + 1;
        int repeat_byte = rdByte(ifile);

        if (!skip_mask) {
          for (int i = 0; i < run; i++)
            *p++ = (uint8_t)repeat_byte;
        }

        cur_width += run;
      }
    }
  }

  return IFF_NO_ERROR;
}

//  the buffer pointed to by raw_data is stuffed with a pointer to bitplane pixel data
void bm_iff_skip_chunk(posix_istream &ifile, uint32_t len) {
  uint32_t i;

  for (i = 0; i < len; i++)
    rdByte(ifile);
}

// modify passed bitmap
int bm_iff_parse_delta(posix_istream &ifile, int len, iff_bitmap_header *bmheader) {
  uint8_t *p = bmheader->raw_data;
  int y;
  int32_t chunk_end = (int32_t)ifile.tell() + len;

  rdIntLE(ifile); // longword, seems to be equal to 4.  Don't know what it is

  for (y = 0; y < bmheader->h; y++) {
    uint8_t n_items;
    int cnt = bmheader->w;
    uint8_t code;

    n_items = (uint8_t)rdByte(ifile);

    while (n_items--) {
      code = (uint8_t)rdByte(ifile);

      if (code == 0) {
        uint8_t rep, val;

        rep = (uint8_t)rdByte(ifile);
        val = (uint8_t)rdByte(ifile);

        cnt -= rep;
        if (cnt == -1)
          rep--;
        while (rep--)
          *p++ = val;
      } else if (code > 0x80) { // skip
        cnt -= (code - 0x80);
        p += (code - 0x80);
        if (cnt == -1)
          p--;
      } else { // literal
        cnt -= code;
        if (cnt == -1)
          code--;

        while (code--)
          *p++ = (uint8_t)rdByte(ifile);

        if (cnt == -1)
          rdByte(ifile);
      }
    }

    if (cnt == -1) {
      if (!bmheader->w & 1)
        return IFF_CORRUPT;
    } else if (cnt)
      return IFF_CORRUPT;
  }

  if ((int32_t)ifile.tell() == chunk_end - 1) // pad
    rdByte(ifile);

  if ((int32_t)ifile.tell() != chunk_end) {
    Q_ASSERT(false);
    return IFF_CORRUPT;
  }

  else
    return IFF_NO_ERROR;
}

// read an PBM
// Pass pointer to opened file, and to empty bitmap_header structure, and form length
int bm_iff_parse_file(posix_istream &ifile, iff_bitmap_header *bmheader, iff_bitmap_header *prev_bm) {
  uint32_t sig, len;
  int done = 0;

  while (!done) {
    if (ifile.eof()) {
      done = 1;
      continue;
    }

    sig = bm_iff_get_sig(ifile);

    len = (uint32_t)rdIntBE(ifile);

    switch (sig) {
    case IFF_SIG_FORM: {
      bm_iff_get_sig(ifile);
      bmheader->type = TYPE_PBM;
      break;
    }
    case IFF_SIG_BMHD: {
      int ret;

      ret = bm_iff_parse_bmhd(ifile, len, bmheader);
      if (ret != IFF_NO_ERROR)
        return ret;
      else {

        bmheader->raw_data = (uint8_t *)mem_malloc(bmheader->w * bmheader->h);
        if (!bmheader->raw_data)
          return IFF_NO_MEM;
      }

    } break;
    case IFF_SIG_ANHD: {

      if (!prev_bm) {
        Q_ASSERT(false);
        return IFF_CORRUPT;
      }

      bmheader->w = prev_bm->w;
      bmheader->h = prev_bm->h;
      bmheader->type = prev_bm->type;
      bmheader->raw_data = (uint8_t *)mem_malloc(bmheader->w * bmheader->h);

      if (!bmheader->raw_data)
        return IFF_NO_MEM;

      memcpy(bmheader->raw_data, prev_bm->raw_data, bmheader->w * bmheader->h);

      if (len & 1)
        len++;
      bm_iff_skip_chunk(ifile, len);

      break;
    }

    case IFF_SIG_CMAP: {
      int ncolors = (int)(len / 3), cnum;
      uint8_t r, g, b;

      for (cnum = 0; cnum < ncolors; cnum++) {
        r = (uint8_t)rdByte(ifile);
        g = (uint8_t)rdByte(ifile);
        b = (uint8_t)rdByte(ifile);
        r >>= 2;
        bmheader->palette[cnum].r = r;
        g >>= 2;
        bmheader->palette[cnum].g = g;
        b >>= 2;
        bmheader->palette[cnum].b = b;
      }
      if (len & 1)
        rdByte(ifile);

    } break;

    case IFF_SIG_BODY: {
      int r;
      if ((r = bm_iff_parse_body(ifile, len, bmheader)) != IFF_NO_ERROR)
        return r;
      done = 1;
      break;
    }
    case IFF_SIG_DELTA: {
      int r;
      if ((r = bm_iff_parse_delta(ifile, len, bmheader)) != IFF_NO_ERROR)
        return r;
      done = 1;
      break;
    }

    default:
      // Don't know this chunk
      if (len & 1)
        len++;
      bm_iff_skip_chunk(ifile, len);
      break;
    }
  }

  return IFF_NO_ERROR; /* ok! */
}

void bm_iff_convert_8_to_16(int dest_bm, iff_bitmap_header *iffbm) {
  uint16_t *data;

  data = (uint16_t *)bm_data(dest_bm, 0);

  for (int i = 0; i < iffbm->h; i++)
    for (int t = 0; t < iffbm->w; t++) {
      uint16_t pixel;
      uint8_t c = iffbm->raw_data[i * iffbm->w + t];

      int r = iffbm->palette[c].r >> 1;
      int g = iffbm->palette[c].g >> 1;
      int b = iffbm->palette[c].b >> 1;

      pixel = OPAQUE_FLAG | (r << 10) | (g << 5) | b;
      if (c == iffbm->transparentcolor)
        pixel = NEW_TRANSPARENT_COLOR;

      data[i * bm_w(dest_bm, 0) + t] = pixel;
    }
}

// Loads an iff into a structure, allocs bitmap memory and converts 8 bit iff file into
// 16bit bitmap
// Returns bitmap handle on success, or -1 if failed
int bm_iff_alloc_file(posix_istream &ifile) {
  int ret; // return code
  iff_bitmap_header bmheader;
  int src_bm;
  char cur_sig[4];

  // Ignore FORM and form length
  rdIntLE(ifile);
  rdIntLE(ifile);

  // check if this an ILBM
  for (int i = 0; i < 4; i++)
    cur_sig[i] = (char)rdByte(ifile);

  if (strncmp("PBM ", cur_sig, 4)) {
    LOG_ERROR("IFF file isn't a PBM...aborting.");
    return -1;
  }
  bmheader.type = TYPE_PBM;

  ret = bm_iff_parse_file(ifile, &bmheader, NULL);

  if (ret != IFF_NO_ERROR) {
    LOG_ERROR("Couldn't load IFF file.");
    return -1;
  }

  // Alloc our bitmap
  src_bm = bm_AllocBitmap(bmheader.w, bmheader.h, 0);
  if (src_bm < 0) {
    mem_free(bmheader.raw_data);
    return -1;
  }

  // Convert our 8 bit bitmap to 16bit
  bm_iff_convert_8_to_16(src_bm, &bmheader);
  free(bmheader.raw_data);

  return src_bm;
}

// ----------------------------------------------------------------------------
// TGA / OGF decoder (ported from bitmap/tga.cpp).  Handles standard
// uncompressed (type 2), run-length (type 10) and the Outrage compressed OGF
// variants that D3 textures ship as.
// ----------------------------------------------------------------------------

static char *Tga_file_data = NULL;
static int Fake_pos = 0;
static int Bad_tga = 0;
static int Fake_file_size = 0;

static inline char tga_read_byte();
static inline int tga_read_int();
static inline int16_t tga_read_short();
static uint16_t bm_tga_translate_pixel(int pixel, int format);

inline char tga_read_byte() {
  // Check for bad file
  if (Fake_pos + 1 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  return Tga_file_data[Fake_pos++];
}
inline int tga_read_int() {
  int i;

  // Check for bad file
  if (Fake_pos + 4 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  i = *(int *)(Tga_file_data + Fake_pos);
  Fake_pos += 4;

  return INTEL_INT(i);
}
inline int16_t tga_read_short() {
  int16_t i;

  // Check for bad file
  if (Fake_pos + 2 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  memcpy(&i, Tga_file_data + Fake_pos, sizeof(i));
  Fake_pos += 2;

  return INTEL_SHORT(i);
}

uint16_t bm_tga_translate_pixel(int pixel, int format) {
  int red = ((pixel >> 16) & 0xFF);
  int green = ((pixel >> 8) & 0xFF);
  int blue = ((pixel) & 0xFF);
  int alpha = ((pixel >> 24) & 0xFF);
  uint16_t newpix;

  if (format == BITMAP_FORMAT_4444) {
    int newred = red >> 4;
    int newgreen = green >> 4;
    int newblue = blue >> 4;
    int newalpha = alpha >> 4;
    newpix = (newalpha << 12) | (newred << 8) | (newgreen << 4) | (newblue);
  } else {
    int newred = red >> 3;
    int newgreen = green >> 3;
    int newblue = blue >> 3;

    newpix = OPAQUE_FLAG | (newred << 10) | (newgreen << 5) | (newblue);

    if (alpha == 0)
      newpix = NEW_TRANSPARENT_COLOR;
  }

  return newpix;
}

static int bm_tga_read_outrage_compressed16(int n, int num_mips, int type) {
  uint16_t *dest_data;
  uint16_t pixel;
  int width, height;
  int m;

  for (m = 0; m < num_mips; m++) {

    width = bm_w(n, m);
    height = bm_h(n, m);

    int total = height * width;
    int count = 0;

    dest_data = (uint16_t *)bm_data(n, m);

    while (count != total) {
      Q_ASSERT(count < total);

      uint8_t command = tga_read_byte();

      if (Bad_tga)
        return 0;

      if (command == 0) // next pixel is raw
      {
        pixel = tga_read_short();

        if (Bad_tga)
          return 0;

        if (type != OUTRAGE_1555_COMPRESSED_MIPPED && type != OUTRAGE_4444_COMPRESSED_MIPPED) {
          if (pixel == 0x07e0)
            pixel = NEW_TRANSPARENT_COLOR;
          else {
            int r = ((pixel & 0xF800) >> 11) << 3;
            int g = ((pixel & 0x07e0) >> 5) << 2;
            int b = (pixel & 0x001f) << 3;

            pixel = OPAQUE_FLAG | GR_RGB16(r, g, b);
          }
        }

        int i = count / width;
        int t = count % width;
        dest_data[i * width + t] = pixel;
        count++;
      } else if (command >= 2 && command <= 250) // next pixel is run of pixels
      {
        pixel = tga_read_short();

        if (Bad_tga)
          return 0;

        if (type != OUTRAGE_1555_COMPRESSED_MIPPED && type != OUTRAGE_4444_COMPRESSED_MIPPED) {
          if (pixel == 0x07e0)
            pixel = NEW_TRANSPARENT_COLOR;
          else {
            int r = ((pixel & 0xF800) >> 11) << 3;
            int g = ((pixel & 0x07e0) >> 5) << 2;
            int b = (pixel & 0x001f) << 3;

            pixel = OPAQUE_FLAG | GR_RGB16(r, g, b);
          }
        }

        for (int k = 0; k < command; k++) {
          int i = count / width;
          int t = count % width;
          dest_data[i * width + t] = pixel;
          count++;
        }
      } else
        Q_ASSERT(false); // bad compression run
    }
  }

  return 1;
}

// Loads a tga or ogf file into a bitmap...returns handle to bm or -1 on error
int bm_tga_alloc_file(posix_istream &infile, char *name, int format) {
  uint8_t image_id_len, color_map_type, image_type, pixsize, descriptor;
  uint8_t upside_down = 0;
  uint16_t width, height;
  uint32_t pixel;
  int i, t, n, savepos = 0;
  int mipped = 0;
  int num_mips = 1;
  int read_ok = 1;

  image_id_len = (uint8_t)rdByte(infile);
  color_map_type = (uint8_t)rdByte(infile);
  image_type = (uint8_t)rdByte(infile);

  if (color_map_type != 0 ||
      (image_type != 10 && image_type != 2 && image_type != OUTRAGE_TGA_TYPE && image_type != OUTRAGE_COMPRESSED_OGF &&
       image_type != OUTRAGE_COMPRESSED_MIPPED && image_type != OUTRAGE_NEW_COMPRESSED_MIPPED &&
       image_type != OUTRAGE_1555_COMPRESSED_MIPPED && image_type != OUTRAGE_4444_COMPRESSED_MIPPED)) {
    LOG_ERROR("bm_tga: Can't read this type of TGA.");
    return -1;
  }

  if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
      image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_TGA_TYPE ||
      image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_OGF ||
      image_type == OUTRAGE_COMPRESSED_OGF_8BIT) {
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_NEW_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_1555_COMPRESSED_MIPPED) {
      // cf_ReadString(name, BITMAP_NAME_LEN - 1, infile): read until NUL/EOF.
      size_t idx = 0;
      while (idx < (size_t)(BITMAP_NAME_LEN - 1)) {
        uint8_t c = infile.get();
        if (infile.eof() || c == 0)
          break;
        name[idx++] = (char)c;
      }
      name[idx] = 0;
    } else {
      for (i = 0; i < BITMAP_NAME_LEN; i++)
        name[i] = (char)rdByte(infile);
    }
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_NEW_COMPRESSED_MIPPED)
      num_mips = rdByte(infile);
    else
      num_mips = 1;

    if (num_mips > 1)
      mipped = 1;
  }

  for (i = 0; i < 9; i++) // ingore next 9 bytes
    rdByte(infile);

  width = (uint16_t)rdShortLE(infile);
  height = (uint16_t)rdShortLE(infile);
  pixsize = (uint8_t)rdByte(infile);

  if (pixsize != 32 && pixsize != 24) {
    LOG_ERROR("bm_tga: This file has a pixsize of field of %d, it should be 32.", pixsize);
    return -1;
  }

  descriptor = (uint8_t)rdByte(infile);
  if (((descriptor & 0x0F) != 8) && ((descriptor & 0x0F) != 0)) {
    LOG_ERROR("bm_tga: Descriptor field & 0x0F must be 8 or 0, but this is %d.", descriptor & 0x0F);
    return -1;
  }

  for (i = 0; i < image_id_len; i++)
    rdByte(infile);

  n = bm_AllocBitmap(width, height, mipped * ((width * height * 2) / 3));

  if (format == BITMAP_FORMAT_4444 || image_type == OUTRAGE_4444_COMPRESSED_MIPPED)
    GameBitmaps[n].format = BITMAP_FORMAT_4444;

  // Copy the name
  strcpy(GameBitmaps[n].name, name);

  if (mipped)
    GameBitmaps[n].flags |= BF_MIPMAPPED;

  if (n < 0) {
    LOG_ERROR("bm_tga: Failed to allocate memory.");
    Q_ASSERT(false);
    return -1;
  }

  upside_down = (descriptor & 0x20) >> 5;
  upside_down = 1 - upside_down;

  // Load the actual bitmap data in, converting it from 32 bit to 16 bit, and replacing
  // that pesky transparency color without our replacement

  if (image_type == 10 || image_type == 2) {
    if (image_type == 10) // compressed tga
    {
      int total = 0;

      while (total < (height * width)) {
        uint8_t command = (uint8_t)rdByte(infile);
        uint8_t len = (command & 127) + 1;

        if (command & 128) // rle chunk
        {
          if (pixsize == 32)
            pixel = (uint32_t)rdIntLE(infile);
          else {
            int r, g, b;
            r = (uint8_t)rdByte(infile);
            g = (uint8_t)rdByte(infile);
            b = (uint8_t)rdByte(infile);
            pixel = (255 << 24) | (r << 16) | (g << 8) | b;
          }

          uint16_t newpix = bm_tga_translate_pixel(pixel, format);

          for (int k = 0; k < len; k++, total++) {
            i = total / width;
            t = total % width;

            if (upside_down)
              GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
            else
              GameBitmaps[n].data16[i * width + t] = newpix;
          }
        } else // raw chunk
        {
          for (int k = 0; k < len; k++, total++) {
            if (pixsize == 32)
              pixel = (uint32_t)rdIntLE(infile);
            else {
              int r, g, b;
              b = (uint8_t)rdByte(infile);
              g = (uint8_t)rdByte(infile);
              r = (uint8_t)rdByte(infile);
              pixel = (255 << 24) | (r << 16) | (g << 8) | b;
            }
            uint16_t newpix = bm_tga_translate_pixel(pixel, format);

            i = total / width;
            t = total % width;

            if (upside_down)
              GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
            else
              GameBitmaps[n].data16[i * width + t] = newpix;
          }
        }
      }
    } else // uncompressed TGA
    {
      for (i = 0; i < height; i++) {
        for (t = 0; t < width; t++) {
          if (pixsize == 32)
            pixel = (uint32_t)rdIntLE(infile);
          else {
            int r, g, b;
            b = (uint8_t)rdByte(infile);
            g = (uint8_t)rdByte(infile);
            r = (uint8_t)rdByte(infile);
            pixel = (255 << 24) | (r << 16) | (g << 8) | b;
          }

          uint16_t newpix = bm_tga_translate_pixel(pixel, format);

          if (upside_down)
            GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
          else
            GameBitmaps[n].data16[i * width + t] = newpix;
        }
      }
    }
  } else if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
             image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_MIPPED ||
             image_type == OUTRAGE_COMPRESSED_OGF || image_type == OUTRAGE_COMPRESSED_OGF_8BIT) // COMPRESSED OGF
  {
    // read this ogf in all at once (much faster)

    savepos = (int)infile.tell();
    infile.seek(0, std::ios_base::end);
    int lastpos = (int)infile.tell();
    int numleft = lastpos - savepos;

    infile.seek(savepos, std::ios_base::beg);

    Tga_file_data = mem_rmalloc<char>(numleft);
    Q_ASSERT(Tga_file_data != NULL);
    Fake_pos = 0;
    Bad_tga = 0;
    Fake_file_size = numleft;

    infile.read(Tga_file_data, numleft);

    read_ok = bm_tga_read_outrage_compressed16(n, num_mips, image_type);
  }

  else
    Q_ASSERT(false); // Get Jason

  if (Tga_file_data != NULL) {
    mem_free(Tga_file_data);
    Tga_file_data = NULL;
    infile.seek(savepos + Fake_pos, std::ios_base::beg);
  }

  if (!read_ok)
    return -1;
  else
    return (n);
}

// ----------------------------------------------------------------------------
// File dispatch (ported from bitmap/bitmain.cpp: bm_GetFileType / the
// bm_AllocLoadFileBitmap file-typing portion).  Decides IFF vs TGA by peeking
// at the first 4 bytes ("FORM" => IFF, otherwise TGA).
// ----------------------------------------------------------------------------

#define BM_FILETYPE_PCX 0
#define BM_FILETYPE_IFF 1
#define BM_FILETYPE_TGA 2

static int bm_GetFileType(posix_istream &infile, const char *dest) {
  char iffcheck[4];
  int i;
  // First, check if it is a PCX
  i = strlen(dest);
  if (i >= 4 && dest[i - 4] == '.' && (dest[i - 3] == 'p' || dest[i - 3] == 'P') && (dest[i - 2] == 'c' || dest[i - 2] == 'C') &&
      (dest[i - 1] == 'x' || dest[i - 1] == 'X'))
    return BM_FILETYPE_PCX;
  // How about an IFF?
  for (i = 0; i < 4; i++)
    iffcheck[i] = (char)rdByte(infile);
  infile.seek(0, std::ios_base::beg);
  if (!strncmp("FORM", iffcheck, 4))
    return BM_FILETYPE_IFF;
  // Lastly, just default to possible TGA or OGF
  return BM_FILETYPE_TGA;
}

// Allocs and loads a bitmap from a full in-memory payload (a HOG entry or a
// file read from disk).  `data`/`size` is wrapped in an fmemopen posix_istream
// so all the decoders above read from memory instead of cfopen()/CFILE.
// Returns the handle of the loaded bitmap, or -1 if something is wrong.
int bm_LoadBitmapFromMemory(const uint8_t *data, size_t size, const char *fname, int format, int mipped) {
  posix_istream infile(const_cast<uint8_t *>(data), size, std::ios_base::in);
  if (!infile.is_open()) {
    LOG_ERROR("bm_LoadBitmapFromMemory: Can't open in-memory stream for %s.", fname);
    return -1;
  }

  (void)mipped;
  char name[BITMAP_NAME_LEN];
  name[0] = 0;

  int src_bm = -1;
  int filetype = bm_GetFileType(infile, fname);

  switch (filetype) {
  case BM_FILETYPE_IFF:
    src_bm = bm_iff_alloc_file(infile);
    break;
  case BM_FILETYPE_TGA:
    // reads a tga or an outrage graphics file (ogf)
    src_bm = bm_tga_alloc_file(infile, name, format);
    break;
  case BM_FILETYPE_PCX:
  default:
    LOG_ERROR("bm_LoadBitmapFromMemory: PCX or unknown file type not supported for %s.", fname);
    src_bm = -1;
    break;
  }
  if (src_bm < 0) {
    LOG_ERROR("Couldn't load %s.", fname);
    return -1;
  }

  // Assign a stable name to the bitmap.
  if (name[0] != 0) {
    strncpy(GameBitmaps[src_bm].name, name, BITMAP_NAME_LEN - 1);
    GameBitmaps[src_bm].name[BITMAP_NAME_LEN - 1] = 0;
  } else {
    std::filesystem::path p(fname);
    std::string stem = p.stem().string();
    strncpy(GameBitmaps[src_bm].name, stem.c_str(), BITMAP_NAME_LEN - 1);
    GameBitmaps[src_bm].name[BITMAP_NAME_LEN - 1] = 0;
  }

  return src_bm;
}
