/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral device dependent IO (ported from the original ddio_* libs).
 */

#include "ddio.h"

#include <cstdio>
#include <filesystem>

int ddio_GetFileLength(FILE *filePtr) {
  if (filePtr == nullptr)
    return 0;
  long cur = ftell(filePtr);
  fseek(filePtr, 0, SEEK_END);
  long len = ftell(filePtr);
  fseek(filePtr, cur, SEEK_SET);
  return (int)len;
}

bool ddio_FileDiff(const std::filesystem::path& a, const std::filesystem::path& b) {
  if (a == b)
    return false;
  if (!std::filesystem::exists(a) || !std::filesystem::exists(b))
    return true;
  return std::filesystem::file_size(a) != std::filesystem::file_size(b);
}
