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
std::optional<uint32_t> AllocSound() {
  const size_t n = Sounds.next_slot();
  Q_ASSERT(Sounds.is_unused(n));

  Sounds[n] = sound_info{};

  Sounds.acquire(n);
  return static_cast<uint32_t>(n);
}

// Frees sound index n
void FreeSound(uint32_t n) {
  Q_ASSERT(Sounds.is_used(n));

  Sounds[n] = sound_info{};
  Sounds.release(n);
}

// Gets next sound from n that has actually been alloced
std::optional<uint32_t> GetNextSound(uint32_t n) {
  return Sounds.next(n);
}

// Gets previous sound from n that has actually been alloced
std::optional<uint32_t> GetPrevSound(uint32_t n) {
  return Sounds.prev(n);
}