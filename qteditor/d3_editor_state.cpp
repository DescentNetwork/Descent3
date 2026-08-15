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
#include "objinfo.h"
#include "gamepath.h"
#include "qt_debug.h"
#include <cstring>
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

// Editor room/face/portal editing context (defined in the MFC editor).
room *Curroomp = nullptr;
int Curface = -1;
int Curportal = -1;
int Current_trigger = -1;

// SLEW.cpp guards SlewControlInit() with EDITOR; the Qt port has no controller
// integration, so provide a stub.
void SlewControlInit() {}

// Manage-system dirs/network globals (LocalModelsDir, NetModelsDir, TableUser,
// Network_up) come from manage.cpp; only the editor-only file-dialog dirs need
// defining here.
char Current_model_dir[_MAX_PATH] = "";
char Current_sounds_dir[_MAX_PATH] = "";
char Current_bitmap_dir[_MAX_PATH] = "";

// Editor-only object helpers guarded by EDITOR in object.cpp/objinfo.cpp; the
// D3 core is compiled without that define, so provide them here.
char *Object_type_names[MAX_OBJECT_TYPES] = {
    "WALL",        // OBJ_WALL         0
    "FIREBALL",    // OBJ_FIREBALL     1
    "ROBOT",       // OBJ_ROBOT        2
    "SHARD",       // OBJ_SHARD        3
    "PLAYER",      // OBJ_PLAYER       4
    "WEAPON",      // OBJ_WEAPON       5
    "VIEWER",      // OBJ_VIEWER       6
    "POWERUP",     // OBJ_POWERUP      7
    "DEBRIS",      // OBJ_DEBRIS       8
    "CAMERA",      // OBJ_CAMERA       9
    "SHOCKWV",     // OBJ_SHOCKWAVE    10
    "CLUTTER",     // OBJ_CLUTTER      11
    "GHOST",       // OBJ_GHOST        12
    "LIGHT",       // OBJ_LIGHT        13
    "COOP",        // OBJ_COOP         14
    "UNUSED",      // OBJ_MARKER       15
    "BUILDING",    // OBJ_BUILDING     16
    "DOOR",        // OBJ_DOOR         17
    "ROOM",        // OBJ_ROOM         18
    "LINE",        // OBJ_PARTICLE     19
    "SPLINTER",    // OBJ_SPLINTER     20
    "DUMMY",       // OBJ_DUMMY        21
    "OBSERVER",    // OBJ_OBSERVER     22
    "DEBUG LINE",  // OBJ_DEBUG_LINE   23
    "SOUNDSOURCE", // OBJ_SOUNDSOURCE  24
    "WAYPOINT",    // OBJ_WAYPOINT     25
};

int GetNextObjectID(int n) {
  int type = Object_info[n].type;
  ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
  if (Num_object_ids[type] == 0)
    return -1;
  for (int i = n + 1; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  for (int i = 0; i <= n; i++)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  return n;
}

int GetPrevObjectID(int n) {
  int type = Object_info[n].type;
  ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
  if (Num_object_ids[type] == 0)
    return -1;
  for (int i = n - 1; i >= 0; i--)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  for (int i = MAX_OBJECT_IDS - 1; i >= n; i--)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  return n;
}

// Path editing helpers (editor/EPath.cpp in the MFC editor). The full version
// does line-of-sight checks against the viewer; simplified here.
int InsertNodeIntoPath(int pathnum, int nodenum, int flags, int roomnum, vector pos, matrix orient) {
  if (GamePaths[pathnum].num_nodes >= MAX_NODES_PER_PATH) {
    OutrageMessageBox("Error: Path already has its maximum amount of nodes.");
    return -1;
  }
  for (int i = GamePaths[pathnum].num_nodes - 1; i > nodenum; i--)
    memcpy(&GamePaths[pathnum].pathnodes[i + 1], &GamePaths[pathnum].pathnodes[i], sizeof(node));
  const int newnode = nodenum + 1;
  GamePaths[pathnum].pathnodes[newnode].pos = pos;
  GamePaths[pathnum].pathnodes[newnode].roomnum = roomnum;
  GamePaths[pathnum].pathnodes[newnode].flags = flags;
  GamePaths[pathnum].pathnodes[newnode].fvec = orient.fvec;
  GamePaths[pathnum].pathnodes[newnode].uvec = orient.uvec;
  GamePaths[pathnum].num_nodes++;
  return newnode;
}

void DeleteNodeFromPath(int pathnum, int nodenum) {
  for (int i = nodenum; i < GamePaths[pathnum].num_nodes - 1; i++)
    memcpy(&GamePaths[pathnum].pathnodes[i], &GamePaths[pathnum].pathnodes[i + 1], sizeof(node));
  GamePaths[pathnum].num_nodes--;
}

int GetNextPath(int n) {
  ASSERT(n >= 0 && n < MAX_GAME_PATHS);
  if (Num_game_paths == 0)
    return -1;
  for (int i = n + 1; i < MAX_GAME_PATHS; i++)
    if (GamePaths[i].used)
      return i;
  for (int i = 0; i < n; i++)
    if (GamePaths[i].used)
      return i;
  return n;
}

int GetPrevPath(int n) {
  ASSERT(n >= 0 && n < MAX_GAME_PATHS);
  if (Num_game_paths == 0)
    return -1;
  for (int i = n - 1; i >= 0; i--)
    if (GamePaths[i].used)
      return i;
  for (int i = MAX_GAME_PATHS - 1; i > n; i--)
    if (GamePaths[i].used)
      return i;
  return n;
}

int GetFirstPath() {
  for (int i = 0; i < MAX_GAME_PATHS; i++)
    if (GamePaths[i].used)
      return i;
  return -1;
}
