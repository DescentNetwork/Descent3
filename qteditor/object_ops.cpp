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

#include <QMessageBox>
#include "logger/log.h"
#include "d3edit.h"
#include "findintersection.h"

#include "object.h"
#include "objinfo.h"
#include "physics.h"
#include "player.h"
#include "polymodel.h"


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
bool MoveObject(object& obj, vector3& newpos) {
  fvi_query fq;
  fvi_info hit_info;

  bool use_radius = (obj.movement_type == movement_type::physics);

  fq.p0 = &obj.pos;
  fq.startroom = obj.roomnum ? static_cast<int>(*obj.roomnum) : -1;
  fq.p1 = &newpos;
  fq.thisobjnum = OBJNUM(&obj);
  fq.ignore_obj_list = NULL;
  fq.flags = fvi_query_flags_t{};
  fq.flags.ignore_render_through_portals = true;
  fq.rad = use_radius ? obj.size : 0.0f;

  if (f_allow_objects_to_be_pushed_through_walls)
    fq.flags.ignore_walls = true;
    fq.flags.ignore_terrain = true;
    fq.flags.ignore_external_rooms = true;

  int fate = fvi_FindIntersection(&fq, &hit_info);

  if (fate == HIT_WALL)
    if (vm_VectorDistance(&obj.pos, &hit_info.hit_pnt) < MOVE_EPSILON)
      return false;

  ObjSetPos(obj, hit_info.hit_pnt, to_roomnum(hit_info.hit_room), std::nullopt, false);
  return true;
}

// ============================================================================
// RotateObject (internal) — editor/HObject.cpp:612
// Applies a rotation to the specified object.
// ============================================================================
bool RotateObject(int objnum, angle p, angle h, angle b) {
  object& obj = Objects[objnum];
  matrix rotmat;

  vm_AnglesToMatrix(&rotmat, p, h, b);
  obj.orient *= rotmat;

  vm_Orthogonalize(&obj.orient);
  ObjSetOrient(obj, obj.orient);

  app.Object_moved = true;
  return true;
}

// ============================================================================
// objectPageCurrentId / setObjectPageCurrentId — editor/ObjectDialog.cpp:350
// The object keypad keeps one current object id per page and dispatches on the
// selected page's type, rather than a single shared id. That way switching to
// another page and back restores the previous selection for each page.
// ============================================================================
std::optional<uint16_t> objectPageCurrentId(object_type page) {
  switch (page) {
  case object_type::robot:
    return app.current_robot;
  case object_type::powerup:
    return app.current_powerup;
  case object_type::clutter:
    return app.current_clutter;
  case object_type::building:
    return app.current_building;
  default:
    // Not an object keypad page with a per-page id (players derive theirs from
    // GetFreePlayerIndex(), mirroring the current_player case in Win32).
    return std::nullopt;
  }
}

void setObjectPageCurrentId(object_type page, std::optional<uint16_t> id) {
  switch (page) {
  case object_type::robot:
    app.current_robot = id;
    break;
  case object_type::powerup:
    app.current_powerup = id;
    break;
  case object_type::clutter:
    app.current_clutter = id;
    break;
  case object_type::building:
    app.current_building = id;
    break;
  default:
    Q_ASSERT(false);
    break;
  }
}

// Returns the first allocated object id of the given page's type, replacing any
// stale id the page was holding. Mirrors the self-healing tail of Win32's
// GetCurrentIndex() (editor/ObjectDialog.cpp:395-403).
static std::optional<uint16_t> firstObjectIdForPage(object_type page) {
  for (uint16_t i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == page)
      return i;
  }
  return std::nullopt;
}

// Returns the object id to place for the currently selected keypad page, healing
// the remembered id if it no longer belongs to that page (which happens when a
// database reload frees the id, or when settings are restored under a different
// page). Win32 does the equivalent check in GetCurrentIndex().
std::optional<uint16_t> currentObjectPageId() {
  if (!app.obj_page)
    return std::nullopt;

  std::optional<uint16_t> id = objectPageCurrentId(*app.obj_page);
  if (id && *id < MAX_OBJECT_IDS && Object_info[*id].type == *app.obj_page)
    return id;

  id = firstObjectIdForPage(*app.obj_page);
  setObjectPageCurrentId(*app.obj_page, id);
  return id;
}

// ============================================================================
// HObjectPlace — editor/HObject.cpp:280
// Places a new object of the given type and ID into the world at the viewer's
// location, then repositions it onto the current surface.
// ============================================================================
bool HObjectPlace(object_type obj_type, uint16_t obj_id) {
  int objnum;
  poly_model *pm;
  matrix orient = IDENTITY_MATRIX;

  // Special stuff for player ship
  if (obj_type == object_type::player) {
    if (Ships.empty()) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot place a player: There are no player ships.");
      return false;
    }

    if (!app.current_ship) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must have a current player ship selected for this operation.");
      return false;
    }

    Players[obj_id].ship_index = *app.current_ship;
  }

  if (obj_type != object_type::powerup) {
    orient = Viewer_object->orient;
  }

  objnum = ObjCreate(obj_type, obj_id, Viewer_object->roomnum, Viewer_object->pos, &orient).value_or(-1);
  if (objnum == -1)
    return false;

  object& obj = Objects[objnum];

  // If we have a ground plane, use current cell or face for position
  if ((obj.render_type == render_type::polyobj) &&
      ((pm = GetPolymodelPointer(obj.rtype.pobj_info().model_num)) != nullptr) &&
      pm->n_ground) {
    vector3 *surface_norm;
    vector3 pos;
    int roomnum;

    if (app.view_mode == state::viewer::terrain) {
      int cellnum = GetSelectedTerrainCell();
      if (cellnum == -1) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must have a terrain cell selected to place an object.");
        ObjDelete(objnum);
        return false;
      }
      if (cellnum == -2) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must have only one cell selected to place an object.");
        ObjDelete(objnum);
        return false;
      }

      ComputeTerrainSegmentCenter(pos, cellnum);
      surface_norm = &TerrainNormals[MAX_TERRAIN_LOD - 1][cellnum].normal1;
      roomnum = MAKE_ROOMNUM(cellnum);
    } else {
      if (!app.current.room) {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "You must have a current room selected for this operation.");
        ObjDelete(objnum);
        return false;
      }
      ComputeCenterPointOnFace(&pos, static_cast<int>(*app.current.room), index_to_int(app.current.face));
      surface_norm = &Rooms[*app.current.room].faces[*app.current.face].normal;
      roomnum = index_to_int(app.current.room);

      if (Rooms[roomnum].flags.external)
        roomnum = GetTerrainRoomFromPos(pos).value_or(-1);
    }

    matrix groundplane_orient, surface_orient, object_orient;

    vector3 ground_point;
    vector3 ground_normal;
    vector3 to_ground;

    PhysCalcGround(ground_point, ground_normal, obj, 0);
    to_ground = obj.pos - ground_point;
    float dist = vm_Dot3Product(ground_normal, to_ground);
    pos += dist * (*surface_norm);

    vm_VectorToMatrix(groundplane_orient, pm->ground_slots[0].norm, std::nullopt, std::nullopt);
    vm_VectorToMatrix(surface_orient, *surface_norm);
    vm_MatrixMulTMatrix(&object_orient, &surface_orient, &groundplane_orient);

    ObjSetPos(obj, pos, to_roomnum(roomnum), object_orient, false);
  } else {
    // No ground plane — move in front of viewer, facing viewer
    vector3 pos;

    if (Viewer_object->flags.outside_mine) {
      ObjDelete(objnum);
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot place the object here: the viewer is outside the mine.");
      return false;
    }

    obj.orient.fvec = -obj.orient.fvec;
    obj.orient.rvec = -obj.orient.rvec;
    ObjSetOrient(obj, obj.orient);

    pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;

    if (!MoveObject(obj, pos)) {
      ObjDelete(objnum);
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot place the object here: collides with wall.");
      return false;
    }
  }

  // Deal with special stuff for player
  if (obj_type == object_type::player) {
    Players[obj_id].start_pos = obj.pos;
    Players[obj_id].start_roomnum = obj.roomnum ? static_cast<int32_t>(*obj.roomnum) : -1;
    Players[obj_id].start_orient = obj.orient;
    vm_Orthogonalize(&Players[obj_id].start_orient);
  }

  app.Cur_object_index = objnum;
  app.World_changed = true;

  return true;
}

// ============================================================================
// ResetGroundObject — editor/HObject.cpp:430
// Adjusts an object so it's at the ground level.
// ============================================================================
void ResetGroundObject(object& obj) {
  if (!OBJECT_OUTSIDE(&obj))
    return;

  poly_model *pm;
  if (!((obj.render_type == render_type::polyobj) &&
        ((pm = GetPolymodelPointer(obj.rtype.pobj_info().model_num)) != nullptr) &&
        pm->n_ground))
    return;

  vector3 surface_norm;
  vector3 pos = obj.pos;
  pos.y() = GetTerrainGroundPoint(pos, surface_norm);

  vector3 ground_point;
  vector3 ground_normal;
  vector3 to_ground;

  PhysCalcGround(ground_point, ground_normal, obj, 0);
  to_ground = obj.pos - ground_point;
  float dist = vm_Dot3Product(ground_normal, to_ground);
  pos += dist * surface_norm;

  matrix groundplane_orient, surface_orient, object_orient;

  vm_VectorToMatrix(groundplane_orient, pm->ground_slots[0].norm, std::nullopt, std::nullopt);
  vm_VectorToMatrix(surface_orient, surface_norm);
  vm_MatrixMulTMatrix(&object_orient, &surface_orient, &groundplane_orient);

  ObjSetPos(obj, pos, obj.roomnum, object_orient, false);

  app.World_changed = true;
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

  object& obj = Objects[objnum];
  matrix& mat = (app.object_move_mode == REL_VIEWER) ? Viewer_object->orient : obj.orient;

  vector3 newpos = obj.pos + (mat.rvec * dx) + (mat.uvec * dy) + (mat.fvec * -dz);

  MoveObject(obj, newpos);
  app.Object_moved = true;
}

// ============================================================================
// Rotation functions — editor/HObject.cpp:503-513
// ============================================================================
void HObjectIncreaseBank() { RotateObject(index_to_int(app.Cur_object_index), 0, 0, Object_move_rotation); }
void HObjectDecreaseBank() { RotateObject(index_to_int(app.Cur_object_index), 0, 0, -Object_move_rotation); }
void HObjectIncreasePitch() { RotateObject(index_to_int(app.Cur_object_index), Object_move_rotation, 0, 0); }
void HObjectDecreasePitch() { RotateObject(index_to_int(app.Cur_object_index), -Object_move_rotation, 0, 0); }
void HObjectIncreaseHeading() { RotateObject(index_to_int(app.Cur_object_index), 0, Object_move_rotation, 0); }
void HObjectDecreaseHeading() { RotateObject(index_to_int(app.Cur_object_index), 0, -Object_move_rotation, 0); }

// ============================================================================
// HObjectDelete — editor/HObject.cpp:517
// Deletes the currently selected object from the mine.
// ============================================================================
void HObjectDelete() {
  if (!app.Cur_object_index)
    return;

  int objnum = *app.Cur_object_index;

  if (&Objects[objnum] == Player_object) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Can't delete Player object");
    return;
  }

  if (Objects[objnum].type == object_type::door) {
    if (QMessageBox::question(nullptr, "Are you sure?", "It's very, very bad to delete a door object.  Are you sure you want to do this?") == QMessageBox::No)
      return;
  }

  ObjDelete(objnum);
  if (objnum == index_to_int(app.Cur_object_index))
    app.Cur_object_index.reset();

  app.World_changed = true;
}

// ============================================================================
// HObjectSetDefault — editor/HObject.cpp:543
// Sets default (identity) orientation for the current object.
// ============================================================================
void HObjectSetDefault() {
  if (!app.Cur_object_index)
    return;

  ObjSetOrient(Objects[*app.Cur_object_index], Identity_matrix);
  app.World_changed = true;
}

// ============================================================================
// HObjectMoveToViewer — editor/HObject.cpp:554
// Teleports an object to in front of the viewer.
// ============================================================================
void HObjectMoveToViewer(object& objp) {
  ObjSetPos(objp, Viewer_object->pos, Viewer_object->roomnum, std::nullopt, false);

  vector3 pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;
  MoveObject(objp, pos);

  app.World_changed = true;
}

// ============================================================================
// HObjectFlip — editor/HObject.cpp:628
// Flips the current object by negating its up and right vectors.
// ============================================================================
void HObjectFlip() {
  matrix *m = &Objects[*app.Cur_object_index].orient;

  m->uvec = -m->uvec;
  m->rvec = -m->rvec;

  app.World_changed = true;
}
