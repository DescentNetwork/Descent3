/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral bitmap allocation (ported from the original bitmap.cpp).
 */

#include "bitmap.h"
#include "mem.h"

#include <QtGlobal>
#include <cstdint>
#include <cstring>

std::optional<uint32_t> bm_AllocBitmap(int w, int h, int add_mem) {
  const size_t n = GameBitmaps.next_slot();
  Q_ASSERT(GameBitmaps.is_unused(n));

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

  GameBitmaps.acquire(n);
  return static_cast<uint32_t>(n);
}

// Given a handle, frees the bitmap memory and flags this bitmap as unused
void bm_FreeBitmap(int handle) {
  if (handle != BAD_BITMAP_HANDLE && GameBitmaps.is_used(handle))
    GameBitmaps.release(handle);
}


uint16_t *bm_data(int handle, int miplevel) {
  if (handle < 0 || handle >= static_cast<int>(GameBitmaps.size()))
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
  if (handle < 0 || handle >= static_cast<int>(GameBitmaps.size()))
    return 0;
  int w = GameBitmaps[handle].width;
  for (int m = 0; m < miplevel; m++) {
    if (w > 1) w >>= 1;
  }
  return w;
}
int bm_h(int handle, int miplevel) {
  if (handle < 0 || handle >= static_cast<int>(GameBitmaps.size()))
    return 0;
  int h = GameBitmaps[handle].height;
  for (int m = 0; m < miplevel; m++) {
    if (h > 1) h >>= 1;
  }
  return h;
}
int bm_mipped(int handle) {
  if (handle < 0 || handle >= static_cast<int>(GameBitmaps.size()))
    return 0;
  return GameBitmaps[handle].mip_levels > 1 ? 1 : 0;
}

// Returns the number of mipmap levels for a bitmap.
int bm_miplevels(int handle) {
  if (handle < 0 || handle >= static_cast<int>(GameBitmaps.size()))
    return 0;
  const bms_bitmap &b = GameBitmaps[handle];
  if (b.mip_levels)
    return b.mip_levels;
  if (b.flags & BF_MIPMAPPED) {
    int levels = 0;
    for (int tmp = b.width; tmp > 0; tmp >>= 1)
      levels++;
    return levels;
  }
  return 0;
}

// Scales the data from src into the size of dest (nearest-neighbor).
void bm_ScaleBitmapToBitmap(int dest, int src) {
  if (dest < 0 || dest >= static_cast<int>(GameBitmaps.size()) || src < 0 ||
      src >= static_cast<int>(GameBitmaps.size()))
    return;
  uint16_t *dp = bm_data(dest, 0);
  uint16_t *sp = bm_data(src, 0);
  Q_ASSERT(dp);
  Q_ASSERT(sp);

  const int smipped = bm_mipped(src);
  const int dmipped = bm_mipped(dest);
  Q_ASSERT(smipped == dmipped);
  Q_ASSERT(GameBitmaps[dest].format == GameBitmaps[src].format);

  int sw = bm_w(src, 0);
  int sh = bm_h(src, 0);
  int dw = bm_w(dest, 0);
  int dh = bm_h(dest, 0);

  if (sw == dw && sh == dh) {
    int limit = smipped ? bm_miplevels(src) : 1;
    for (int i = 0; i < limit; i++) {
      uint16_t *sdata = bm_data(src, i);
      uint16_t *ddata = bm_data(dest, i);
      dw = bm_w(dest, i);
      dh = bm_h(dest, i);
      memcpy(ddata, sdata, dw * dh * sizeof(uint16_t));
    }
    GameBitmaps[dest].flags |= BF_CHANGED;
    return;
  }

  int limit = smipped ? bm_miplevels(src) : 1;
  for (int m = 0; m < limit; m++) {
    sw = bm_w(src, m);
    sh = bm_h(src, m);
    dw = bm_w(dest, m);
    dh = bm_h(dest, m);

    uint16_t *sdata = bm_data(src, m);
    uint16_t *ddata = bm_data(dest, m);

    // These are our interpolant variables
    float xstep = static_cast<float>(sw) / static_cast<float>(dw);
    float ystep = static_cast<float>(sh) / static_cast<float>(dh);
    float xoff = 0;
    float yoff = 0;
    for (int i = 0; i < dh; i++, yoff += ystep) {
      int t;
      for (xoff = 0, t = 0; t < dw; t++, xoff += xstep)
        ddata[i * dw + t] = sdata[static_cast<int>(yoff) * sw + static_cast<int>(xoff)];
    }
  }
  GameBitmaps[dest].flags |= BF_CHANGED;
}
