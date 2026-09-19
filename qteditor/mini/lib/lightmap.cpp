/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral lightmap texture table (ported from bitmap/lightmap.cpp).  The
 * mini keeps the same free-list + shared handle scheme so the level loader can
 * round-trip the NLMP chunk byte-for-byte.  Renderer-specific fields
 * (cache_slot, square_res) are retained in the struct but never used.
 */

#include "lightmap.h"

#include <QtGlobal>

#include <algorithm>
#include <optional>

static int Num_of_lightmaps = 0;
static std::vector<uint16_t> Free_lightmap_list;
std::vector<bms_lightmap> GameLightmaps;

static bool f_lm_initialized = false;

// Sets all the lightmaps to unused
void lm_InitLightmaps() {
  GameLightmaps.clear();
  Free_lightmap_list.clear();
  Num_of_lightmaps = 0;
  f_lm_initialized = true;
}

void lm_ShutdownLightmaps(void) {
  for (uint32_t i = 0; i < GameLightmaps.size(); i++) {
    while (GameLightmaps[i].used > 0)
      lm_FreeLightmap(i);
  }
}

// Allocs a lightmap of w x h size
// Returns 16-bit lightmap handle if successful, nullopt if otherwise
std::optional<uint16_t> lm_AllocLightmap(int w, int h) {
  if (!f_lm_initialized)
    lm_InitLightmaps();

  // The free list hands out fresh handles as identity values (its slot index).
  // When the cursor reaches the current frontier the table must first grow by
  // one slot whose free-list value equals its own index; MAX_LIGHTMAPS stays
  // as the hard cap keeping handles inside uint16_t range.
  if (Num_of_lightmaps == static_cast<int>(GameLightmaps.size())) {
    if (GameLightmaps.size() >= MAX_LIGHTMAPS)
      return std::nullopt; // Ran out of lightmaps!
    GameLightmaps.push_back(bms_lightmap{});
    Free_lightmap_list.push_back(static_cast<uint16_t>(GameLightmaps.size() - 1));
  }

  int n = Free_lightmap_list[Num_of_lightmaps++];
  Q_ASSERT(n >= 0 && n < static_cast<int>(GameLightmaps.size()));
  Q_ASSERT(GameLightmaps[n].used == 0);

  GameLightmaps[n] = {};
  GameLightmaps[n].data.assign(h, std::vector<uint16_t>(w, 0));

  GameLightmaps[n].width = w;
  GameLightmaps[n].height = h;
  GameLightmaps[n].used = 1;
  GameLightmaps[n].cache_slot = -1;
  GameLightmaps[n].flags = LF_CHANGED;
  // Find power-of-2 "square" resolution, as the original does.
  int res = std::max(w, h);
  int lightmap_res = 2;
  for (int i = 0; i <= 7; i++) {
    int low_num = 1 << i;
    int hi_num = 2 << i;
    if (res <= hi_num && res > low_num) {
      lightmap_res = hi_num;
      break;
    }
  }
  Q_ASSERT(lightmap_res >= 2 && lightmap_res <= 128);
  GameLightmaps[n].square_res = lightmap_res;

  return static_cast<uint16_t>(n);
}

// Given a handle, frees the lightmap memory and flags this lightmap as unused
void lm_FreeLightmap(int handle) {
  if (handle < 0 || handle >= static_cast<int>(GameLightmaps.size()))
    return;
  if (GameLightmaps[handle].used < 1)
    return;
  GameLightmaps[handle].used--;
  if (GameLightmaps[handle].used == 0) {
    GameLightmaps[handle].data.clear();
    GameLightmaps[handle].cache_slot = -1;

    Q_ASSERT(Num_of_lightmaps > 0);
    Free_lightmap_list[--Num_of_lightmaps] = static_cast<uint16_t>(handle);
  }
}

// returns a lightmaps width  else nullopt if something is wrong
std::optional<uint8_t> lm_w(int handle) {
  if (handle < 0 || handle >= static_cast<int>(GameLightmaps.size()))
    return std::nullopt;
  if (!GameLightmaps[handle].used)
    return std::nullopt;
  return GameLightmaps[handle].width;
}

// returns a lightmaps height , else nullopt if something is wrong
std::optional<uint8_t> lm_h(int handle) {
  if (handle < 0 || handle >= static_cast<int>(GameLightmaps.size()))
    return std::nullopt;
  if (!GameLightmaps[handle].used)
    return std::nullopt;
  return GameLightmaps[handle].height;
}

// returns a lightmaps data else NULL if something is wrong
std::vector<std::vector<uint16_t>> &lm_data(int handle) {
  Q_ASSERT(handle >= 0 && handle < static_cast<int>(GameLightmaps.size()));
  Q_ASSERT(GameLightmaps[handle].used);
  return GameLightmaps[handle].data;
}
