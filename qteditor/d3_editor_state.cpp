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
#include "terrain.h"
#include "slew.h"
#include "manage.h"
#include "crossplat.h"
#include "objinfo.h"
#include "gamepath.h"
#include "room.h"
#include "vecmat.h"
#include <cstring>
#include <filesystem>
#include <QtGlobal>

#include "d3edit.h"

d3edit_state D3EditState;
bool World_changed = false;

// Editor-only terrain globals guarded by EDITOR in terrain.cpp; the D3 core is
// compiled without that define, so provide them here.
uint8_t TerrainSelected[TERRAIN_WIDTH * TERRAIN_DEPTH];
int Num_terrain_selected = 0;

// Editor-only state flags declared in d3edit.h (defined in the MFC editor's
// editor.cpp/EDVARS.cpp).
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
bool Mine_changed = false;
int Editor_view_mode = 0; // VM_MINE
int Editor_viewer_id = -1;

// SLEW.cpp guards SlewControlInit() with EDITOR; the Qt port has no controller
// integration, so provide a stub. The non-EDITOR branch of slew.h turns
// SlewControlInit into a no-arg function-like macro with an empty replacement
// list, so undef it before defining the real function body.
#undef SlewControlInit
void SlewControlInit() {}


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

  // Port of editor/Erooms.cpp:GetUVLForRoomPoint — projects each vertex onto
  // the face's normal plane and assigns UVs with a 1/20.0 world-to-UV scale.
  matrix face_matrix, trans_matrix;
  vector fvec;
  vector avg_vert;
  vector verts[MAX_VERTS_PER_FACE];
  vector rot_vert;

  // Find the center point of this face.
  vm_MakeZero(&avg_vert);
  int i;
  for (i = 0; i < fp->num_verts; i++)
    avg_vert += rp->verts[fp->face_verts[i]];
  avg_vert /= static_cast<float>(i);

  // Make the orientation matrix (reverse the normal: looking "at" the face).
  fvec = -fp->normal;
  vm_VectorToMatrix(&face_matrix, &fvec, nullptr, nullptr);

  // Extract angles and rebuild a pure-rotation matrix.
  angvec avec;
  vm_ExtractAnglesFromMatrix(&avec, &face_matrix);
  vm_AnglesToMatrix(&trans_matrix, avec.p(), avec.h(), avec.b());

  // Rotate all vertices into face-local space.
  for (i = 0; i < fp->num_verts; i++) {
    vector vert = rp->verts[fp->face_verts[i]];
    vert -= avg_vert;
    vm_MatrixMulVector(&rot_vert, &vert, &trans_matrix);
    verts[i] = rot_vert;
  }

  // Find leftmost, topmost, rightmost, bottommost vertices.
  int leftmost = 0, topmost = 0, rightmost = 0, bottommost = 0;
  for (i = 1; i < fp->num_verts; i++) {
    if (verts[i].x() < verts[leftmost].x())
      leftmost = i;
    if (verts[i].y() > verts[topmost].y())
      topmost = i;
    if (verts[i].x() > verts[rightmost].x())
      rightmost = i;
    if (verts[i].y() < verts[bottommost].y())
      bottommost = i;
  }

  // Base vector: (leftmost x, topmost y, 0).
  vector base_vector;
  base_vector.x() = verts[leftmost].x();
  base_vector.y() = verts[topmost].y();
  base_vector.z() = 0;

  // Assign UVs with 1/20.0 scale, matching the Win32 editor.
  for (i = 0; i < fp->num_verts; i++) {
    fp->face_uvls[i].u = (verts[i].x() - base_vector.x()) / 20.0f;
    fp->face_uvls[i].v = (base_vector.y() - verts[i].y()) / 20.0f;
    fp->face_uvls[i].alpha = 255;
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
bool SaveLevel(char *filename, bool f_save_room_AABB) {
  (void)f_save_room_AABB;
  // The stub refuses to scribble anything to disk: writing a stale or empty
  // .d3l would mask real bugs during development. The Qt port will swap
  // this for the real implementation once editor/ebnode.h's MFC deps
  // (EditorMessageBox, etc.) have a Linux equivalent.
  (void)filename;
  return false;
}
