/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral sound table management (ported from the original soundload.cpp).
 */

#include "soundload.h"

#include <QtGlobal>

// Allocs a sound for use, returns -1 if error, else index on success
int AllocSound() {
  const size_t n = Sounds.next_slot();
  Q_ASSERT(Sounds.is_unused(n));

  Sounds[n] = sound_info{};

  Sounds.acquire(n);
  return static_cast<int>(n);
}

// Frees sound index n
void FreeSound(int n) {
  Q_ASSERT(Sounds.is_used(n));

  Sounds[n] = sound_info{};
  Sounds.release(n);
}

// Gets next sound from n that has actually been alloced
int GetNextSound(int n) {
  if (Sounds.size() == 0)
    return -1;

  if ((n < 0) || (n >= static_cast<int>(Sounds.size())))
    n = -1;

  for (int i = n + 1; i < static_cast<int>(Sounds.size()); i++)
    if (Sounds.is_used(i))
      return i;
  for (int i = 0; i < n; i++)
    if (Sounds.is_used(i))
      return i;

  // this is the only one
  return n;
}

// Gets previous sound from n that has actually been alloced
int GetPrevSound(int n) {
  if (Sounds.size() == 0)
    return -1;

  if ((n < 0) || (n >= static_cast<int>(Sounds.size())))
    n = static_cast<int>(Sounds.size());

  for (int i = n - 1; i >= 0; i--)
    if (Sounds.is_used(i))
      return i;
  for (int i = static_cast<int>(Sounds.size()) - 1; i > n; i--)
    if (Sounds.is_used(i))
      return i;

  // this is the only one
  return n;
}