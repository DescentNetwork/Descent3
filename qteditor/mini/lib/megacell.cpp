/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral megacell table iteration (ported from the original megacell.cpp).
 *
 * The megacell table is a fixed array (not a slotvec); it also tracks a
 * running count in Num_megacells on its own.
 */

#include "megacell.h"

#include <QtGlobal>

// Gets next megacell from n that has actually been alloced
int GetNextMegacell(int n) {
  Q_ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  for (int i = n + 1; i < MAX_MEGACELLS; i++)
    if (Megacells[i].used)
      return i;
  for (int i = 0; i < n; i++)
    if (Megacells[i].used)
      return i;

  // this is the only one
  return n;
}

// Gets previous megacell from n that has actually been alloced
int GetPrevMegacell(int n) {
  Q_ASSERT(n >= 0 && n < MAX_MEGACELLS);

  if (Num_megacells == 0)
    return 0;

  for (int i = n - 1; i >= 0; i--)
    if (Megacells[i].used)
      return i;
  for (int i = MAX_MEGACELLS - 1; i > n; i--)
    if (Megacells[i].used)
      return i;

  // this is the only one
  return n;
}