/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

#ifndef GAMEDATA_LOADER_H
#define GAMEDATA_LOADER_H

#include <filesystem>

#include "manage.h"
#include "megacell.h"
#include <posix_stream.h>


// Reads a megacell page from an open file.  Returns 0 on error.  (Declared in
// megapage.h.)

// Parses Table.gam from the given d3.hog into the global metadata arrays
// (Object_info / Ships / Weapons / Sounds / GameTextures / Doors / Megacells)
// plus their Num_* counters.  Returns true on success, false on failure.
bool loadGameDataTable(const std::filesystem::path& d3HogPath);

#endif
