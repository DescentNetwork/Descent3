/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral ship table management (ported from the original ship.cpp).
 */

#include "ship.h"
#include "string_helpers.h"
#include "robotfire.h"
#include "objinfo.h"

#include <QtGlobal>

namespace {
constexpr float DEFAULT_SHIP_SIZE = 4.0f;
}

// Allocs a ship for use, returns -1 if error, else index on success
int AllocShip() {
  const size_t n = Ships.next_slot();
  Q_ASSERT(Ships.is_unused(n));

  Ships[n] = ship{};
  Ships[n].size = DEFAULT_SHIP_SIZE;
  Ships[n].dying_model_handle = -1;
  Ships[n].med_render_handle = -1;
  Ships[n].lo_render_handle = -1;
  Ships[n].med_lod_distance = DEFAULT_MED_LOD_DISTANCE;
  Ships[n].lo_lod_distance = DEFAULT_LO_LOD_DISTANCE;
  Ships[n].model_handle = -1;
  Ships[n].armor_scalar = 1.0f;
  Ships[n].flags = 0;

  Ships[n].phys_info.hit_die_dot = -1; // -1 means doesn't apply

  // Make sure the weapon battery info is cleared for a new object
  WBClearInfo(Ships[n].static_wb.data());

  for (int w = 0; w < MAX_PLAYER_WEAPONS; w++) {
    Ships[n].firing_sound[w] = -1;
    Ships[n].firing_release_sound[w] = -1;
  }

  Ships.acquire(n);
  return static_cast<int>(n);
}

// Frees ship index n
void FreeShip(int n) {
  Q_ASSERT(Ships.is_used(n));

  Ships[n].name.clear();
  Ships.release(n);
}

// Gets next ship from n that has actually been alloced
int GetNextShip(int n) {
  if (Ships.size() == 0)
    return -1;

  if ((n < 0) || (n >= static_cast<int>(Ships.size())))
    n = -1;

  for (int i = n + 1; i < static_cast<int>(Ships.size()); i++)
    if (Ships.is_used(i))
      return i;
  for (int i = 0; i < n; i++)
    if (Ships.is_used(i))
      return i;

  // this is the only one
  return n;
}

// Gets previous ship from n that has actually been alloced
int GetPrevShip(int n) {
  if (Ships.size() == 0)
    return -1;

  if ((n < 0) || (n >= static_cast<int>(Ships.size())))
    n = static_cast<int>(Ships.size());

  for (int i = n - 1; i >= 0; i--)
    if (Ships.is_used(i))
      return i;
  for (int i = static_cast<int>(Ships.size()) - 1; i > n; i--)
    if (Ships.is_used(i))
      return i;

  // this is the only one
  return n;
}

// Searches thru all ships for a specific name, returns -1 if not found
// or index of ship with name
int FindShipName(const std::string &name) {
  for (int i = 0; i < static_cast<int>(Ships.size()); i++)
    if (Ships.is_used(i) && match(name, Ships[i].name))
      return i;

  return -1;
}