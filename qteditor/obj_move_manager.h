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

#ifndef OBJ_MOVE_MANAGER_H
#define OBJ_MOVE_MANAGER_H

#include "vecmat.h"

struct object;

const int OBJMOVEAXIS_X = 1, OBJMOVEAXIS_Y = 2, OBJMOVEAXIS_Z = 3, OBJMOVEAXIS_XY = 4, OBJMOVEAXIS_P = 5,
          OBJMOVEAXIS_H = 6, OBJMOVEAXIS_B = 7, OBJMOVEAXIS_PH = 8;

class ObjectMoveManager {
  int m_DragState;              // 0 = idle, 1 = dragging
  int m_MoveAxis;               // OBJMOVEAXIS_*
  int m_ObjNum;                 // object index being moved
  float m_WindowW2, m_WindowH2; // half-window dimensions for projection

  matrix m_ViewMat;
  vector3 m_ViewPos;

  void GetObjectDeltas(float *dx, float *dy, object *obj, int dsx, int dsy);

public:
  ObjectMoveManager();

  // Starts a drag.  view_width/view_height are the viewport dimensions in
  // pixels; view_pos/view_mat are the camera; x/y are the press coordinates
  // (not used directly, reserved for future cursor-clamping).
  void Start(int view_width, int view_height, vector3 *view_pos, matrix *view_mat, int x, int y);

  // Ends the current drag.
  void End();

  // Called on each mouse move while dragging.  dsx/dsy are the incremental
  // screen deltas since the last event and leftDown is whether the left mouse
  // button is still held.  Applies movement/rotation to the tracked object;
  // ends the drag when the button is released.
  void Defer(int dsx, int dsy, bool leftDown);

  bool IsMoving() const { return (m_DragState == 1); }

  void SetMoveAxis(int axis) { m_MoveAxis = axis; }
};

extern ObjectMoveManager ObjMoveManager;

#endif
