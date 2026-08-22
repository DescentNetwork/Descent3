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

// Object operations ported from editor/HObject.cpp.
// All functions are reimplemented here since the editor library is not
// linked to the Qt port.

#include "object_ops.h"

#include "logger/log.h"
#include "d3edit.h"
#include "findintersection.h"

#include "object.h"
#include "physics.h"
#include "player.h"
#include "polymodel.h"

#include "qt_messagebox.h"
#include "ship.h"
#include "terrain.h"
#include "vecmat.h"

#include <cstring>

// ============================================================================
// Internal data
// ============================================================================

float Object_move_scale = HOBJECT_SCALE_UNIT;
angle Object_move_rotation = HOBJECT_ROTATION_UNIT;

#define OBJECT_PLACE_DIST (scalar)10.0
#define MOVE_EPSILON 0.1f

bool f_allow_objects_to_be_pushed_through_walls = false;

// ============================================================================
// GetSelectedTerrainCell — editor/HObject.cpp:263
// Finds the selected terrain cell.  Returns cell number, or -1 if none, -2 if
// more than one.
// ============================================================================
int GetSelectedTerrainCell() {
  int found_cellnum = -1;

  for (int i = 0; i < TERRAIN_DEPTH * TERRAIN_WIDTH; i++) {
    if (TerrainSelected[i]) {
      if (found_cellnum == -1)
        found_cellnum = i;
      else
        return -2;
    }
  }

  return found_cellnum;
}

// ============================================================================
// MoveObject (internal) — editor/HObject.cpp:575
// Attempt to set new object position using FVI.  Returns true if moved.
// ============================================================================
bool MoveObject(object *obj, vector *newpos) {
  fvi_query fq;
  fvi_info hit_info;

  bool use_radius = (obj->movement_type == MT_PHYSICS);

  fq.p0 = &obj->pos;
  fq.startroom = obj->roomnum;
  fq.p1 = newpos;
  fq.thisobjnum = OBJNUM(obj);
  fq.ignore_obj_list = NULL;
  fq.flags = FQ_IGNORE_RENDER_THROUGH_PORTALS;
  fq.rad = use_radius ? obj->size : 0.0f;

  if (f_allow_objects_to_be_pushed_through_walls)
    fq.flags |= (FQ_IGNORE_WALLS | FQ_IGNORE_TERRAIN | FQ_IGNORE_EXTERNAL_ROOMS);

  int fate = fvi_FindIntersection(&fq, &hit_info);

  if (fate == HIT_WALL)
    if (vm_VectorDistance(&obj->pos, &hit_info.hit_pnt) < MOVE_EPSILON)
      return false;

  ObjSetPos(obj, &hit_info.hit_pnt, hit_info.hit_room, NULL, false);
  return true;
}

// ============================================================================
// RotateObject (internal) — editor/HObject.cpp:612
// Applies a rotation to the specified object.
// ============================================================================
bool RotateObject(int objnum, angle p, angle h, angle b) {
  object *obj = &Objects[objnum];
  matrix rotmat;

  vm_AnglesToMatrix(&rotmat, p, h, b);
  obj->orient *= rotmat;

  vm_Orthogonalize(&obj->orient);
  ObjSetOrient(obj, &obj->orient);

  Object_moved = true;
  return true;
}

// ============================================================================
// HObjectPlace — editor/HObject.cpp:280
// Places a new object of the given type and ID into the world at the viewer's
// location, then repositions it onto the current surface.
// ============================================================================
bool HObjectPlace(int obj_type, int obj_id) {
  int objnum;
  object *objp;
  poly_model *pm;
  matrix orient = IDENTITY_MATRIX;

  // Special stuff for player ship
  if (obj_type == OBJ_PLAYER) {
    if (!Num_ships) {
      OutrageMessageBox("Cannot place a player: There are no player ships.");
      return false;
    }

    int ship_num = D3EditState.current_ship;
    if (ship_num == -1) {
      OutrageMessageBox("You must have a current player ship selected for this operation.");
      return false;
    }

    Players[obj_id].ship_index = ship_num;
  }

  if (obj_type != OBJ_POWERUP) {
    orient = Viewer_object->orient;
  }

  objnum = ObjCreate(obj_type, obj_id, Viewer_object->roomnum, &Viewer_object->pos, &orient);
  if (objnum == -1)
    return false;

  objp = &Objects[objnum];

  // If we have a ground plane, use current cell or face for position
  if ((objp->render_type == RT_POLYOBJ) &&
      ((pm = GetPolymodelPointer(objp->rtype.pobj_info.model_num)) != nullptr) &&
      pm->n_ground) {
    vector *surface_norm;
    vector pos;
    int roomnum;

    if (Editor_view_mode == VM_TERRAIN) {
      int cellnum = GetSelectedTerrainCell();
      if (cellnum == -1) {
        OutrageMessageBox("You must have a terrain cell selected to place an object.");
        ObjDelete(objnum);
        return false;
      }
      if (cellnum == -2) {
        OutrageMessageBox("You must have only one cell selected to place an object.");
        ObjDelete(objnum);
        return false;
      }

      ComputeTerrainSegmentCenter(&pos, cellnum);
      surface_norm = &TerrainNormals[MAX_TERRAIN_LOD - 1][cellnum].normal1;
      roomnum = MAKE_ROOMNUM(cellnum);
    } else {
      ComputeCenterPointOnFace(&pos, Curroomp, Curface);
      surface_norm = &Curroomp->faces[Curface].normal;
      roomnum = ROOMNUM(Curroomp);

      if (Rooms[roomnum].flags & RF_EXTERNAL)
        roomnum = GetTerrainRoomFromPos(&pos);
    }

    matrix groundplane_orient, surface_orient, object_orient;

    vector ground_point;
    vector ground_normal;
    vector to_ground;

    PhysCalcGround(&ground_point, &ground_normal, objp, 0);
    to_ground = objp->pos - ground_point;
    float dist = vm_Dot3Product(ground_normal, to_ground);
    pos += dist * (*surface_norm);

    vm_VectorToMatrix(&groundplane_orient, &pm->ground_slots[0].norm, NULL, NULL);
    vm_VectorToMatrix(&surface_orient, surface_norm);
    vm_MatrixMulTMatrix(&object_orient, &surface_orient, &groundplane_orient);

    ObjSetPos(objp, &pos, roomnum, &object_orient, false);
  } else {
    // No ground plane — move in front of viewer, facing viewer
    vector pos;

    if (Viewer_object->flags & OF_OUTSIDE_MINE) {
      ObjDelete(objnum);
      OutrageMessageBox("Cannot place the object here: the viewer is outside the mine.");
      return false;
    }

    objp->orient.fvec = -objp->orient.fvec;
    objp->orient.rvec = -objp->orient.rvec;
    ObjSetOrient(objp, &objp->orient);

    pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;

    if (!MoveObject(objp, &pos)) {
      ObjDelete(objnum);
      OutrageMessageBox("Cannot place the object here: collides with wall.");
      return false;
    }
  }

  // Deal with special stuff for player
  if (obj_type == OBJ_PLAYER) {
    Players[obj_id].start_pos = objp->pos;
    Players[obj_id].start_roomnum = objp->roomnum;
    Players[obj_id].start_orient = objp->orient;
    vm_Orthogonalize(&Players[obj_id].start_orient);
  }

  Cur_object_index = objnum;
  World_changed = true;

  return true;
}

// ============================================================================
// ResetGroundObject — editor/HObject.cpp:430
// Adjusts an object so it's at the ground level.
// ============================================================================
void ResetGroundObject(object *objp) {
  if (!OBJECT_OUTSIDE(objp))
    return;

  poly_model *pm;
  if (!((objp->render_type == RT_POLYOBJ) &&
        ((pm = GetPolymodelPointer(objp->rtype.pobj_info.model_num)) != nullptr) &&
        pm->n_ground))
    return;

  vector surface_norm;
  vector pos = objp->pos;
  pos.y() = GetTerrainGroundPoint(&pos, &surface_norm);

  vector ground_point;
  vector ground_normal;
  vector to_ground;

  PhysCalcGround(&ground_point, &ground_normal, objp, 0);
  to_ground = objp->pos - ground_point;
  float dist = vm_Dot3Product(ground_normal, to_ground);
  pos += dist * surface_norm;

  matrix groundplane_orient, surface_orient, object_orient;

  vm_VectorToMatrix(&groundplane_orient, &pm->ground_slots[0].norm, NULL, NULL);
  vm_VectorToMatrix(&surface_orient, &surface_norm);
  vm_MatrixMulTMatrix(&object_orient, &surface_orient, &groundplane_orient);

  ObjSetPos(objp, &pos, objp->roomnum, &object_orient, false);

  World_changed = true;
}

// ============================================================================
// HObjectMove — editor/HObject.cpp:477
// Moves the specified object by a delta in viewer/object frame.
// ============================================================================
void HObjectMove(int objnum, float dx, float dy, float dz) {
  if (objnum == -1) {
    LOG_INFO("HObjectMove:No current object.\n");
    return;
  }

  object *obj = &Objects[objnum];
  object *ref_obj = (D3EditState.object_move_mode == REL_VIEWER) ? Viewer_object : obj;
  matrix *mat = &ref_obj->orient;

  vector newpos = obj->pos + (mat->rvec * dx) + (mat->uvec * dy) + (mat->fvec * -dz);

  MoveObject(obj, &newpos);
  Object_moved = true;
}

// ============================================================================
// Rotation functions — editor/HObject.cpp:503-513
// ============================================================================
void HObjectIncreaseBank() { RotateObject(Cur_object_index, 0, 0, Object_move_rotation); }
void HObjectDecreaseBank() { RotateObject(Cur_object_index, 0, 0, -Object_move_rotation); }
void HObjectIncreasePitch() { RotateObject(Cur_object_index, Object_move_rotation, 0, 0); }
void HObjectDecreasePitch() { RotateObject(Cur_object_index, -Object_move_rotation, 0, 0); }
void HObjectIncreaseHeading() { RotateObject(Cur_object_index, 0, Object_move_rotation, 0); }
void HObjectDecreaseHeading() { RotateObject(Cur_object_index, 0, -Object_move_rotation, 0); }

// ============================================================================
// HObjectDelete — editor/HObject.cpp:517
// Deletes the currently selected object from the mine.
// ============================================================================
void HObjectDelete() {
  if (Cur_object_index == -1)
    return;

  int objnum = Cur_object_index;

  if (&Objects[objnum] == Player_object) {
    OutrageMessageBox("Can't delete Player object");
    return;
  }

  if (Objects[objnum].type == OBJ_DOOR) {
    if (!OutrageMessageBox(MBOX_YESNO,
                          "It's very, very bad to delete a door object.  Are you sure you want to do this?"))
      return;
  }

  ObjDelete(objnum);
  if (objnum == Cur_object_index)
    Cur_object_index = -1;

  World_changed = true;
}

// ============================================================================
// HObjectSetDefault — editor/HObject.cpp:543
// Sets default (identity) orientation for the current object.
// ============================================================================
void HObjectSetDefault() {
  if (Cur_object_index == -1)
    return;

  ObjSetOrient(&Objects[Cur_object_index], &Identity_matrix);
  World_changed = true;
}

// ============================================================================
// HObjectMoveToViewer — editor/HObject.cpp:554
// Teleports an object to in front of the viewer.
// ============================================================================
void HObjectMoveToViewer(object *objp) {
  ObjSetPos(objp, &Viewer_object->pos, Viewer_object->roomnum, NULL, false);

  vector pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;
  MoveObject(objp, &pos);

  World_changed = true;
}

// ============================================================================
// HObjectFlip — editor/HObject.cpp:628
// Flips the current object by negating its up and right vectors.
// ============================================================================
void HObjectFlip() {
  matrix *m = &Objects[Cur_object_index].orient;

  m->uvec = -m->uvec;
  m->rvec = -m->rvec;

  World_changed = true;
}
