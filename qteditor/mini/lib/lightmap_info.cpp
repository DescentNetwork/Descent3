/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral lightmap info (ported from the original lightmap_info.cpp).  The
 * store table is auto-sized storage owned by the mini; slots are handed out in
 * sequential order via a free list so the level loader's NLMP round-trip can
 * treat file ordinals and store slots interchangeably.
 */

#include "lightmap_info.h"

#include "lightmap.h"

#include <QtGlobal>

// The lightmap info store table and its globals.
d3::slotvec_t<lightmap_info> LightmapInfo;
int Num_lightmap_infos_read = 0;

void CloseLightmapInfos() {
  // The mini keeps lightmap info storage in auto-sized vectors; nothing is
  // freed at exit.
}

// Sets all the lightmaps to unused
void InitLightmapInfo(int nummaps) {
  (void)nummaps;
  LightmapInfo.clear();
}

// Allocs a lightmap of w x h size, optionally allocating its backing texture.
// Returns 16-bit lightmap info handle if successful, nullopt if otherwise
std::optional<uint16_t> AllocLightmapInfo(int w, int h, int type, bool alloc_lightmap) {
  if (LightmapInfo.num_empty() == 0 && LightmapInfo.size() >= MAX_LIGHTMAP_INFOS)
    return std::nullopt; // Ran out of lightmap infos!

  const size_t n = LightmapInfo.next_slot();
  Q_ASSERT(n < MAX_LIGHTMAP_INFOS);
  Q_ASSERT(LightmapInfo.is_unused(n));

  LightmapInfo[n] = lightmap_info{};

  Q_ASSERT(w >= 2 && h >= 2);

  if (alloc_lightmap) {
    const std::optional<uint16_t> lm = lm_AllocLightmap(w, h);
    Q_ASSERT(lm);
    LightmapInfo[n].lm_handle = lm.value_or(BAD_LM_INDEX);
  }

  LightmapInfo.acquire(n);
  LightmapInfo[n].type = type;
  LightmapInfo[n].dynamic = BAD_LM_INDEX;
  LightmapInfo[n].spec_map = -1;
  LightmapInfo[n].width = w;
  LightmapInfo[n].height = h;
  LightmapInfo[n].x1 = 0;
  LightmapInfo[n].y1 = 0;

  return static_cast<uint16_t>(n);
}

// Given a handle, frees the lightmap info (and its lightmap) if it is the last
// reference.
void FreeLightmapInfo(int handle) {
  if (handle < 0 || handle >= static_cast<int>(LightmapInfo.size()))
    return;

  if (LightmapInfo.is_unused(handle))
    return;

  LightmapInfo.release(handle);

  if (LightmapInfo.is_unused(handle))
    lm_FreeLightmap(LightmapInfo[handle].lm_handle);
}

// Gets the width of this lightmap_info handle (stored as uint8)
std::optional<uint8_t> lmi_w(int handle) {
  if (handle < 0 || handle >= static_cast<int>(LightmapInfo.size()))
    return std::nullopt;
  if (LightmapInfo.is_unused(handle))
    return std::nullopt;
  return LightmapInfo[handle].width;
}

// Gets the height of this lightmap_info handle (stored as uint8)
std::optional<uint8_t> lmi_h(int handle) {
  if (handle < 0 || handle >= static_cast<int>(LightmapInfo.size()))
    return std::nullopt;
  if (LightmapInfo.is_unused(handle))
    return std::nullopt;
  return LightmapInfo[handle].height;
}