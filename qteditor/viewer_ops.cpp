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

#include "viewer_ops.h"

#include "d3edit.h"
#include "object.h"
#include "room.h"

#include <cstdio>

namespace QtEditor {

namespace {

// viewer state holder used by editor/MainFrm.cpp:3829. Default zoom is
// 1.0f (D3_DEFAULT_ZOOM in editor/editorView.cpp).
constexpr float kDefaultViewRadius = 1.0f;

} // namespace

void CenterViewOnMine() {
  if (Viewer_object == nullptr)
    return;
  // Editor_view_mode determines whether the editor mines-terrain split
  // is meaningful. We only recentre when the mode is VM_MINE; other
  // modes are left as-is so the viewport doesn't snap while the user
  // is poking at terrain.
  if (Editor_view_mode != VM_MINE)
    return;
  if (Curroomp == nullptr || Curroomp->num_verts <= 0)
    return;

  // Average the verts to find the centroid of the current room; the
  // Win32 OnViewCenterOnMine uses the same trick.
  vector centroid{};
  for (int i = 0; i < Curroomp->num_verts; ++i)
    centroid += Curroomp->verts[i];
  centroid /= static_cast<float>(Curroomp->num_verts);

  matrix idmat{};
  ObjSetPos(Viewer_object, &centroid, ROOMNUM(Curroomp), &idmat, false);
  State_changed = 1;
  std::fprintf(stderr,
               "[viewer_ops] CenterViewOnMine -> (%g,%g,%g) room %d\n",
               centroid.x(), centroid.y(), centroid.z(), ROOMNUM(Curroomp));
}

void CenterViewOnObject() {
  if (Viewer_object == nullptr)
    return;
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;

  // Win32 OnViewCenterOnObject places the viewer one unit behind the
  // target object's facing vector so the object stays visible after
  // the move.
  object *target = &Objects[Cur_object_index];
  vector pos = target->pos;
  pos -= target->orient.fvec;
  ObjSetPos(Viewer_object, &pos, target->roomnum, &target->orient, false);
  State_changed = 1;
  std::fprintf(stderr,
               "[viewer_ops] CenterViewOnObject -> (%g,%g,%g) room %d\n",
               pos.x(), pos.y(), pos.z(), target->roomnum);
}

void ResetViewRadius() {
  // Win32 OnViewResetViewRadius re-resets the wireframe view's zoom
  // radius to D3_DEFAULT_ZOOM. The Qt port can't drive WireframeGrWnd
  // (no GL surface yet) but updates D3EditState.texscale so the editor
  // state round-trips through QSettings cleanly.
  D3EditState.texscale = kDefaultViewRadius;
  State_changed = 1;
  std::fprintf(stderr, "[viewer_ops] ResetViewRadius -> %g\n",
               D3EditState.texscale);
}

void MoveViewToSelectedRoom() {
  if (Viewer_object == nullptr)
    return;
  int target_room = -1;
  if (Curroomp != nullptr && Curroomp->used)
    target_room = ROOMNUM(Curroomp);
  if (target_room < 0)
    return;
  // Pull the room's centroid; if the room is brand new with no verts
  // yet, just keep the viewer's current pos/orient and only update
  // roomnum (matches the Win32 fallback in editor/editorView.cpp).
  if (Curroomp->num_verts > 0) {
    vector centroid{};
    for (int i = 0; i < Curroomp->num_verts; ++i)
      centroid += Curroomp->verts[i];
    centroid /= static_cast<float>(Curroomp->num_verts);
    ObjSetPos(Viewer_object, &centroid, target_room, &Viewer_object->orient,
              false);
  } else {
    ObjSetPos(Viewer_object, &Viewer_object->pos, target_room,
              &Viewer_object->orient, false);
  }
  State_changed = 1;
  std::fprintf(stderr, "[viewer_ops] MoveViewToSelectedRoom -> room %d\n",
               target_room);
}

} // namespace QtEditor
