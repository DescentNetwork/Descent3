/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "level_io.h"

#include <cstdio>

namespace QtEditor {

namespace {

// The full editor targets (Windows-only) construct a single boot mine via
// editor/HFile.cpp's CreateNewMine(). The Linux port has no game-state mine
// data structures linked yet, so this stub is a no-op that satisfies the
// File→New menu wiring. The real implementation will land once the engine
// integration pieces mature.
void log_stub(const char *what, const char *arg = nullptr) {
  if (arg == nullptr)
    std::fprintf(stderr, "[level_io] stub: %s\n", what);
  else
    std::fprintf(stderr, "[level_io] stub: %s(%s)\n", what, arg);
}

} // namespace

void CreateNewMine() { log_stub("CreateNewMine"); }

bool EditorLoadLevel(const char *filename) {
  log_stub("EditorLoadLevel", filename);
  return true;
}

int EditorSaveLevel(const char *filename) {
  log_stub("EditorSaveLevel", filename);
  return 1;
}

} // namespace QtEditor
