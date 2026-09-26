/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral ship table management (ported from the original ship.cpp).
 */

#include "ship.h"
#include "string_helpers.h"
#include "objinfo.h"

#include <QtGlobal>

// Allocs a ship for use, returns -1 if error, else index on success
int AllocShip() {
  const size_t n = Ships.next_slot();
  Q_ASSERT(Ships.is_unused(n));

  Ships[n] = ship{};

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