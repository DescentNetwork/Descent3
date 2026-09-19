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
std::vector<lightmap_info> LightmapInfo;
int Num_of_lightmap_info = 0;
int Num_lightmap_infos_read = 0;

static std::vector<uint16_t> Free_lmi_list;

void CloseLightmapInfos() {
  // The mini keeps lightmap info storage in auto-sized vectors; nothing is
  // freed at exit.
}

// Sets all the lightmaps to unused
void InitLightmapInfo(int nummaps) {
  (void)nummaps;
  LightmapInfo.clear();
  Free_lmi_list.clear();
  Num_of_lightmap_info = 0;
}

// Allocs a lightmap of w x h size, optionally allocating its backing texture.
// Returns 16-bit lightmap info handle if successful, nullopt if otherwise
std::optional<uint16_t> AllocLightmapInfo(int w, int h, int type, bool alloc_lightmap) {
  // The free list hands out fresh handles as identity values (its slot index).
  // When the cursor reaches the current frontier the table must first grow by
  // one slot whose free-list value equals its own index; MAX_LIGHTMAP_INFOS
  // stays as the hard cap keeping handles inside uint16_t range.
  if (Num_of_lightmap_info == static_cast<int>(LightmapInfo.size())) {
    if (LightmapInfo.size() >= MAX_LIGHTMAP_INFOS)
      return std::nullopt; // Ran out of lightmap infos!
    LightmapInfo.push_back(lightmap_info{});
    Free_lmi_list.push_back(static_cast<uint16_t>(Num_of_lightmap_info));
  }

  int n = Free_lmi_list[Num_of_lightmap_info++];
  Q_ASSERT(n >= 0 && n < static_cast<int>(LightmapInfo.size()));
  Q_ASSERT(LightmapInfo[n].used == 0);

  LightmapInfo[n] = lightmap_info{};

  Q_ASSERT(w >= 2 && h >= 2);

  if (alloc_lightmap) {
    const std::optional<uint16_t> lm = lm_AllocLightmap(w, h);
    Q_ASSERT(lm);
    LightmapInfo[n].lm_handle = lm.value_or(BAD_LM_INDEX);
  }

  LightmapInfo[n].used = 1;
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

  if (LightmapInfo[handle].used < 1)
    return;

  LightmapInfo[handle].used--;

  if (LightmapInfo[handle].used == 0) {
    lm_FreeLightmap(LightmapInfo[handle].lm_handle);

    Q_ASSERT(Num_of_lightmap_info > 0);
    Free_lmi_list[--Num_of_lightmap_info] = static_cast<uint16_t>(handle);
  }
}

// Gets the width of this lightmap_info handle (stored as uint8)
std::optional<uint8_t> lmi_w(int handle) {
  if (handle < 0 || handle >= static_cast<int>(LightmapInfo.size()))
    return std::nullopt;
  if (!LightmapInfo[handle].used)
    return std::nullopt;
  return LightmapInfo[handle].width;
}

// Gets the height of this lightmap_info handle (stored as uint8)
std::optional<uint8_t> lmi_h(int handle) {
  if (handle < 0 || handle >= static_cast<int>(LightmapInfo.size()))
    return std::nullopt;
  if (!LightmapInfo[handle].used)
    return std::nullopt;
  return LightmapInfo[handle].height;
}