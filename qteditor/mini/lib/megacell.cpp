/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral megacell table management (ported from the original
 * Descent3/megacell.cpp). The table now uses d3::slotvec_t.
 */

#include "megacell.h"

#include <optional>
#include <string>

#include <QtGlobal>

#include "string_helpers.h"

// Sets all megacells to unused
void InitMegacells() {
  Megacells.clear();
}

// Allocs a megacell for use, returns std::nullopt if error, else index on success
std::optional<uint32_t> AllocMegacell() {
  const size_t i = Megacells.next_slot();
  Q_ASSERT(Megacells.is_unused(i));

  Megacells[i] = megacell{};
  Megacells.acquire(i);
  Megacells[i].width = DEFAULT_MEGACELL_WIDTH;
  Megacells[i].height = DEFAULT_MEGACELL_HEIGHT;
  return static_cast<uint32_t>(i);
}

// Frees megacell index n
void FreeMegacell(uint32_t n) {
  Q_ASSERT(Megacells.is_used(n));

  Megacells[n].name.clear();
  Megacells.release(n);
}

// Gets next megacell from n that has actually been alloced
std::optional<uint32_t> GetNextMegacell(uint32_t n) {
  return Megacells.next(n);
}

// Gets previous megacell from n that has actually been alloced
std::optional<uint32_t> GetPrevMegacell(uint32_t n) {
  return Megacells.prev(n);
}

// Searches thru all megacells for a specific name, returns std::nullopt if not
// found or the index of the megacell with that name
std::optional<uint32_t> FindMegacellName(const std::string &name) {
  for (uint32_t i = 0; i < static_cast<uint32_t>(Megacells.size()); i++)
    if (Megacells.is_used(i) && match(Megacells[i].name, name))
      return i;

  return std::nullopt;
}
