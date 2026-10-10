#include "weapon.h"
#include "physics.h"
#include "doorway.h"
#include "chrono_timer.h"
#include "object_external_struct.h"

static bool AreObjectsAttached(const object *obj1, const object *obj2) {
  const bool f_o1_a = obj1->flags.attached;
  const bool f_o2_a = obj2->flags.attached;

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

  if (obj1->movement_type == movement_type_e::obj_linked || obj2->movement_type == movement_type_e::obj_linked)
    return true;

  if (obj1->type != object_type_e::shockwave && obj1->mtype.phys_info.flags.no_collide) {
    return true;
  }

  if (obj2->type != object_type_e::shockwave && obj2->mtype.phys_info.flags.no_collide) {
    return true;
  }

  if (((obj1->type == object_type_e::player) && ((obj2->type == object_type_e::robot) && (obj2->id == GENOBJ_CHAFFCHUNK))) ||
      ((obj2->type == object_type_e::player) && ((obj1->type == object_type_e::robot) && (obj1->id == GENOBJ_CHAFFCHUNK))))
    return true;

  if (((obj1->type == object_type_e::building) && (obj1->movement_type != movement_type_e::none) && (obj2->type == object_type_e::powerup)) ||
      ((obj2->type == object_type_e::building) && (obj2->movement_type != movement_type_e::none) && (obj1->type == object_type_e::powerup))) {
    return true;
  }

  if (obj1->type == object_type_e::door && DoorwayPositionForRoom(from_roomnum(obj1->roomnum)) == 1.0f && obj2->type == object_type_e::robot)
    return true;

  if (obj2->type == object_type_e::door && DoorwayPositionForRoom(from_roomnum(obj2->roomnum)) == 1.0f && obj1->type == object_type_e::robot)
    return true;

  if (AreObjectsAttached(obj1, obj2))
    return true;

  if (obj1->type != object_type_e::weapon && obj2->type != object_type_e::weapon) {
    if (((d3::chrono::last_update() < obj1->creation_time + 3.0f) && obj1->parent_handle == obj2->handle) ||
        ((d3::chrono::last_update() < obj2->creation_time + 3.0f) && obj2->parent_handle == obj1->handle))
      return true;
    else
      return false;
  }

  if (obj1->type == object_type_e::weapon && obj1->movement_type == movement_type_e::physics && obj1->mtype.phys_info.flags.persistent &&
      obj1->ctype.laser_info().last_hit_handle == obj2->handle)
    return true;

  if (obj2->type == object_type_e::weapon && obj2->movement_type == movement_type_e::physics && obj2->mtype.phys_info.flags.persistent &&
      obj2->ctype.laser_info().last_hit_handle == obj1->handle)
    return true;

  // See if o2 is the parent of o1
  if (obj1->type == object_type_e::weapon && obj1->mtype.phys_info.flags.no_collide_parent) {
    if (obj1->parent_handle == obj2->handle)
      return true;

    object *t1 = ObjGet(obj1->parent_handle);

    if (t1) {
      if (AreObjectsAttached(obj2, t1))
        return true;
    }
  }

  // See if o1 is the parent of o2
  if (obj2->type == object_type_e::weapon && obj2->mtype.phys_info.flags.no_collide_parent) {
    if (obj2->parent_handle == obj1->handle)
      return true;

    object *t2 = ObjGet(obj2->parent_handle);

    if (t2) {
      if (AreObjectsAttached(obj1, t2))
        return true;
    }
  }

  // They must both be weapons
  if (obj1->type != object_type_e::weapon || obj2->type != object_type_e::weapon) {
    return false;
  }

  //	Here is the 09/07/94 change -- Siblings must be identical, others can hurt each other
  // See if they're siblings...
  if (obj1->parent_handle == obj2->parent_handle) {
    if (obj1->mtype.phys_info.flags.hits_siblings || obj2->mtype.phys_info.flags.hits_siblings) {
      return false; // if either is proximity, then can blow up, so say not related
    } else {
      return true;
    }
  }

  // Otherwise, it is two weapons and by default, they should not collide
  return true;
}

// ============================================================================
// Weapon slot management (ported from the engine's weapon.cpp).
// ============================================================================

// Allocs a weapon for use, returns -1 if error, else index on success
index_t AllocWeapon() {
  const size_t n = Weapons.next_slot();
  Q_ASSERT(Weapons.is_unused(n));

  Weapons[n] = weapon{};

  Weapons.acquire(n);
  return static_cast<uint32_t>(n);
}

// Frees weapon index n and all associated images
void FreeWeapon(uint32_t n) {
  Q_ASSERT(Weapons.is_used(n));

  Weapons[n] = weapon{};
  Weapons.release(n);
}

// Gets next weapon from n that has actually been alloced
index_t GetNextWeapon(uint32_t n) {
  return Weapons.next(n);
}

// Gets previous weapon from n that has actually been alloced
index_t GetPrevWeapon(uint32_t n) {
  return Weapons.prev(n);
}
