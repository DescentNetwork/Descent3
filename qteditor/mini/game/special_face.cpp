/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral special-face table (ported from game/special_face.cpp).  The mini
 * keeps the same free-list + handle scheme; SpecialFaces grows on demand
 * (handles live in face::special_handle, an index_t where std::nullopt means
 * "no special face").  Table is per-level: cleared by
 * InitSpecialFaces() at level load.
 */

#include <QtGlobal>

#include <vector>

#include "special_face.h"

d3::slotvec_t<special_face> SpecialFaces;

// Sets all the special faces to unused
void InitSpecialFaces() {
  SpecialFaces.clear();
}

// Returns an index into the special faces array
index_t AllocSpecialFace(int type, int num, bool vertnorms, int num_vertnorms) {
  const size_t n = SpecialFaces.next_slot();
  Q_ASSERT(!SpecialFaces.is_used(n));

  special_face &sf = SpecialFaces[n] = special_face{};
  sf.spec_instance.assign(static_cast<size_t>(num), specular_instance{});
  sf.vertnorms.clear();

  sf.type = type;
  sf.num = num;
  sf.flags = {};
  SpecialFaces.acquire(n);

  if (vertnorms) {
    sf.vertnorms.assign(static_cast<size_t>(num_vertnorms), vector3{});
    sf.flags.spec_smooth = true;
  }

  return static_cast<uint32_t>(n);
}

// Given a handle, frees the special face
void FreeSpecialFace(index_t handle) {
  if (!handle || *handle >= SpecialFaces.size())
    return;

  if (SpecialFaces.is_unused(*handle))
    return;

  SpecialFaces.release(*handle);

  if (SpecialFaces.is_unused(*handle)) {
    SpecialFaces[*handle].spec_instance.clear();
    SpecialFaces[*handle].vertnorms.clear();
    SpecialFaces[*handle].num = 0;
  }
}