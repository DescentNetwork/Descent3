/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral lightmap info (ported from the original lightmap_info.cpp).
 */

#include "lightmap_info.h"

#include <cstring>

// The lightmap info store table and its globals live in stubs.cpp.
extern lightmap_info LightmapInfoStore[MAX_LIGHTMAP_INFOS];

void InitLightmapInfo(int nummaps) {
  if (nummaps > 0 && nummaps < MAX_LIGHTMAP_INFOS) {
    memset(LightmapInfoStore, 0, sizeof(lightmap_info) * nummaps);
  } else {
    memset(LightmapInfoStore, 0, sizeof(LightmapInfoStore));
  }
  Num_of_lightmap_info = 0;
}
