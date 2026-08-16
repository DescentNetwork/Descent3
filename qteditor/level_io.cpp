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
#include "crossplat.h"
#include "d3x.h"
#include "ddio.h"
#include "doorway.h"
#include "Erooms.h"
#include "findintersection.h"
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
#include "moveworld.h"
#include "object.h"
#include "osiris_predefs.h"
#include "player.h"
#include "polymodel.h"
#include "renderer.h"
#include "room.h"
#include "selectedroom.h"
#include "special_face.h"
#include "terrain.h"
#include "trigger.h"

#include <cstdio>
#include <cstring>

namespace QtEditor {

// Trim leading and trailing spaces in-place; returns whether anything was
// stripped. Verbatim from editor/HFile.cpp and free of game-state deps so
// the Qt port's level_io.cpp can re-export it for tests.
bool StripLeadingTrailingSpaces(char *s) {
  if (s == nullptr)
    return false;
  bool stripped = false;
  char *t = s;
  while (*t == ' ')
    t++;
  if (t != s) {
    std::strcpy(s, t);
    stripped = true;
  }
  for (t = s + std::strlen(s) - 1; t >= s && *t == ' '; t--) {
    *t = 0;
    stripped = true;
  }
  return stripped;
}

// Number of leading spaces needed to right-justify a numeric column in
// RenderLevelStats; same trick as the Win32 IntSpacing helper.
char *IntSpacing(int i) {
  static char spaces[] = "               ";
  i = std::abs(i);
  int n;
  for (n = 1; i >= 10; n++)
    i /= 10;
  return spaces + n * 2 + n / 2;
}


// Build a fresh empty mine. The renderer-coupled callers from the Win32
// entry point (ResetWireframeView, SetEditorViewer, …) stay in qteditor; on
// Linux we seed the editor-only globals (Curroomp, Curface, Markedroomp…),
// call FreeAllRooms / FreeAllObjects from Descent3Core, then leave the cube
// construction until the engine-side room walker ships. The full Win32
// version's CreateDefaultRoom() (16-vertex / 10-face cube at Mine_origin)
// is left as a comment block below for reference.
void CreateNewMine() {
  // Reset selection / viewer globals. These live in qteditor/d3_editor_state.cpp
  // because the Win32 editor's EDVARS.cpp lives behind MFC and doesn't link.
  Curface = Curedge = Curvert = 0;
  Curportal = -1;
  Curroomp = nullptr;
  Markedroomp = nullptr;
  Placed_room = -1;
  Placed_group = nullptr;
  Num_triggers = 0;
  Current_trigger = -1;
  Editor_view_mode = VM_MINE;
  Editor_viewer_id = -1;
  New_mine = 1;
  World_changed = 0;

  // Tear down any pre-existing mine. FreeAllRooms / FreeAllObjects come from
  // Descent3Core.
  FreeAllRooms();
  FreeAllObjects();

  // Stamp default metadata onto Level_info so Save As writes sane defaults.
  std::strcpy(Level_info.name, "Unnamed");
  std::strcpy(Level_info.designer, "Anonymous");
  std::strcpy(Level_info.copyright,
              "Copyright (c) 1999 Outrage Entertainment, Inc.");
  std::strcpy(Level_info.notes, "");

  // ==== Win32 CreateDefaultRoom portion (descent3/room.cpp + editor/Erooms.cpp)
  //      is gated behind ATL/MFC callers until qteditor links them. The body
  //      below is the verbatim geometry from HFile.cpp; restore it once
  //      Erooms.cpp ports.
  /*
  vector default_room_verts[] = {...};
  vector Mine_origin = {...};
  room *CreateDefaultRoom() { rp = CreateNewRoom(16, 10, 0); ... }
  */
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
    if (objp->type != OBJ_NONE && objp->name != nullptr) {
      const int handle = osipf_FindObjectName(objp->name);
      if (handle != objp->handle)
        std::fprintf(stderr, "[level_io] duplicate object name \"%s\"\n",
                     objp->name);
      if (StripLeadingTrailingSpaces(objp->name))
        std::fprintf(stderr,
                     "[level_io] stripped spaces from object %d name\n", i);
    }
  }
  trigger *tp;
  for (i = 0, tp = Triggers; i < Num_triggers; i++, tp++) {
    if (tp->name != nullptr) {
      const int n = osipf_FindTriggerName(tp->name);
      if (n != i)
        std::fprintf(stderr, "[level_io] duplicate trigger name \"%s\"\n",
                     tp->name);
      if (StripLeadingTrailingSpaces(tp->name))
        std::fprintf(stderr,
                     "[level_io] stripped spaces from trigger %d name\n", i);
    }
  }
  room *rp;
  for (i = 0, rp = Rooms; i <= Highest_room_index; i++, rp++) {
    if (rp->used && rp->name != nullptr) {
      const int n = osipf_FindRoomName(rp->name);
      if (n != i)
        std::fprintf(stderr, "[level_io] duplicate room name \"%s\"\n",
                     rp->name);
      if (StripLeadingTrailingSpaces(rp->name))
        std::fprintf(stderr,
                     "[level_io] stripped spaces from room %d name\n", i);
    }
  }
}

bool EditorLoadLevel(const char *filename) {
  if (filename == nullptr)
    return false;
  // LoadLevel takes an optional progress callback; we don't surface the
  // progress UI yet, so pass nullptr and call the engine.
  if (!LoadLevel(const_cast<char *>(filename), nullptr))
    return false;
  CheckLevelNames();
  New_mine = 1;
  return true;
}

int EditorSaveLevel(const char *filename) {
  if (filename == nullptr)
    return 0;
  // SaveLevel lives in Descent3/LoadLevel.cpp behind an
  // "#include editor/ebnode.h" mid-file; Descent3Core doesn't compile that
  // path on Linux (editor/) so we report success/0 honestly. The Qt port
  // will replace this once editor/ebnode.h's MFC deps (EditorMessageBox)
  // have a Linux equivalent.
  if (!SaveLevel(const_cast<char *>(filename), true))
    return 0;
  Mine_changed = 0;
  return 1;
}

// Compose the multi-line "Level Stats:" report described in
// editor/HFile.cpp::ShowLevelStats(). Returns a heap buffer owned by the
// caller; delete[] when done.
char *RenderLevelStats() {
  static constexpr int BUF_LEN = 5000;
  char *text_buf = new char[BUF_LEN];

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
    if (rp->flags & RF_EXTERNAL)
      n_rooms_external++;
    else
      total_volume_bytes += GetVolumeSizeOfRoom(rp);

    for (int t = 0; t < rp->num_faces; t++) {
      face *fp = &rp->faces[t];
      if (fp->special_handle != BAD_SPECIAL_FACE_INDEX &&
          GameTextures[fp->tmap].flags & TF_SPECULAR &&
          fp->lmi_handle != BAD_LMI_INDEX)
        spec_faces++;
    }
    if (rp->flags & RF_DOOR)
      n_doors++;
    if (rp->flags & RF_SPECIAL1)
      num_sp1++;
    if (rp->flags & RF_SPECIAL2)
      num_sp2++;
    if (rp->flags & RF_SPECIAL3)
      num_sp3++;
    if (rp->flags & RF_SPECIAL4)
      num_sp4++;
    if (rp->flags & RF_SPECIAL5)
      num_sp5++;
    if (rp->flags & RF_SPECIAL6)
      num_sp6++;
    if (rp->flags & RF_GOAL1)
      num_redgoals++;
    if (rp->flags & RF_GOAL2)
      num_bluegoals++;
    if (rp->flags & RF_GOAL3)
      num_greengoals++;
    if (rp->flags & RF_GOAL4)
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

  std::snprintf(text_buf, BUF_LEN,
                "Level Stats:\n"
                "\n"
                "%s%d   Rooms (%d external)\n"
                "%s%d   Faces\n"
                "%s%d   Vertices\n"
                "\n"
                "%s%d   Portals\n"
                "%s%d   Doors\n"
                "\n"
                "%s%d   Polygon Objects (%d inside, %d outside)\n"
                "%s%d   Object Faces (%d with lightmaps)\n"
                "\n"
                "%s%d   Total lightmap faces\n"
                "%d	Total volume bytes\n"
                "%d   Total bytes in lightmaps\n"
                "%d   Total specular faces\n"
                "%d   Bytes wasted in lightmaps\n"
                "\n"
                "%d Red Goals\n"
                "%d Blue Goals\n"
                "%d Green Goals\n"
                "%d Yellow Goals\n"
                "%d Special 1 Rooms\n"
                "%d Special 2 Rooms\n"
                "%d Special 3 Rooms\n"
                "%d Special 4 Rooms\n"
                "%d Special 5 Rooms\n"
                "%d Special 6 Rooms\n",
                IntSpacing(n_rooms), n_rooms, n_rooms_external,
                IntSpacing(n_faces), n_faces,
                IntSpacing(n_verts), n_verts,
                IntSpacing(n_portals / 2), n_portals / 2,
                IntSpacing(n_doors), n_doors,
                IntSpacing(n_objects), n_objects,
                n_objects - n_objects_outside, n_objects_outside,
                IntSpacing(n_object_faces), n_object_faces,
                n_object_lightmap_faces,
                IntSpacing(n_faces + n_object_lightmap_faces),
                n_faces + n_object_lightmap_faces,
                total_volume_bytes, lm_bytes, spec_faces, bytes_wasted,
                num_redgoals, num_bluegoals, num_greengoals, num_yellowgoals,
                num_sp1, num_sp2, num_sp3, num_sp4, num_sp5, num_sp6);
  return text_buf;
}

} // namespace QtEditor
