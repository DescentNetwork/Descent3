/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral special-face table (ported from game/special_face.cpp).  The mini
 * keeps the same free-list + handle scheme; SpecialFaces grows on demand
 * (handles live in face::special_handle, an int16, with
 * BAD_SPECIAL_FACE_INDEX == -1).  Table is per-level: cleared by
 * InitSpecialFaces() at level load.
 */

#include <QtGlobal>

#include <vector>

#include "special_face.h"

std::vector<special_face> SpecialFaces;
int Num_of_special_faces = 0;
static std::vector<uint16_t> Free_special_face_list;

// Sets all the special faces to unused
void InitSpecialFaces() {
  SpecialFaces.clear();
  Free_special_face_list.clear();
  Num_of_special_faces = 0;
}

// Returns an index into the special faces array
int AllocSpecialFace(int type, int num, bool vertnorms, int num_vertnorms) {
  // The free list hands out fresh handles as identity values (its slot index).
  // When the cursor reaches the current frontier the table must first grow by
  // one slot whose free-list value equals its own index; SpecialFaces grows
  // without an arbitrary size cap.
  if (Num_of_special_faces == static_cast<int>(SpecialFaces.size())) {
    SpecialFaces.push_back(special_face{});
    Free_special_face_list.push_back(static_cast<uint16_t>(SpecialFaces.size() - 1));
  }

  int n = Free_special_face_list[Num_of_special_faces++];
  Q_ASSERT(n >= 0 && n < static_cast<int>(SpecialFaces.size()));
  Q_ASSERT(SpecialFaces[n].used == 0);

  special_face &sf = SpecialFaces[n] = special_face{};
  sf.spec_instance.assign(static_cast<size_t>(num), specular_instance{});
  sf.vertnorms.clear();

  sf.type = type;
  sf.num = num;
  sf.flags = 0;
  sf.used = 1;

  if (vertnorms) {
    sf.vertnorms.assign(static_cast<size_t>(num_vertnorms), vector3{});
    sf.flags |= SFF_SPEC_SMOOTH;
  }

  return n;
}

// Given a handle, frees the special face
void FreeSpecialFace(int handle) {
  if (handle < 0 || handle >= static_cast<int>(SpecialFaces.size()))
    return;

  if (SpecialFaces[handle].used < 1)
    return;

  SpecialFaces[handle].used--;

  if (SpecialFaces[handle].used == 0) {
    Free_special_face_list[--Num_of_special_faces] = static_cast<uint16_t>(handle);
    SpecialFaces[handle].spec_instance.clear();
    SpecialFaces[handle].vertnorms.clear();
    SpecialFaces[handle].num = 0;
  }
}