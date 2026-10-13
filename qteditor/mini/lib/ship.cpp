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

// Allocs a ship for use, returns std::nullopt if error, else index on success
index_t AllocShip() {
  const size_t n = Ships.next_slot();
  Q_ASSERT(Ships.is_unused(n));

  Ships[n] = ship{};

  Ships.acquire(n);
  return static_cast<uint32_t>(n);
}

// Frees ship index n
void FreeShip(uint32_t n) {
  Q_ASSERT(Ships.is_used(n));

  Ships[n].name.clear();
  Ships.release(n);
}

// Gets next ship from n that has actually been alloced
index_t GetNextShip(uint32_t n) {
  return Ships.next(n);
}

// Gets previous ship from n that has actually been alloced
index_t GetPrevShip(uint32_t n) {
  return Ships.prev(n);
}

// Searches thru all ships for a specific name, returns std::nullopt if not
// found or index of ship with name
index_t FindShipName(const std::string &name) {
  for (uint32_t i = 0; i < Ships.size(); i++)
    if (Ships.is_used(i) && match(name, Ships[i].name))
      return i;

  return std::nullopt;
}