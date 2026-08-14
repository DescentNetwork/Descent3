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

// Editor-wide state that the original MFC editor keeps in EDVARS.cpp/editor.cpp.
// The D3 core is compiled without the EDITOR define, so this is re-provided
// here for the Qt port.
#include "d3edit.h"
#include "terrain.h"

d3edit_state D3EditState;
int World_changed = 0;

// Editor-only terrain globals guarded by EDITOR in terrain.cpp; the D3 core is
// compiled without that define, so provide them here.
uint8_t TerrainSelected[TERRAIN_WIDTH * TERRAIN_DEPTH];
int Num_terrain_selected = 0;
