/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral lightmap info (ported from the original lightmap_info.cpp).  The
 * store table is static storage owned by the mini; slots are handed out in
 * sequential order via a free list so the level loader's NLMP round-trip can
 * treat file ordinals and store slots interchangeably.
 */

#include "lightmap_info.h"

#include "lightmap.h"

#include <QtGlobal>

#include <cstring>

// The lightmap info store table and its globals.
lightmap_info LightmapInfoStore[MAX_LIGHTMAP_INFOS];
lightmap_info *LightmapInfo = LightmapInfoStore;
int Num_of_lightmap_info = 0;
int Num_lightmap_infos_read = 0;

static uint16_t Free_lmi_list[MAX_LIGHTMAP_INFOS];

void CloseLightmapInfos() {
  // The mini keeps lightmap info storage statically; nothing is freed at exit.
}

// Sets all the lightmaps to unused
void InitLightmapInfo(int nummaps) {
  (void)nummaps;
  memset(LightmapInfoStore, 0, sizeof(LightmapInfoStore));
  for (uint32_t i = 0; i < MAX_LIGHTMAP_INFOS; i++)
    Free_lmi_list[i] = static_cast<uint16_t>(i);
  Num_of_lightmap_info = 0;
}

// Allocs a lightmap of w x h size, optionally allocating its backing texture.
// Returns lightmap info handle if successful, nullopt if otherwise
std::optional<uint32_t> AllocLightmapInfo(int w, int h, int type, bool alloc_lightmap) {
  int n;

  if (Num_of_lightmap_info >= static_cast<int>(MAX_LIGHTMAP_INFOS))
    return std::nullopt; // Ran out of lightmap infos!

  n = Free_lmi_list[Num_of_lightmap_info++];
  Q_ASSERT(n >= 0 && n < static_cast<int>(MAX_LIGHTMAP_INFOS));
  Q_ASSERT(LightmapInfo[n].used == 0);

  memset(&LightmapInfo[n], 0, sizeof(lightmap_info));

  Q_ASSERT(w >= 2 && h >= 2);

  if (alloc_lightmap) {
    const std::optional<uint32_t> lm = lm_AllocLightmap(w, h);
    Q_ASSERT(lm.has_value());
    LightmapInfo[n].lm_handle = static_cast<uint16_t>(lm.value_or(BAD_LM_INDEX));
  }

  LightmapInfo[n].used = 1;
  LightmapInfo[n].type = type;
  LightmapInfo[n].dynamic = BAD_LM_INDEX;
  LightmapInfo[n].spec_map = -1;
  LightmapInfo[n].width = w;
  LightmapInfo[n].height = h;
  LightmapInfo[n].x1 = 0;
  LightmapInfo[n].y1 = 0;

  return static_cast<uint32_t>(n);
}

// Given a handle, frees the lightmap info (and its lightmap) if it is the last
// reference.
void FreeLightmapInfo(int handle) {
  if (handle < 0 || handle >= static_cast<int>(MAX_LIGHTMAP_INFOS))
    return;

  if (LightmapInfo[handle].used < 1)
    return;

  LightmapInfo[handle].used--;

  if (LightmapInfo[handle].used == 0) {
    lm_FreeLightmap(LightmapInfo[handle].lm_handle);

    Free_lmi_list[--Num_of_lightmap_info] = static_cast<uint16_t>(handle);
  }
}

// Gets the width of this lightmap_info handle
std::optional<uint32_t> lmi_w(int handle) {
  if (!LightmapInfo[handle].used)
    return std::nullopt;
  return LightmapInfo[handle].width;
}

// Gets the height of this lightmap_info handle
std::optional<uint32_t> lmi_h(int handle) {
  if (!LightmapInfo[handle].used)
    return std::nullopt;
  return LightmapInfo[handle].height;
}
