/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral bitmap allocation (ported from the original bitmap.cpp).
 */

#include "bitmap.h"
#include "mem.h"

#include <cstdint>
#include <cstring>

int bm_AllocBitmap(int w, int h, int add_mem) {
  int n = -1;
  for (int i = 0; i < MAX_BITMAPS; i++) {
    if (GameBitmaps[i].used == 0) {
      n = i;
      break;
    }
  }
  if (n == -1)
    return -1;

  GameBitmaps[n].width = (uint16_t)w;
  GameBitmaps[n].height = (uint16_t)h;
  GameBitmaps[n].format = BITMAP_FORMAT_STANDARD;
  GameBitmaps[n].flags = BF_CHANGED | BF_BRAND_NEW;
  GameBitmaps[n].cache_slot = -1;
  GameBitmaps[n].data16.reset();
  GameBitmaps[n].mip_levels = 0;
  GameBitmaps[n].name[0] = 0;

  if (w > 0 && h > 0) {
    size_t base = (size_t)w * (size_t)h * 2;
    size_t extra = (add_mem > 0) ? (size_t)add_mem : 0;
    size_t bytes = base + extra;
    GameBitmaps[n].data16 = std::make_unique<uint16_t[]>((bytes + 1) / 2);
    std::fill_n(GameBitmaps[n].data16.get(), (bytes + 1) / 2, 0);
    if (add_mem > 0)
      GameBitmaps[n].mip_levels = 1; // marked mipped; actual count set by caller
  }

  GameBitmaps[n].used = 1;
  return n;
}

// Given a handle, frees the bitmap memory and flags this bitmap as unused
void bm_FreeBitmap(int handle) {
  if (handle != BAD_BITMAP_HANDLE && GameBitmaps[handle].used >= 1)
    GameBitmaps[handle].used--;
}


uint16_t *bm_data(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return nullptr;
  const bms_bitmap &b = GameBitmaps[handle];
  if (!b.data16)
    return nullptr;
  int offset = 0, w = b.width, h = b.height;
  for (int m = 0; m < miplevel; m++) {
    offset += w * h;
    if (w > 1) w >>= 1;
    if (h > 1) h >>= 1;
  }
  return b.data16.get() + offset;
}
int bm_w(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  int w = GameBitmaps[handle].width;
  for (int m = 0; m < miplevel; m++) {
    if (w > 1) w >>= 1;
  }
  return w;
}
int bm_h(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  int h = GameBitmaps[handle].height;
  for (int m = 0; m < miplevel; m++) {
    if (h > 1) h >>= 1;
  }
  return h;
}
int bm_mipped(int handle) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  return GameBitmaps[handle].mip_levels > 1 ? 1 : 0;
}
