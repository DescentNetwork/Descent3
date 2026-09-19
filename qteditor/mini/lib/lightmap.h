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

#ifndef LIGHTMAP_H
#define LIGHTMAP_H

#include <cstdint>
#include <optional>
#include <vector>

#define MAX_LIGHTMAPS (65534)
#define BAD_LM_INDEX 65535

// lightmap flags
#define LF_CHANGED 1   // this bitmap has changed since last frame (useful for hardware cacheing)
#define LF_LIMITS 2    // This lightmap has a specific area that has changed since last frame
#define LF_WRAP 4      // This lightmap should be drawn with wrapping (not clamping)
#define LF_BRAND_NEW 8 // This lightmap is brand new and hasn't been to the video card yet

struct bms_lightmap {
  uint8_t width, height; // Width and height in pixels
  std::vector<std::vector<uint16_t>> data; // height rows, each width 16bit texels wide

  uint16_t used;
  uint8_t flags;
  int16_t cache_slot;         // for the renderers use
  uint8_t square_res;         // for renderers use
  uint8_t cx1, cy1, cx2, cy2; // Change x and y coords
};

// The lightmap table.  Grows on demand as fresh handles are created (the
// original's MAX_LIGHTMAPS array is kept as a hard upper bound so handles stay
// inside uint16_t range; the test suite asserts handles < MAX_LIGHTMAPS).
extern std::vector<bms_lightmap> GameLightmaps;

// Sets all the lightmaps to unused
void lm_InitLightmaps();

void lm_ShutdownLightmaps(void);

// Allocs a lightmap of w x h size
// Returns 16-bit lightmap handle (index into GameLightmaps) if
// successful, nullopt if otherwise
std::optional<uint16_t> lm_AllocLightmap(int w, int h);

// Given a handle, frees the lightmap memory and flags this lightmap as unused
void lm_FreeLightmap(int handle);

// returns a lightmaps width (stored as uint8) else nullopt if something is wrong
std::optional<uint8_t> lm_w(int handle);

// returns a lightmaps height (stored as uint8), else nullopt if something is wrong
std::optional<uint8_t> lm_h(int handle);

// returns a lightmaps data as height rows of width texels
std::vector<std::vector<uint16_t>> &lm_data(int handle);

#endif
