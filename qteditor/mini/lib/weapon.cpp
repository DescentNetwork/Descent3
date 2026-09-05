#include "weapon.h"
#include "physics.h"
#include "doorway.h"
#include "chrono_timer.h"
#include "object_external_struct.h"

static bool AreObjectsAttached(const object *obj1, const object *obj2) {
  const bool f_o1_a = (obj1->flags & OF_ATTACHED) != 0;
  const bool f_o2_a = (obj2->flags & OF_ATTACHED) != 0;

  if (f_o1_a || f_o2_a) {
    const int o1_uh = obj1->attach_ultimate_handle;
    const int o2_uh = obj2->attach_ultimate_handle;

    if ((f_o1_a) && ((o1_uh == obj2->handle) || (f_o2_a && (o1_uh == o2_uh))))
      return true;

    if ((f_o2_a) && (o2_uh == obj1->handle))
      return true;
  }

  return false;
}


bool ObjectsAreRelated(int o1, int o2) {
  if ((o1 < 0) || (o2 < 0))
    return false;

  const object *obj1 = &Objects[o1];
  const object *obj2 = &Objects[o2];

  Q_ASSERT(obj1->handle != OBJECT_HANDLE_NONE);
  Q_ASSERT(obj2->handle != OBJECT_HANDLE_NONE);

  if (obj1->movement_type == MT_OBJ_LINKED || obj2->movement_type == MT_OBJ_LINKED)
    return true;

  if (obj1->type != OBJ_SHOCKWAVE && (obj1->mtype.phys_info.flags & PF_NO_COLLIDE)) {
    return true;
  }

  if (obj2->type != OBJ_SHOCKWAVE && (obj2->mtype.phys_info.flags & PF_NO_COLLIDE)) {
    return true;
  }

  if (((obj1->type == OBJ_PLAYER) && ((obj2->type == OBJ_ROBOT) && (obj2->id == GENOBJ_CHAFFCHUNK))) ||
      ((obj2->type == OBJ_PLAYER) && ((obj1->type == OBJ_ROBOT) && (obj1->id == GENOBJ_CHAFFCHUNK))))
    return true;

  if (((obj1->type == OBJ_BUILDING) && (obj1->movement_type != MT_NONE) && (obj2->type == OBJ_POWERUP)) ||
      ((obj2->type == OBJ_BUILDING) && (obj2->movement_type != MT_NONE) && (obj1->type == OBJ_POWERUP))) {
    return true;
  }

  if (obj1->type == OBJ_DOOR && DoorwayGetPosition(&Rooms[obj1->roomnum]) == 1.0f && obj2->type == OBJ_ROBOT)
    return true;

  if (obj2->type == OBJ_DOOR && DoorwayGetPosition(&Rooms[obj2->roomnum]) == 1.0f && obj1->type == OBJ_ROBOT)
    return true;

  if (AreObjectsAttached(obj1, obj2))
    return true;

  if (obj1->type != OBJ_WEAPON && obj2->type != OBJ_WEAPON) {
    if (((d3::chrono::last_update() < obj1->creation_time + 3.0f) && obj1->parent_handle == obj2->handle) ||
        ((d3::chrono::last_update() < obj2->creation_time + 3.0f) && obj2->parent_handle == obj1->handle))
      return true;
    else
      return false;
  }

  if (obj1->type == OBJ_WEAPON && obj1->movement_type == MT_PHYSICS && (obj1->mtype.phys_info.flags & PF_PERSISTENT) &&
      obj1->ctype.laser_info.last_hit_handle == obj2->handle)
    return true;

  if (obj2->type == OBJ_WEAPON && obj2->movement_type == MT_PHYSICS && (obj2->mtype.phys_info.flags & PF_PERSISTENT) &&
      obj2->ctype.laser_info.last_hit_handle == obj1->handle)
    return true;

  // See if o2 is the parent of o1
  if (obj1->type == OBJ_WEAPON && (obj1->mtype.phys_info.flags & PF_NO_COLLIDE_PARENT)) {
    if (obj1->parent_handle == obj2->handle)
      return true;

    object *t1 = ObjGet(obj1->parent_handle);

    if (t1) {
      if (AreObjectsAttached(obj2, t1))
        return true;
    }
  }

  // See if o1 is the parent of o2
  if (obj2->type == OBJ_WEAPON && (obj2->mtype.phys_info.flags & PF_NO_COLLIDE_PARENT)) {
    if (obj2->parent_handle == obj1->handle)
      return true;

    object *t2 = ObjGet(obj2->parent_handle);

    if (t2) {
      if (AreObjectsAttached(obj1, t2))
        return true;
    }
  }

  // They must both be weapons
  if (obj1->type != OBJ_WEAPON || obj2->type != OBJ_WEAPON) {
    return false;
  }

  //	Here is the 09/07/94 change -- Siblings must be identical, others can hurt each other
  // See if they're siblings...
  if (obj1->parent_handle == obj2->parent_handle) {
    if ((obj1->mtype.phys_info.flags & PF_HITS_SIBLINGS) || (obj2->mtype.phys_info.flags & PF_HITS_SIBLINGS)) {
      return false; // if either is proximity, then can blow up, so say not related
    } else {
      return true;
    }
  }

  // Otherwise, it is two weapons and by default, they should not collide
  return true;
}
