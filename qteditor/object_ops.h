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

#pragma once

#include "fix.h"
#include "vecmat.h"

struct object;

// Port of editor/HObject.h constants.
constexpr float HOBJECT_SCALE_UNIT = 0.5f;
constexpr float HOBJECT_ROTATION_UNIT = 1024.0f;

// Object move direction constants (from HObject.h).
enum ObjectMoveDir {
  HOBJECT_MOVE_LEFT = 1,
  HOBJECT_MOVE_RIGHT = 2,
  HOBJECT_MOVE_FORWARD = 3,
  HOBJECT_MOVE_BACK = 4,
  HOBJECT_MOVE_UP = 5,
  HOBJECT_MOVE_DOWN = 6,
};

// Globals (from HObject.cpp).
extern float Object_move_scale;
extern angle Object_move_rotation;

// Placement.
bool HObjectPlace(int obj_type, int obj_id);
int GetSelectedTerrainCell();

// Movement.
void HObjectMove(int objnum, float dx, float dy, float dz);
void HObjectMoveToViewer(object *objp);

// Rotation.
void HObjectIncreaseBank();
void HObjectDecreaseBank();
void HObjectIncreasePitch();
void HObjectDecreasePitch();
void HObjectIncreaseHeading();
void HObjectDecreaseHeading();

// Orientation.
void HObjectSetDefault();
void HObjectFlip();

// Deletion.
void HObjectDelete();

// Terrain ground re-alignment.
void ResetGroundObject(object *objp);

// Internal helpers (exposed for testing).
bool MoveObject(object *obj, vector *newpos);
bool RotateObject(int objnum, angle p, angle h, angle b);
