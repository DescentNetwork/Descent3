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
#include "object_external.h" // object_type
#include "vecmat.h"

struct object;

// Port of editor/HObject.h constants.
constexpr float HOBJECT_SCALE_UNIT = 0.5f;
constexpr float HOBJECT_ROTATION_UNIT = 1024.0f;

// Object move direction constants (from HObject.h).
enum class object_move_dir : uint8_t {
  left = 1,
  right = 2,
  forward = 3,
  back = 4,
  up = 5,
  down = 6,
};

// Globals (from HObject.cpp).
extern float Object_move_scale;
extern angle Object_move_rotation;

// Placement.
bool HObjectPlace(object_type_e obj_type, uint16_t obj_id);
int GetSelectedTerrainCell();

// Object keypad page selection — ports SetCurrentIndex()/GetCurrentIndex() from
// editor/ObjectDialog.cpp:350. The keypad keeps one current id per object page
// and dispatches on the selected page type, rather than a single shared id.
std::optional<uint16_t> objectPageCurrentId(object_type_e page);
void setObjectPageCurrentId(object_type_e page, std::optional<uint16_t> id);

// The object id that Place Object should use for the currently selected keypad
// page, healing the remembered id if it no longer belongs to that page.
std::optional<uint16_t> currentObjectPageId();

// Movement.
void HObjectMove(int objnum, float dx, float dy, float dz);
void HObjectMoveToViewer(object& objp);

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
void ResetGroundObject(object& objp);

// Internal helpers (exposed for testing).
bool MoveObject(object& obj, vector3& newpos);
bool RotateObject(int objnum, angle p, angle h, angle b);
