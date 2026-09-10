/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

// Qt port of editor/ObjMoveManager.cpp.
// Handles drag-based translation and rotation of objects in the 3D view.
// The Win32 version used CWnd/ClipCursor/GetWindowRect — all replaced
// with plain ddio mouse polling and Qt viewport dimensions.

#include "obj_move_manager.h"

#include <cmath>

#include "d3edit.h"
#include "object.h"
#include "object_ops.h"

ObjectMoveManager ObjMoveManager;

ObjectMoveManager::ObjectMoveManager() {
  m_DragState = 0;
  m_MoveAxis = OBJMOVEAXIS_X;
}

void ObjectMoveManager::Start(int view_width, int view_height, vector3 *view_pos, matrix *view_mat, int x, int y) {
  if (Cur_object_index < 0 || Cur_object_index >= Highest_object_index + 1)
    return;
  if (Objects[Cur_object_index].type == OBJ_DOOR)
    return;

  m_DragState = 1;
  m_WindowW2 = view_width / 2.0f;
  m_WindowH2 = view_height / 2.0f;
  m_ObjNum = Cur_object_index;

  m_ViewPos = *view_pos;
  m_ViewMat = *view_mat;
}

void ObjectMoveManager::End() {
  m_DragState = 0;
  Object_moved = false;
  World_changed = true;
}

#define ROTATE_SCALE (256.0f * 20.0f / ((obj->size < 10.0f) ? 10.0f : obj->size))

void ObjectMoveManager::Defer(int dsx, int dsy, bool leftDown) {
  if (m_DragState != 1)
    return;

  Object_moved = false;

  if (leftDown) {
    object *obj = &Objects[m_ObjNum];

    if (!dsx && !dsy)
      return;

    float dx, dy;
    GetObjectDeltas(&dx, &dy, obj, dsx, dsy);

    switch (m_MoveAxis) {
    case OBJMOVEAXIS_X:
      HObjectMove(Cur_object_index, dx, 0, 0);
      break;
    case OBJMOVEAXIS_Y:
      HObjectMove(Cur_object_index, 0, -dy, 0);
      break;
    case OBJMOVEAXIS_Z:
      HObjectMove(Cur_object_index, 0, 0, dy);
      break;
    case OBJMOVEAXIS_XY:
      HObjectMove(Cur_object_index, dx, -dy, 0);
      break;
    case OBJMOVEAXIS_P:
      Object_move_rotation = fabs(-dy) * ROTATE_SCALE;
      if (dy > 0)
        HObjectIncreasePitch();
      else if (dy < 0)
        HObjectDecreasePitch();
      break;
    case OBJMOVEAXIS_H:
      Object_move_rotation = fabs(dx) * ROTATE_SCALE;
      if (dx > 0)
        HObjectIncreaseHeading();
      else if (dx < 0)
        HObjectDecreaseHeading();
      break;
    case OBJMOVEAXIS_B:
      Object_move_rotation = fabs(dx) * ROTATE_SCALE;
      if (dx > 0)
        HObjectIncreaseBank();
      else if (dx < 0)
        HObjectDecreaseBank();
      break;
    case OBJMOVEAXIS_PH:
      Object_move_rotation = fabs(-dy) * ROTATE_SCALE;
      if (dy > 0)
        HObjectIncreasePitch();
      else if (dy < 0)
        HObjectDecreasePitch();
      Object_move_rotation = fabs(dx) * ROTATE_SCALE;
      if (dx > 0)
        HObjectIncreaseHeading();
      else if (dx < 0)
        HObjectDecreaseHeading();
      break;
    }
  } else {
    End();
  }
}

void ObjectMoveManager::GetObjectDeltas(float *dx, float *dy, object *obj, int dsx, int dsy) {
  vector3 pos;
  pos = obj->pos - m_ViewPos;
  pos = pos * m_ViewMat;

  if (pos.z() == 0.0f) {
    *dx = 0;
    *dy = 0;
    return;
  }

  *dx = ((scalar)dsx * pos.z()) / m_WindowW2;
  *dy = ((scalar)dsy * pos.z()) / m_WindowH2;
}
