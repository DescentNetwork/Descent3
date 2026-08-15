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
#include "slew.h"
#include "manage.h"
#include "crossplat.h"
#include <filesystem>

d3edit_state D3EditState;
int World_changed = 0;

// Editor-only terrain globals guarded by EDITOR in terrain.cpp; the D3 core is
// compiled without that define, so provide them here.
uint8_t TerrainSelected[TERRAIN_WIDTH * TERRAIN_DEPTH];
int Num_terrain_selected = 0;

// Editor-only state flags declared in d3edit.h (defined in the MFC editor's
// editor.cpp/EDVARS.cpp).
int State_changed = 0;
int Viewer_moved = 0;
int Object_moved = 0;
int New_mine = 0;

// Slew movement limitations flag (defined in the MFC editor's editor.cpp).
int Slew_limitations = 0;

// SLEW.cpp guards SlewControlInit() with EDITOR; the Qt port has no controller
// integration, so provide a stub.
void SlewControlInit() {}

// Manage-system dirs/network globals (LocalModelsDir, NetModelsDir, TableUser,
// Network_up) come from manage.cpp; only the editor-only file-dialog dirs need
// defining here.
char Current_model_dir[_MAX_PATH] = "";
