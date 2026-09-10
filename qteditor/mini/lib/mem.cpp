/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral memory allocation (ported from the original lib/mem.cpp).
 */

#include "mem.h"

#include <new>
#include <cstddef>
#include <cstring>
#include <cstdlib>

void *mem_malloc_sub(int size, const char *fn, int line) {
  void *p = malloc(size);
  if (p == nullptr)
    throw std::bad_alloc();
  return p;
}
void mem_free_sub(void *ptr) { free(ptr); }
void *mem_realloc_sub(void *ptr, int size) {
  void *p = realloc(ptr, size);
  if (p == nullptr)
    throw std::bad_alloc();
  return p;
}

char *mem_strdup_sub(const char *s, const char *fn, int line) {
  if (s == nullptr)
    return nullptr;
  const std::size_t n = std::strlen(s) + 1;
  char *d = static_cast<char *>(mem_malloc(n));
  std::memcpy(d, s, n);
  return d;
}
