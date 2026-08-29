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

// Qt port of editor/HFile.cpp. The Win32 sources pulled in the full MFC
// stack because CreateDefaultRoom / CreateNewMine / EditorLoadLevel /
// EditorSaveLevel touch game-state structures (Rooms[], Objects[], the path
// system, BOA cache, matcen list) compiled out of Descent3Core. The Qt port
// uses Descent3Core for those primitives where they're available and skips
// the renderer-dependent bits that still belong to the Win32 CMainFrame.

#include "level_io.h"

#include "d3edit.h"

#include "aiambient.h"
#include "BOA.h"
#include "bnode.h"

#include "doorway.h"
//#include "Erooms.h"
#include "editor_room_state.h"
#include "findintersection.h"
#include "selectedroom.h"
#include "game.h"
#include "gametexture.h"
#include "gameevent.h"
#include "gamepath.h"
#include "levelgoal.h"
#include "lighting.h"
#include "lightmap.h"
#include "lightmap_info.h"
#include "LoadLevel.h"
#include "manage.h"
#include "matcen.h"
#include "mem.h"
//#include "moveworld.h"
#include "object.h"
#include "osiris_predefs.h"
#include "player.h"
#include "polymodel.h"
#include "renderer.h"
#include "room.h"
//#include "selectedroom.h"
#include "special_face.h"
#include "terrain.h"
#include "trigger.h"
#include "vecmat.h"

#include <cstdio>
#include <cstring>
#include <sstream>


// Number of leading spaces needed to right-justify a numeric column in
// RenderLevelStats; same trick as the Win32 IntSpacing helper.
std::string IntSpacing(int i) {
  i = std::abs(i);
  int n;
  for (n = 1; i >= 10; n++)
    i /= 10;
  return std::string(2 * n + n / 2, ' ');
}


// ---- Default room geometry (from editor/HFile.cpp) ----
// Vertices for the octagonal prism created by CreateNewMine().
static vector default_room_verts[] = {
    {-10, 8, 20},  {-5, 10, 20},  {5, 10, 20},    {10, 8, 20},
    {10, -8, 20},  {5, -10, 20},  {-5, -10, 20},  {-10, -8, 20},
    {-10, 8, -20}, {-5, 10, -20}, {5, 10, -20},   {10, 8, -20},
    {10, -8, -20}, {5, -10, -20}, {-5, -10, -20}, {-10, -8, -20}};

// Center of the mine world, matching editor/HFile.cpp.
static vector Mine_origin = {float(TERRAIN_WIDTH * (TERRAIN_SIZE / 2)),
                             -100,
                             float(TERRAIN_DEPTH * (TERRAIN_SIZE / 2))};

// Find the first unused slot in Rooms[]. Returns -1 if all slots are in use.
static int GetEditorFreeRoom() {
  for (int i = 0; i < MAX_ROOMS; i++)
    if (!Rooms[i].used)
      return i;
  return -1;
}

// Create a default octagonal prism room for a new mine.
// Port of editor/HFile.cpp:CreateDefaultRoom().
static room *CreateDefaultRoom() {
  const int slot = GetEditorFreeRoom();
  if (slot < 0)
    return nullptr;

  room *rp = &Rooms[slot];
  InitRoom(rp, 16, 10, 0);

  // Set the 16 vertices, offset to Mine_origin.
  for (int i = 0; i < 16; i++)
    rp->verts[i] = default_room_verts[i] + Mine_origin;

  // Face 0: front octagonal cap (verts 0..7).
  InitRoomFace(&rp->faces[0], 8);
  for (int i = 0; i < 8; i++)
    rp->faces[0].face_verts[i] = i;

  // Face 1: back octagonal cap (verts 15..8, reversed).
  InitRoomFace(&rp->faces[1], 8);
  for (int i = 0; i < 8; i++)
    rp->faces[1].face_verts[i] = 15 - i;

  // Faces 2..9: eight side quads connecting front and back edges.
  for (int i = 0; i < 8; i++) {
    InitRoomFace(&rp->faces[i + 2], 4);
    rp->faces[i + 2].face_verts[0] = i;
    rp->faces[i + 2].face_verts[1] = i + 8;
    rp->faces[i + 2].face_verts[2] = ((i + 1) % 8) + 8;
    rp->faces[i + 2].face_verts[3] = (i + 1) % 8;
  }

  // Compute normals, assign textures and UVs for every face.
  for (int i = 0; i < 10; i++) {
    ComputeFaceNormal(rp, i);
    rp->faces[i].tmap = i + 1;
    AssignDefaultUVsToRoomFace(rp, i);
  }

  if (slot > Highest_room_index)
    Highest_room_index = slot;

  return rp;
}

// Build a fresh empty mine.
// Port of editor/HFile.cpp:CreateNewMine().
void CreateNewMine() {
  // Tear down any pre-existing mine.
  FreeAllRooms();
  FreeAllObjects();
  Viewer_object = nullptr;
  Player_object = nullptr;

  // Create the default room (octagonal prism at Mine_origin).
  Curroomp = CreateDefaultRoom();

  // Reset selection / viewer globals.
  Curface = Curedge = Curvert = 0;
  Curportal = -1;
  New_mine = true;
  World_changed = false;

  // Reset the view position for the orbit camera.
  Editor_view_mode = VM_MINE;
  Editor_viewer_id = -1;

  // Clear the marked room and selected segments.
  Markedroomp = nullptr;
  ClearRoomSelectedList();

  // Clear the placed room & group.
  Placed_room = -1;
  Placed_group = nullptr;

  // Reset triggers.
  Num_triggers = 0;
  Current_trigger = -1;

  // Reset terrain.
  ResetTerrain(1);
  ClearTerrainSound();

  // Clear game events and paths.
  ClearAllEvents();
  InitGamePaths();

  // Reset matcens and ambient life.
  DestroyAllMatcens();
  a_life.ALReset();
  Level_goals.CleanupAfterLevel();

  // Reset BNode and physics globals.
  BNode_ClearBNodeInfo();
  FVI_always_check_ceiling = false;
  Ceiling_height = MAX_TERRAIN_HEIGHT;

  // Reset sound overrides and force field bounces.
  sound_override_force_field = -1;
  sound_override_glass_breaking = -1;
  for (int i = 0; i < MAX_FORCE_FIELD_BOUNCE_TEXTURES; i++) {
    force_field_bounce_texture[i] = -1;
    force_field_bounce_multiplier[i] = 1.0f;
  }
  Level_powerups_ignore_wind = false;

  // BOA checksums.
  BOA_AABB_checksum = BOA_mine_checksum = 0;
  for (int i = 0; i < MAX_ROOMS; i++)
    BOA_AABB_ROOM_checksum[i] = 0;

  // Init level info.
  Level_info.name = "Unnamed";
  Level_info.designer = "Anonymous";
  Level_info.copyright = "Copyright (c) 1999 Outrage Entertainment, Inc.";
  Level_info.notes.clear();
}

// Walk the level's named entities (objects, triggers, rooms) and surface
// duplicate-name / leading-or-trailing-space warnings. The Win32 build
// routed these through EditorMessageBox; on the Qt port we only log them
// loudly because popping a modal dialog inside EditorLoadLevel isn't safe
// while the engine is mid-flight.
void CheckLevelNames() {
  int i;
  object *objp;
  for (i = 0, objp = Objects; i <= Highest_object_index; i++, objp++) {
    if (objp->type != OBJ_NONE && !objp->name.empty()) {
      const int handle = osipf_FindObjectName(objp->name);
      if (handle != objp->handle)
        std::fprintf(stderr, "[level_io] duplicate object name \"%s\"\n",
                     objp->name);
    }
  }
  trigger *tp;
  for (i = 0, tp = Triggers; i < Num_triggers; i++, tp++) {
    if (tp != nullptr && !tp->name.empty()) {
      const int n = osipf_FindTriggerName(tp->name);
      if (n != i)
        std::fprintf(stderr, "[level_io] duplicate trigger name \"%s\"\n",
                     tp->name.c_str());
    }
  }
  room *rp;
  for (i = 0, rp = Rooms; i <= Highest_room_index; i++, rp++) {
    if (rp->used && !rp->name.empty()) {
      const int n = osipf_FindRoomName(rp->name);
      if (n != i)
        std::fprintf(stderr, "[level_io] duplicate room name \"%s\"\n",
                     rp->name);
    }
  }
}

bool EditorLoadLevel(const std::filesystem::path& filename) {
  if (filename.empty())
    return false;
  // LoadLevel takes an optional progress callback; we don't surface the
  // progress UI yet, so pass nullptr and call the engine.
  if (!LoadLevel(filename, nullptr))
    return false;
  // LoadLevel → FreeAllObjects leaves Viewer_object dangling (see comment
  // in CreateNewMine).  Null it so updateCamera() uses orbit fallback.
  Viewer_object = nullptr;
  CheckLevelNames();
  New_mine = true;
  return true;
}

bool EditorSaveLevel(const std::filesystem::path& filename) {
  if (filename.empty())
    return false;
  // SaveLevel lives in Descent3/LoadLevel.cpp behind an
  // "#include editor/ebnode.h" mid-file; Descent3Core doesn't compile that
  // path on Linux (editor/) so we report success/0 honestly. The Qt port
  // will replace this once editor/ebnode.h's MFC deps (EditorMessageBox)
  // have a Linux equivalent.
  if (!SaveLevel(filename, true))
    return false;
  Mine_changed = false;
  return true;
}

// Compose the multi-line "Level Stats:" report described in
// editor/HFile.cpp::ShowLevelStats(). Returns the report as a std::string.
std::string RenderLevelStats() {
  std::ostringstream oss;

  int n_rooms = 0, n_rooms_external = 0, n_faces = 0, n_verts = 0;
  int n_objects = 0, n_portals = 0, n_doors = 0, n_objects_outside = 0;
  int n_object_faces = 0, n_object_lightmap_faces = 0;
  int bytes_wasted = 0, spec_faces = 0, lm_bytes = 0;
  int total_volume_bytes = 0;
  int num_redgoals = 0, num_bluegoals = 0, num_greengoals = 0,
      num_yellowgoals = 0;
  int num_sp1 = 0, num_sp2 = 0, num_sp3 = 0, num_sp4 = 0, num_sp5 = 0,
      num_sp6 = 0;
  uint8_t lightmaps_used[MAX_LIGHTMAPS]{};

  int i;
  room *rp;
  for (i = 0, rp = Rooms; i <= Highest_room_index; i++, rp++) {
    if (!rp->used)
      continue;
    n_rooms++;
    n_verts += rp->num_verts;
    n_faces += rp->num_faces;
    n_portals += rp->num_portals;
    if (rp->flags.external)
      n_rooms_external++;
    else
      total_volume_bytes += GetVolumeSizeOfRoom(rp);

    for (int t = 0; t < rp->num_faces; t++) {
      face *fp = &rp->faces[t];
      if (fp->special_handle != BAD_SPECIAL_FACE_INDEX &&
          (GameTextures[fp->tmap].flags.metal || GameTextures[fp->tmap].flags.marble ||
           GameTextures[fp->tmap].flags.plastic) &&
          fp->lmi_handle != BAD_LMI_INDEX)
        spec_faces++;
    }
    if (rp->flags.door)
      n_doors++;
    if (rp->flags.special1)
      num_sp1++;
    if (rp->flags.special2)
      num_sp2++;
    if (rp->flags.special3)
      num_sp3++;
    if (rp->flags.special4)
      num_sp4++;
    if (rp->flags.special5)
      num_sp5++;
    if (rp->flags.special6)
      num_sp6++;
    if (rp->flags.goal1)
      num_redgoals++;
    if (rp->flags.goal2)
      num_bluegoals++;
    if (rp->flags.goal3)
      num_greengoals++;
    if (rp->flags.goal4)
      num_yellowgoals++;
  }
  object *objp;
  for (i = 0, objp = Objects; i <= Highest_object_index; i++, objp++) {
    if (objp->type == OBJ_NONE || objp->type == OBJ_ROOM)
      continue;
    if (objp->render_type != RT_POLYOBJ)
      continue;
    n_objects++;
    if (OBJECT_OUTSIDE(objp))
      n_objects_outside++;
    poly_model *pm = GetPolymodelPointer(objp->rtype.pobj_info.model_num);
    if (pm == nullptr)
      continue;
    for (int m = 0; m < pm->n_models; m++) {
      n_object_faces += pm->submodel[m].num_faces;
      if (objp->lighting_render_type == LRT_LIGHTMAPS)
        n_object_lightmap_faces += pm->submodel[m].num_faces;
    }
  }

  for (i = 0; i < MAX_LIGHTMAP_INFOS; i++) {
    if (!LightmapInfo[i].used)
      continue;
    if (LightmapInfo[i].type == LMI_DYNAMIC ||
        LightmapInfo[i].type == LMI_TERRAIN)
      continue;
    lightmaps_used[LightmapInfo[i].lm_handle] = 1;
  }
  for (i = 0; i < MAX_LIGHTMAPS; i++) {
    if (!lightmaps_used[i])
      continue;
    uint16_t *data = lm_data(i);
    const int w = lm_w(i);
    const int h = lm_h(i);
    if (data == nullptr || w <= 0 || h <= 0)
      continue;
    for (int j = 0; j < w * h; j++) {
      if (!(data[j] & OPAQUE_FLAG))
        bytes_wasted += 2;
      else
        lm_bytes += 2;
    }
  }

  oss << "Level Stats:\n"
      << "\n"
      << IntSpacing(n_rooms) << n_rooms << "   Rooms (" << n_rooms_external
      << " external)\n"
      << IntSpacing(n_faces) << n_faces << "   Faces\n"
      << IntSpacing(n_verts) << n_verts << "   Vertices\n"
      << "\n"
      << IntSpacing(n_portals / 2) << n_portals / 2 << "   Portals\n"
      << IntSpacing(n_doors) << n_doors << "   Doors\n"
      << "\n"
      << IntSpacing(n_objects) << n_objects << "   Polygon Objects ("
      << n_objects - n_objects_outside << " inside, " << n_objects_outside
      << " outside)\n"
      << IntSpacing(n_object_faces) << n_object_faces << "   Object Faces ("
      << n_object_lightmap_faces << " with lightmaps)\n"
      << "\n"
      << IntSpacing(n_faces + n_object_lightmap_faces)
      << n_faces + n_object_lightmap_faces << "   Total lightmap faces\n"
      << total_volume_bytes << "\tTotal volume bytes\n"
      << lm_bytes << "   Total bytes in lightmaps\n"
      << spec_faces << "   Total specular faces\n"
      << bytes_wasted << "   Bytes wasted in lightmaps\n"
      << "\n"
      << num_redgoals << " Red Goals\n"
      << num_bluegoals << " Blue Goals\n"
      << num_greengoals << " Green Goals\n"
      << num_yellowgoals << " Yellow Goals\n"
      << num_sp1 << " Special 1 Rooms\n"
      << num_sp2 << " Special 2 Rooms\n"
      << num_sp3 << " Special 3 Rooms\n"
      << num_sp4 << " Special 4 Rooms\n"
      << num_sp5 << " Special 5 Rooms\n"
      << num_sp6 << " Special 6 Rooms\n";
  return oss.str();
}

