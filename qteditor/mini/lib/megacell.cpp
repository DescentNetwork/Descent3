/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral megacell table management (ported from the original
 * Descent3/megacell.cpp).
 *
 * The megacell table is a fixed array (not a slotvec); it also tracks a
 * running count in Num_megacells on its own.  Slots are recycled in ascending
 * index order, and a freed slot leaves a hole behind, so Num_megacells is a
 * count of live entries rather than a high-water mark.
 *
 * Uninitialized entries report used == 0 but carry no name, which is why
 * GetNextMegacell/GetPrevMegacell treat "no live entry" as index 0.
 */

#include "megacell.h"

#include <optional>
#include <string>

#include <QtGlobal>

#include "string_helpers.h"

uint32_t Num_megacells = 0;
megacell Megacells[MAX_MEGACELLS] = {};

// Sets all megacells to unused
void InitMegacells() {
  for (megacell &cell : Megacells)
    cell = megacell{};

  Num_megacells = 0;
}

// Allocs a megacell for use, returns std::nullopt if error, else index on success
std::optional<uint32_t> AllocMegacell() {
  for (uint32_t i = 0; i < MAX_MEGACELLS; i++) {
    if (Megacells[i].used == 0) {
      // megacell holds a std::string, so it is reset by assignment rather than
      // memset.
      Megacells[i] = megacell{};
      Megacells[i].used = 1;
      Megacells[i].width = static_cast<int8_t>(DEFAULT_MEGACELL_WIDTH);
      Megacells[i].height = static_cast<int8_t>(DEFAULT_MEGACELL_HEIGHT);
      Num_megacells++;
      return i;
    }
  }

  Q_ASSERT(false); // No megacells free!
  return std::nullopt;
}

// Frees megacell index n
void FreeMegacell(uint32_t n) {
  Q_ASSERT(n < MAX_MEGACELLS);
  Q_ASSERT(Megacells[n].used > 0);

  Megacells[n].name.clear();
  Megacells[n].used = 0;
  Num_megacells--;
  Q_ASSERT(Num_megacells <= MAX_MEGACELLS);
}

// Gets next megacell from n that has actually been alloced
int32_t GetNextMegacell(int32_t n) {
  Q_ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  // n may be the -1 "nothing selected" sentinel, so this index is signed: the
  // forward scan starts at n + 1, which is 0 in that case.
  for (int32_t i = n + 1; i < MAX_MEGACELLS; i++)
    if (Megacells[i].used)
      return i;
  for (int32_t i = 0; i < n; i++)
    if (Megacells[i].used)
      return i;

  // this is the only one
  return n;
}

// Gets previous megacell from n that has actually been alloced
int32_t GetPrevMegacell(int32_t n) {
  Q_ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  // The backward scan counts down to -1 before giving up, so the index must be
  // signed here.
  for (int32_t i = n - 1; i >= 0; i--) {
    if (Megacells[i].used)
      return i;
  }
  for (int32_t i = MAX_MEGACELLS - 1; i > n; i--) {
    if (Megacells[i].used)
      return i;
  }

  // this is the only one
  return n;
}

// Searches thru all megacells for a specific name, returns std::nullopt if not
// found or the index of the megacell with that name
std::optional<uint32_t> FindMegacellName(const std::string &name) {
  for (uint32_t i = 0; i < MAX_MEGACELLS; i++)
    if (Megacells[i].used && match(Megacells[i].name, name))
      return i;

  return std::nullopt;
}
