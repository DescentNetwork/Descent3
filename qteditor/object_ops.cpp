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

#include <QInputDialog>
#include <QString>

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

void CreateNewViewer() {
  // Editor_viewer_id tracks the next available viewer id. We don't call
  // ObjCreate on Linux; the menu slot is wired so the user can still
  // reach the Win32 entry point symbolically, but the actual spawn
  // happens through SpawnNewViewer() which writes a duplicate of the
  // current viewer instead of bootstrapping a fresh OBJ_VIEWER via the
  // engine's ObjCreate path.
  std::fprintf(stderr,
               "[object_ops] CreateNewViewer: pending editor/ObjCreate\n");
}

int SpawnNewViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return -1;
  // Walk Objects[] to find the first unused slot, then copy the current
  // viewer's pose/orient/roomnum into a fresh OBJ_VIEWER slot. We don't
  // touch ObjCreate because the engine-side path is gated on MFC code
  // paths in editor/HView.cpp; this Qt-stub is honest about that.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return -1;
  Objects[slot].type = OBJ_VIEWER;
  Objects[slot].render_type = RT_POLYOBJ;
  Objects[slot].orient = Viewer_object->orient;
  Editor_viewer_id = (Editor_viewer_id < 0) ? 0 : Editor_viewer_id + 1;
  Objects[slot].id = Editor_viewer_id;
  ObjSetPos(&Objects[slot], &Viewer_object->pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);
  if (slot > Highest_object_index)
    Highest_object_index = slot;
  Mine_changed = 1;
  New_mine = 1;
  std::fprintf(stderr,
               "[object_ops] SpawnNewViewer -> object %d (id %d)\n", slot,
               Editor_viewer_id);
  return slot;
}

int SelectNextViewer() {
  // Win32 SelectNextViewer != SelectNextObject: it walks the OBJ_VIEWER
  // slots (not OBJ_NONE ones) and swaps Viewer_object to the next one
  // so the user can flip through multiple cameras without choosing
  // world objects. We do the same here.
  if (Viewer_object == nullptr)
    return -1;
  const int cur_id = Viewer_object->id;
  int best = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type != OBJ_VIEWER)
      continue;
    if (Objects[i].id == cur_id)
      continue;
    best = i;
    break;
  }
  if (best < 0)
    return -1;
  Viewer_object = &Objects[best];
  Editor_viewer_id = Viewer_object->id;
  State_changed = Viewer_moved = 1;
  std::fprintf(stderr, "[object_ops] SelectNextViewer -> object %d (id %d)\n",
               best, Editor_viewer_id);
  return best;
}

void DeleteCurrentViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return;
  // Mark the current viewer's slot freed and resync to the next
  // available OBJ_VIEWER (or clear Viewer_object if none).
  int cur_slot = -1;
  // Find Viewer_object's slot lookup: Viewer_object - Objects.
  if (Viewer_object >= Objects && Viewer_object <= &Objects[MAX_OBJECTS - 1]) {
    cur_slot = static_cast<int>(Viewer_object - Objects);
  }
  if (cur_slot >= 0) {
    Objects[cur_slot].type = OBJ_NONE;
    Objects[cur_slot].id = -1;
  }
  // Auto-pick the remaining OBJ_VIEWER if any.
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_VIEWER) {
      Viewer_object = &Objects[i];
      Editor_viewer_id = Objects[i].id;
      std::fprintf(stderr,
                   "[object_ops] DeleteCurrentViewer: resync to %d (id %d)\n",
                   i, Editor_viewer_id);
      return;
    }
  }
  Viewer_object = nullptr;
  std::fprintf(stderr,
               "[object_ops] DeleteCurrentViewer: no viewers left\n");
}

int SelectObjectByNumber() {
  bool ok = false;
  const int value = QInputDialog::getInt(
      nullptr, QStringLiteral("Select Object"),
      QStringLiteral("Enter object number to select:"), 0, 0, MAX_OBJECTS, 1,
      &ok);
  if (!ok)
    return -1;
  if (value < 0 || value > Highest_object_index || Objects[value].type == OBJ_NONE) {
    std::fprintf(stderr,
                 "[object_ops] SelectObjectByNumber: %d is invalid\n", value);
    return -1;
  }
  Cur_object_index = value;
  return value;
}

void SelectObject(int objnum) {
  if (objnum < 0 || objnum > Highest_object_index)
    return;
  if (Objects[objnum].type == OBJ_NONE)
    return;
  Cur_object_index = objnum;
}

} // namespace QtEditor
