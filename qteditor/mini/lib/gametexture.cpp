/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral game texture lookup (ported from the original gametexture.cpp).
 */

#include "gametexture.h"

#include <string>
#include <stdexcept>

int FindTextureName(const std::string &name) {
  for (int i = 0; i < Num_textures; i++) {
    if (!GameTextures[i].name.empty() && name == GameTextures[i].name)
      return i;
  }
  std::runtime_error("texture not found!");
  return -1;
}
