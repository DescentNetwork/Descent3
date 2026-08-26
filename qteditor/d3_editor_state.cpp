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
#include "editor_room_state.h"
#include "mem/mem.h"
#include "terrain.h"
#include "slew.h"
#include "manage.h"

#include "objinfo.h"
#include "gamepath.h"
#include "findintersection.h"
#include "room.h"
#include "vecmat.h"
#include <cstdarg>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <QtGlobal>
#include <QMessageBox>
#include <QApplication>

#include "d3edit.h"

d3edit_state D3EditState;
bool World_changed = false;

// Editor-only terrain globals guarded by EDITOR in terrain.cpp; the D3 core is
// compiled without that define, so provide them here.
uint8_t TerrainSelected[TERRAIN_WIDTH * TERRAIN_DEPTH];
int Num_terrain_selected = 0;
int Editor_LOD_engine_off = 1;
bool Terrain_render_ext_room_objs = true;

// Editor-only state flags declared in d3edit.h (defined in the MFC editor's
// editor.cpp/EDVARS.cpp).
bool TV_changed = false;
bool State_changed = false;
bool Viewer_moved = false;
bool Object_moved = false;
bool New_mine = false;

// Room selection list (defined in the MFC editor's EDVARS.cpp).
int N_selected_rooms = 0;
int Selected_rooms[MAX_ROOMS];

// Slew movement limitations flag (defined in the MFC editor's editor.cpp).
int Slew_limitations = 0;

// Editor-side lighting globals (editor_lighting.cpp / rad_init.cpp in MFC).
int BestFit = 0;
int Shoot_from_patch = 1;

// Editor room/face/portal editing context (defined in the MFC editor).
room *Curroomp = nullptr;
int Curface = -1;
int Curedge = 0;
int Curvert = 0;
int Curportal = -1;
int Current_trigger = -1;
int Cur_object_index = -1;
room *Markedroomp = nullptr;
int Markedface = 0;
int Markededge = 0;
int Markedvert = 0;
int Placed_room = -1;
group *Placed_group = nullptr;
int Placed_room_face = 0;
int Placed_door = -1;
float Placed_room_angle = 0;
vector Placed_room_origin = {0, 0, 0};
matrix Placed_room_orient = IDENTITY_MATRIX;
vector Placed_room_attachpoint = {0, 0, 0};
matrix Placed_room_rotmat = IDENTITY_MATRIX;
room *Placed_baseroomp = nullptr;
int Placed_baseface = 0;
bool Mine_changed = false;
int Editor_view_mode = 0; // VM_MINE
int Editor_viewer_id = -1;

// SLEW.cpp guards SlewControlInit() with EDITOR; slew.cpp provides it.


// Editor-only object helpers guarded by EDITOR in object.cpp/objinfo.cpp; the
// D3 core is compiled without that define, so provide them here.
const char *Object_type_names[MAX_OBJECT_TYPES] = {
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
  Q_ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
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
  Q_ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
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

// Path editing helpers (editor/EPath.cpp in the MFC editor).
uint8_t Show_paths = 1;

int InsertNodeIntoPath(int pathnum, int nodenum, int flags, int roomnum, vector pos, matrix orient) {
  if (GamePaths[pathnum].num_nodes >= MAX_NODES_PER_PATH) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Path already has its maximum amount of nodes.");
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

int AllocGamePath() {
  for (int i = 0; i < MAX_GAME_PATHS; i++) {
    if (GamePaths[i].used == 0) {
      GamePaths[i].used = 1;
      GamePaths[i].name[0] = 0;
      GamePaths[i].num_nodes = 0;
      GamePaths[i].flags = 0;
      GamePaths[i].pathnodes = mem_rmalloc<node>(MAX_NODES_PER_PATH);
      Num_game_paths++;
      return i;
    }
  }
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Too many paths to add another.");
  return -1;
}

int MovePathNodeToPos(int pathnum, int nodenum, vector *attempted_pos) {
  fvi_query fq;
  fvi_info hit_info;

  fq.p0 = &GamePaths[pathnum].pathnodes[nodenum].pos;
  fq.startroom = GamePaths[pathnum].pathnodes[nodenum].roomnum;
  fq.p1 = attempted_pos;
  fq.rad = 0.0f;
  fq.thisobjnum = -1;
  fq.ignore_obj_list = NULL;
  fq.flags = FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;
  fvi_FindIntersection(&fq, &hit_info);

  if (nodenum >= 1) {
    fvi_query fq1;
    fvi_info hit_info1;
    fq1.p0 = &GamePaths[pathnum].pathnodes[nodenum - 1].pos;
    fq1.startroom = GamePaths[pathnum].pathnodes[nodenum - 1].roomnum;
    fq1.p1 = &hit_info.hit_pnt;
    fq1.rad = 0.0f;
    fq1.thisobjnum = -1;
    fq1.ignore_obj_list = NULL;
    fq1.flags = FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;
    fvi_FindIntersection(&fq1, &hit_info1);
    if (vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot move point.  No line of sight from the previous node to the new position.");
      return -1;
    }
  }

  if (nodenum < GamePaths[pathnum].num_nodes - 1) {
    fvi_query fq1;
    fvi_info hit_info1;
    fq1.p0 = &GamePaths[pathnum].pathnodes[nodenum + 1].pos;
    fq1.startroom = GamePaths[pathnum].pathnodes[nodenum + 1].roomnum;
    fq1.p1 = &hit_info.hit_pnt;
    fq1.rad = 0.0f;
    fq1.thisobjnum = -1;
    fq1.ignore_obj_list = NULL;
    fq1.flags = FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;
    fvi_FindIntersection(&fq1, &hit_info1);
    if (vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot move point.  No line of sight from the next node to the new position.");
      return -1;
    }
  }

  GamePaths[pathnum].pathnodes[nodenum].pos = hit_info.hit_pnt;
  GamePaths[pathnum].pathnodes[nodenum].roomnum = hit_info.hit_room;
  return 0;
}

int MovePathNode(int pathnum, int nodenum, vector *delta_pos) {
  vector attempted_pos = GamePaths[pathnum].pathnodes[nodenum].pos + *delta_pos;
  return MovePathNodeToPos(pathnum, nodenum, &attempted_pos);
}

int GetNextPath(int n) {
  Q_ASSERT(n >= 0 && n < MAX_GAME_PATHS);
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
  Q_ASSERT(n >= 0 && n < MAX_GAME_PATHS);
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

// Editor-only room selection helper (defined in editor/selectedroom.cpp).
int IsRoomSelected(int roomnum) {
  for (int i = 0; i < N_selected_rooms; i++)
    if (Selected_rooms[i] == roomnum)
      return 1;
  return 0;
}

// Editor-only room/face helpers (defined in editor/Erooms.cpp /
// editor/selectedroom.cpp). The Qt port's level_io.cpp + room_ops.cpp use
// these to construct / tear down rooms with full geometry; they preserve
// the public signatures so qteditor links cleanly.

room *CreateNewRoom(int nverts, int nfaces, bool palette_room) {
  (void)palette_room;
  room *rp = new room();
  memset(rp, 0, sizeof(room));
  rp->used = 1;
  rp->num_verts = nverts;
  rp->num_faces = nfaces;
  rp->verts = new vector[nverts]();
  rp->faces = new face[nfaces]();
  return rp;
}

// Counterpart of CreateNewRoom() that releases the per-room vectors/faces
// array and marks the slot free. The full Win32 path also walks the
// portal list, recycles to the free list, and detaches from the marked
// room; the Qt port stops at "free the slot" because Curroomp tracking
// lives at the qteditor level, not in Descent3Core.
void DestroyRoom(int roomnum) {
  if (roomnum < 0 || roomnum >= MAX_ROOMS)
    return;
  room *rp = &Rooms[roomnum];
  if (!rp->used)
    return;
  if (rp->verts != nullptr) {
    delete[] rp->verts;
    rp->verts = nullptr;
  }
  if (rp->faces != nullptr) {
    delete[] rp->faces;
    rp->faces = nullptr;
  }
  if (rp->portals != nullptr) {
    delete[] rp->portals;
    rp->portals = nullptr;
  }
  rp->used = 0;
  rp->num_verts = 0;
  rp->num_faces = 0;
  rp->num_portals = 0;
}

void AssignDefaultUVsToRoomFace(room *rp, int facenum) {
  if (rp == nullptr || !rp->used)
    return;
  if (facenum < 0 || facenum >= rp->num_faces)
    return;
  face *fp = &rp->faces[facenum];
  if (fp->num_verts < 3)
    return;

  for (int t = 0; t < fp->num_verts; t++) {
    GetUVLForRoomPoint(ROOMNUM(rp), facenum, t, &fp->face_uvls[t]);
    fp->face_uvls[t].alpha = 255;
  }
}

// Editor-only room selection list (editor/selectedroom.cpp in Win32).
void ClearRoomSelectedList() {
  N_selected_rooms = 0;
  State_changed = true;
}

void AddRoomToSelectedList(int roomnum) {
  if (!IsRoomSelected(roomnum)) {
    Selected_rooms[N_selected_rooms++] = roomnum;
    State_changed = true;
  }
}

void RemoveRoomFromSelectedList(int roomnum) {
  for (int i = 0; i < N_selected_rooms; i++) {
    if (Selected_rooms[i] == roomnum) {
      for (int j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      State_changed = true;
      return;
    }
  }
}

int ToggleRoomSelectedState(int roomnum) {
  State_changed = true;
  for (int i = 0; i < N_selected_rooms; i++) {
    if (Selected_rooms[i] == roomnum) {
      for (int j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      return 0;
    }
  }
  Selected_rooms[N_selected_rooms++] = roomnum;
  return 1;
}

int SelectConnectedRooms(int roomnum) {
  if (IsRoomSelected(roomnum))
    return 0;

  Selected_rooms[N_selected_rooms++] = roomnum;
  int count = 1;
  State_changed = true;

  for (int s = 0; s < Rooms[roomnum].num_portals; s++) {
    if (Rooms[roomnum].portals[s].croom != -1)
      count += SelectConnectedRooms(Rooms[roomnum].portals[s].croom);
  }
  return count;
}

static int *Save_selected_rooms = nullptr;
static int N_save_selected_rooms = -1;

void SaveRoomSelectedList() {
  if (N_save_selected_rooms != -1)
    return;

  N_save_selected_rooms = N_selected_rooms;
  if (!N_save_selected_rooms)
    return;

  Save_selected_rooms = new int[N_save_selected_rooms];
  for (int i = 0; i < N_selected_rooms; i++)
    Save_selected_rooms[i] = Selected_rooms[i];
}

void RestoreRoomSelectedList() {
  if (N_save_selected_rooms == -1)
    return;

  N_selected_rooms = N_save_selected_rooms;
  N_save_selected_rooms = -1;

  if (!N_selected_rooms)
    return;

  for (int i = 0; i < N_selected_rooms; i++)
    Selected_rooms[i] = Save_selected_rooms[i];

  delete[] Save_selected_rooms;
  Save_selected_rooms = nullptr;
}


// SaveLevel lives in Descent3/LoadLevel.cpp but its definition #includes
// "editor/ebnode.h" mid-file; Descent3Core doesn't have editor/ in its
// include path, so the symbol never makes it into libDescent3Core.a. We
// stub it at editor-side scope so the Qt port's level_io.cpp can keep
// EditorSaveLevel's contract (success → true) until the engine path lands.
// EditorStatus/SetErrorMessage/GetErrorMessage live in editor/MainFrm.cpp which
// is not linked into the Qt port.  Provide lightweight implementations here.
static char Editor_error_message[512] = "";

void EditorStatus(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(Editor_error_message, sizeof(Editor_error_message), format, args);
  va_end(args);
}

void SetErrorMessage(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(Editor_error_message, sizeof(Editor_error_message), fmt, args);
  va_end(args);
}

const char *GetErrorMessage() { return Editor_error_message; }
