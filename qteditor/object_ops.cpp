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

#include "object_ops.h"

#include "d3edit.h"
#include "object.h"
#include "player.h"
#include "room.h"

#include <cstdio>

namespace QtEditor {

namespace {

// Returns the first used object slot at or after `from`, wrapping back
// to 0 and wrapping forward to < Highest_object_index after the high-water
// mark so subsequent selects cycle through the editor's full inventory.
int find_used(int from) {
  if (Highest_object_index < 0)
    return -1;
  const int total = Highest_object_index + 1;
  for (int step = 0; step < total; ++step) {
    const int idx = (from + step) % total;
    if (Objects[idx].type != OBJ_NONE)
      return idx;
  }
  return -1;
}

} // namespace

int PlaceCameraAtViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return -1;
  if (Curroomp == nullptr)
    return -1;
  // Just succeed without allocating — the Win32 entry point's ObjCreate
  // path needs the object library on Linux, which isn't linked. Returning
  // -1 here is honest about what's stubbed while still letting the rest
  // of the helper set work.
  return -1;

  // Find an unused object slot to host the camera.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return -1;

  // Mirror the Win32 placement logic in editor/Placement.cpp: take the
  // viewer's pose and bump a bit on z so the camera isn't right on top
  // of the camera setup itself.
  vector pos = Viewer_object->pos;
  pos.z() += 1.0f;
  Objects[slot].type = OBJ_CAMERA;
  Objects[slot].render_type = RT_POLYOBJ;
  std::strncpy(Objects[slot].name, "Cam", sizeof(Objects[slot].name) - 1);
  ObjSetPos(&Objects[slot], &pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);

  Cur_object_index = slot;
  D3EditState.current_room = Viewer_object->roomnum;
  Mine_changed = 1;
  New_mine = 1;

  std::fprintf(stderr,
               "[object_ops] PlaceCameraAtViewer -> object %d\n", slot);
  return slot;
}

void SetViewerFromCamera() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  object *cam = &Objects[Cur_object_index];
  if (cam->type != OBJ_CAMERA)
    return;

  // In Win32 OnObjectSetViewerFromCamera, the viewer's pos/orient/roomnum
  // are copied from the camera. We follow that contract directly.
  if (Viewer_object != nullptr) {
    ObjSetPos(Viewer_object, &cam->pos, cam->roomnum, &cam->orient, false);
  }
  // Also propagate to the player object (object 0) so saving the level
  // from the editor preserves the latest camera-driven viewpoint.
  if (Player_object != nullptr)
    ObjSetPos(Player_object, &cam->pos, cam->roomnum, &cam->orient, false);
  State_changed = 1;
  std::fprintf(stderr, "[object_ops] SetViewerFromCamera: viewer=(%g,%g,%g) room %d\n",
               cam->pos.x(), cam->pos.y(), cam->pos.z(), cam->roomnum);
}

void SetCameraFromViewer() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  object *cam = &Objects[Cur_object_index];
  if (cam->type != OBJ_CAMERA)
    return;
  if (Viewer_object == nullptr)
    return;
  ObjSetPos(cam, &Viewer_object->pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);
  Mine_changed = 1;
  std::fprintf(stderr,
               "[object_ops] SetCameraFromViewer: camera=(%g,%g,%g) room %d\n",
               cam->pos.x(), cam->pos.y(), cam->pos.z(), cam->roomnum);
}

void DeleteCurrentObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  ObjDelete(Cur_object_index);
  const int was = Cur_object_index;
  Cur_object_index = -1;
  // After delete, walk forward to find the next used slot so the
  // editor's "next object" key keeps cycling correctly.
  Cur_object_index = find_used(was + 1);
  if (Cur_object_index < 0)
    Cur_object_index = -1;
  Mine_changed = 1;
  std::fprintf(stderr, "[object_ops] DeleteCurrentObject: removed %d, "
                       "Cur_object_index = %d\n",
               was, Cur_object_index);
}

void MovePlayerToCurrentRoom() {
  if (Curroomp == nullptr)
    return;
  if (Player_object == nullptr)
    return;

  // Win32 OnObjectMovePlayer rewinds the player to a known start state:
  // origin of the current room, identity matrix, roomnum from Curroomp.
  vector rp;
  const int slot = ROOMNUM(Curroomp);
  matrix idmat;
  ObjSetPos(Player_object, &rp, slot, &idmat, false);
  State_changed = 1;
  std::fprintf(stderr, "[object_ops] MovePlayerToCurrentRoom -> room %d\n",
               slot);
}

void SelectNextObject(int from) {
  const int idx = find_used(from + 1);
  if (idx >= 0)
    Cur_object_index = idx;
}

void SelectPrevObject(int from) {
  if (from <= 0)
    return;
  for (int i = from - 1; i >= 0; --i) {
    if (Objects[i].type != OBJ_NONE) {
      Cur_object_index = i;
      return;
    }
  }
  // Wrap to the highest-used slot.
  Cur_object_index = (Highest_object_index >= 0) ? Highest_object_index : -1;
}

} // namespace QtEditor
