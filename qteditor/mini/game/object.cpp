// Object subsystem ported from Descent3/object.cpp / Descent3/ObjInit.cpp.
//
// The mini (decoupled editor engine) does not wire in the runtime game
// subsystems (multiplayer, demo, OSIRIS scripts, AI, collision), so those
// branches of the originals are omitted.  Everything the editor relies on is
// kept: the object free list, room/terrain linking, handles, AABB and
// orientation maintenance, placement and deletion, and level-save/load glue.

#include "object.h"
#include "objinit.h"

#include "descent.h"
#include "findintersection.h"
#include "mem/mem.h"
#include "objinfo.h"
#include "polymodel.h"
#include "room.h"
#include "terrain.h"

#include "object_lighting.h"

#include <cstring>

// The mini defines Num_objects in stubs.cpp without a header declaration.
extern int Num_objects;

// Big-object tracking, declared extern in object.h but not defined elsewhere
// in the mini tree.
int Num_big_objects = 0;
int16_t BigObjectList[MAX_BIG_OBJECTS];

// The mini defines the in-memory model table with this many slots
// (see stubs.cpp).  Guards model lookups against the original's
// unguarded Poly_models[model_num] indexing.
static const int MINI_POLY_MODELS = 500;

// ---------------------------------------------------------------------------
// Object globals (mirrors the file-scope globals from the original object.cpp)
// ---------------------------------------------------------------------------

int free_obj_list[MAX_OBJECTS];
int Highest_ever_object_index = -1;

// Position history (declared extern in object.h).  Used for runtime motion
// blur interpolation; the editor only maintains the free-slot bookkeeping.
tPosHistory Object_position_samples[MAX_OBJECT_POS_HISTORY];
uint8_t Object_position_head = 0;
int16_t Object_map_position_history[MAX_OBJECTS];
float Last_position_history_update[MAX_POSITION_HISTORY];

static int Object_map_position_free_slots[MAX_OBJECT_POS_HISTORY];
static int Num_free_object_position_history = 0;

// ---------------------------------------------------------------------------
// Big objects
// ---------------------------------------------------------------------------

void InitBigObjects() { Num_big_objects = 0; }

void BigObjAdd(int objnum) {
  if (Num_big_objects >= MAX_BIG_OBJECTS)
    return;

  Objects[objnum].flags |= OF_BIG_OBJECT;
  BigObjectList[Num_big_objects++] = objnum;
}

void BigObjRemove(int objnum) {
  Objects[objnum].flags &= (~OF_BIG_OBJECT);

  int i = 0;
  for (i = 0; i < Num_big_objects; i++)
    if (BigObjectList[i] == objnum)
      break;

  if (i == Num_big_objects)
    return; // wasn't in the list

  Num_big_objects--;

  while (i < Num_big_objects) {
    BigObjectList[i] = BigObjectList[i + 1];
    i++;
  }
}

// ---------------------------------------------------------------------------
// Free list / list resets
// ---------------------------------------------------------------------------

// Resets the object list: sets all objects to unused, initializes handles,
// & sets roomnums to -1.  Called by the editor to init a new level.
void ResetObjectList() {
  // Init data for each object
  for (int i = 0; i < MAX_OBJECTS; i++) {
    Objects[i].handle = i;
    Objects[i].type = OBJ_NONE;
    Objects[i].roomnum = -1;
  }

  // Build the free object list
  ResetFreeObjects();

  // Say no big objects
  InitBigObjects();

  ObjResetPositionHistory();
}

// Builds the free object list by scanning the list of free objects & adding
// unused ones to the list.  Also sets Highest_object_index.
void ResetFreeObjects() {
  Highest_object_index = -1;

  int i;
  for (i = Num_objects = MAX_OBJECTS; --i >= 0;)
    if (Objects[i].type == OBJ_NONE)
      free_obj_list[--Num_objects] = i;
    else if (Highest_object_index == -1)
      Highest_object_index = i;
}

//-----------------------------------------------------------------------------
//	Scan the object list, freeing down to num_used objects
//	Returns number of slots freed.
int FreeObjectSlots(int num_used) {
  int i, olind;
  int obj_list[MAX_OBJECTS];
  int num_already_free, num_to_free, original_num_to_free;

  olind = 0;
  num_already_free = MAX_OBJECTS - Highest_object_index - 1;

  if (MAX_OBJECTS - num_already_free < num_used)
    return 0;

  for (i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].flags & OF_DEAD) {
      num_already_free++;
      if (MAX_OBJECTS - num_already_free < num_used)
        return num_already_free;
    } else {
      switch (Objects[i].type) {
      case OBJ_NONE:
        num_already_free++;
        if (MAX_OBJECTS - num_already_free < num_used)
          return 0;
        break;
      case OBJ_FIREBALL:
      case OBJ_WEAPON:
      case OBJ_DEBRIS:
      case OBJ_SPLINTER:
        obj_list[olind++] = i;
        break;
      default:
        break;
      }
    }
  }

  num_to_free = MAX_OBJECTS - num_used - num_already_free;
  original_num_to_free = num_to_free;

  if (num_to_free > olind)
    num_to_free = olind;

  for (i = 0; i < num_to_free; i++)
    if (Objects[obj_list[i]].type == OBJ_DEBRIS) {
      SetObjectDeadFlag(Objects[obj_list[i]]);
      num_to_free--;
    }

  for (i = 0; i < num_to_free; i++)
    if (Objects[obj_list[i]].type == OBJ_WEAPON) {
      SetObjectDeadFlag(Objects[obj_list[i]]);
      num_to_free--;
    }

  return original_num_to_free - num_to_free;
}

// returns the number of a free object, updating Highest_object_index.
// Generally, ObjCreate() should be called to get an object, since it
// fills in important fields and does the linking.
// returns -1 if no free objects
int ObjAllocate(void) {
  if (Num_objects >= MAX_OBJECTS - 2)
    FreeObjectSlots(MAX_OBJECTS - 10);

  if (Num_objects >= MAX_OBJECTS)
    return -1;

  int objnum = free_obj_list[Num_objects++];

  if (objnum > Highest_object_index) {
    Highest_object_index = objnum;
    if (Highest_object_index > Highest_ever_object_index)
      Highest_ever_object_index = Highest_object_index;
  }

  return objnum;
}

// Frees up an object.  Generally, ObjDelete() should be called to get rid of
// an object.  This function deallocates the object entry after the object has
// been unlinked.
void ObjFree(int objnum) {
  ObjFreePositionHistory(Objects[objnum]);

  free_obj_list[--Num_objects] = objnum;
  if (Num_objects < 0)
    Num_objects = 0;

  if (objnum == Highest_object_index)
    while (Highest_object_index > 0 && Objects[--Highest_object_index].type == OBJ_NONE)
      ;
}

// ---------------------------------------------------------------------------
// Linking
// ---------------------------------------------------------------------------

// Links the object into the list for its room (or terrain cell).
void ObjLink(int objnum, int roomnum) {
  object *obj = &Objects[objnum];
  if (objnum == -1)
    return;

  if (obj->roomnum != -1 || (obj->flags & OF_BIG_OBJECT))
    return;

  if ((obj->size >= MIN_BIG_OBJ_RAD) && (!ROOMNUM_OUTSIDE(roomnum)))
    BigObjAdd(objnum);

  obj->roomnum = roomnum;

  if (ROOMNUM_OUTSIDE(roomnum)) {
    int cellnum = CELLNUM(roomnum);
    if (cellnum < 0 || cellnum > (TERRAIN_WIDTH + 1) * (TERRAIN_DEPTH + 1))
      return;

    obj->next = Terrain_seg[cellnum].objects;
    Terrain_seg[cellnum].objects = objnum;
  } else {
    if (roomnum < 0 || roomnum > Highest_room_index)
      return;

    obj->next = Rooms[roomnum].objects;
    Rooms[roomnum].objects = objnum;
  }

  obj->prev = -1;

  if (obj->next != -1)
    Objects[obj->next].prev = objnum;
}

void ObjUnlink(int objnum) {
  object *obj = &Objects[objnum];
  if (objnum == -1)
    return;

  // If object is already unlinked, do nothing
  if (obj->roomnum == -1)
    return;

  if (obj->flags & OF_BIG_OBJECT)
    BigObjRemove(objnum);

  if (OBJECT_OUTSIDE(obj)) {
    int cellnum = CELLNUM(obj->roomnum);
    if (cellnum < 0 || cellnum > (TERRAIN_WIDTH + 1) * (TERRAIN_DEPTH + 1))
      return;

    terrain_segment *seg = &Terrain_seg[cellnum];

    if (obj->prev == -1)
      seg->objects = obj->next;
    else
      Objects[obj->prev].next = obj->next;

    if (obj->next != -1)
      Objects[obj->next].prev = obj->prev;
  } else {
    if (obj->roomnum < 0 || obj->roomnum > Highest_room_index)
      return;

    room *rp = &Rooms[obj->roomnum];

    if (obj->prev == -1)
      rp->objects = obj->next;
    else
      Objects[obj->prev].next = obj->next;

    if (obj->next != -1)
      Objects[obj->next].prev = obj->prev;
  }

  // Mark as not linked
  obj->roomnum = -1;
}

// When an object has moved into a new room, this function unlinks it from its
// old room and links it into the new room.
void ObjRelink(int objnum, int newroomnum) {
  if ((objnum < 0) || (objnum > Highest_object_index))
    return;

  ObjUnlink(objnum);
  ObjLink(objnum, newroomnum);
}

// ---------------------------------------------------------------------------
// Position history
// ---------------------------------------------------------------------------

void ObjInitPositionHistory(object& obj) {
  // Not saving positions
  Object_map_position_history[OBJNUM(&obj)] = -1;
}

void ObjFreePositionHistory(object& obj) {
  int objnum = OBJNUM(&obj);

  if (objnum < 0 || objnum >= MAX_OBJECTS)
    return;

  if (Object_map_position_history[objnum] != -1) {
    int slot_to_free = Object_map_position_history[objnum];
    Object_map_position_history[objnum] = -1;

    Object_map_position_free_slots[Num_free_object_position_history] = slot_to_free;
    Num_free_object_position_history++;
  }
}

void ObjResetPositionHistory(void) {
  Num_free_object_position_history = MAX_OBJECT_POS_HISTORY;
  Object_position_head = 0;

  int i;

  for (i = 0; i < MAX_OBJECTS; i++) {
    Object_map_position_history[i] = -1;

    if (i < MAX_OBJECT_POS_HISTORY)
      Object_map_position_free_slots[i] = i;
  }

  for (i = 0; i < MAX_POSITION_HISTORY; i++)
    Last_position_history_update[i] = 0.0f - (0.1f * i);
}

// ---------------------------------------------------------------------------
// AABB
// ---------------------------------------------------------------------------

static vector3 ComputeObjectRadiusFromModel(int model_num, float fallback_size) {
  float size = fallback_size;
  if (model_num >= 0 && model_num < MINI_POLY_MODELS)
    size = Poly_models[model_num].anim_size;
  return vector3{size, size, size};
}

void ObjSetAABB(object& obj) {
  vector3 object_rad;

  if (obj.type == OBJ_ROOM && obj.id >= 0 && obj.id < MAX_ROOMS) {
    obj.min_xyz = Rooms[obj.id].min_xyz;
    obj.max_xyz = Rooms[obj.id].max_xyz;
  } else if (obj.flags & OF_POLYGON_OBJECT && obj.type != OBJ_WEAPON && obj.type != OBJ_DEBRIS &&
             obj.type != OBJ_POWERUP && obj.type != OBJ_PLAYER) {
    vector3 offset_pos;

    object_rad = ComputeObjectRadiusFromModel(obj.rtype.pobj_info().model_num, obj.size);
    offset_pos = obj.pos + obj.anim_sphere_offset;

    obj.min_xyz = offset_pos - object_rad;
    obj.max_xyz = offset_pos + object_rad;
  } else {
    object_rad.x() = obj.size;
    object_rad.y() = obj.size;
    object_rad.z() = obj.size;

    obj.min_xyz = obj.pos - object_rad;
    obj.max_xyz = obj.pos + object_rad;
  }
}

// ---------------------------------------------------------------------------
// Type-specific initialization
// ---------------------------------------------------------------------------

// Sets the render info from an object-info page handle.  In the original this
// is ObjSetRenderPolyobj (polymodel.cpp); the mini inlines the page-driven
// subset the editor needs.
static void ObjSetRenderPolyobj(object& obj, int handle) {
  if (handle == -1) {
    obj.render_type = RT_NONE;
    obj.flags &= ~OF_POLYGON_OBJECT;
    obj.rtype.pobj_info().model_num = -1;
  } else {
    obj.render_type = RT_POLYOBJ;
    obj.flags |= OF_POLYGON_OBJECT;
    obj.rtype.pobj_info().model_num = handle;
  }

  obj.rtype.pobj_info().subobj_flags = 0;
  obj.rtype.pobj_info().anim_time = 0.0f;
  obj.rtype.pobj_info().anim_frame = 0.0f;
  obj.rtype.pobj_info().anim_start_frame = 0.0f;
  obj.rtype.pobj_info().anim_end_frame = 0.0f;
}

// Initializes the type-specific data of an object from its object_info page.
// The original (ObjInitTypeSpecific / ObjInitGeneric, ObjInit.cpp) also wires
// AI, weapons, animation timers and model paging — all runtime systems the
// mini omits.  This is the editor-relevant subset.
static int ObjInitTypeSpecific(object& obj, bool reinitializing) {
  (void)reinitializing;

  if (obj.id < 0 || obj.id >= MAX_OBJECTS)
    return 0;

  object_info *oi = &Object_info[obj.id];

  // Deal with deleted type
  if (oi->type == OBJ_NONE)
    return 0;

  if (oi->type != obj.type)
    obj.type = oi->type;

  switch (obj.type) {
  case OBJ_ROOM:
    obj.render_type = RT_ROOM;
    obj.movement_type = MT_NONE;
    obj.control_type = CT_NONE;
    obj.size = oi->size;
    break;
  case OBJ_VIEWER:
  case OBJ_CAMERA:
  case OBJ_MARKER:
  case OBJ_CLUTTER:
  case OBJ_BUILDING:
  case OBJ_ROBOT:
  case OBJ_POWERUP:
  case OBJ_PLAYER:
  case OBJ_WAYPOINT:
  case OBJ_DOOR:
    ObjSetRenderPolyobj(obj, obj.id);
    obj.size = oi->size;
    obj.shields = static_cast<float>(oi->hit_points);
    if (obj.type == OBJ_VIEWER)
      obj.control_type = CT_SLEW;
    else if (obj.type == OBJ_PLAYER)
      obj.movement_type = MT_PHYSICS;
    else if (obj.type == OBJ_POWERUP)
      obj.movement_type = MT_PHYSICS;
    else if (obj.type == OBJ_CAMERA)
      obj.movement_type = MT_NONE;
    break;
  case OBJ_SOUNDSOURCE:
    obj.render_type = RT_NONE;
    obj.movement_type = MT_NONE;
    obj.control_type = CT_SOUNDSOURCE;
    break;
  case OBJ_WEAPON:
  case OBJ_FIREBALL:
  case OBJ_DEBRIS:
  case OBJ_SHARD:
  case OBJ_SPLINTER:
  case OBJ_SHOCKWAVE:
    // Runtime systems only; keep the object inert in the editor.
    break;
  default:
    return 0;
  }

  return 1;
}

// Initializes a new object.  All fields not passed in are set to defaults.
// Returns 1 if ok, 0 if error
int ObjInit(object& obj, int type, int id, int handle, vector3& pos, float creation_time, int parent_handle) {
  // Zero out the object structure.  The original uses memset() here; the mini
  // object holds a std::string (name) so a value-initialized temporary is used
  // instead — equivalent zeroing without clobbering the string.
  obj = object{};

  // Set the stuff that's passed in
  obj.type = type;
  obj.id = id;
  obj.handle = handle;
  obj.pos = obj.last_pos = pos;
  obj.parent_handle = parent_handle;
  obj.creation_time = creation_time;

  // Initialize some general stuff
  obj.roomnum = -1;
  obj.orient = Identity_matrix;
  obj.next = obj.prev = -1;
  obj.dummy_type = OBJ_NONE;
  obj.flags = 0;
  obj.size = 0;
  obj.change_flags = 0;
  obj.generic_nonvis_flags = 0;
  obj.generic_sent_nonvis = 0;
  obj.custom_default_script_name.clear();
  obj.custom_default_module_name.clear();
  obj.contains_type = -1;
  obj.lifeleft = 0;
  obj.attach_children.clear();

  // Now initialize the type-specific data
  return ObjInitTypeSpecific(obj, false);
}

// Re-copies data to each object from the appropriate page for that object type.
// Called after an object page has changed.
void ObjReInitAll() {
  for (int objnum = 0; objnum <= Highest_object_index; objnum++)
    if (Objects[objnum].type != OBJ_NONE)
      ObjInitTypeSpecific(Objects[objnum], true);
}

// ---------------------------------------------------------------------------
// Create / delete
// ---------------------------------------------------------------------------

// Initializes a new object.  Adds it to the list for the given room.
// Returns the object number, or -1 on failure.
int ObjCreate(uint8_t type, uint16_t id, int roomnum, vector3& pos, const matrix *orient, int parent_handle) {
  if (type == OBJ_NONE)
    return -1;

  if (ROOMNUM_OUTSIDE(roomnum)) {
    int cellnum = CELLNUM(roomnum);
    if (cellnum < 0 || cellnum > TERRAIN_WIDTH * TERRAIN_DEPTH)
      return -1;

    roomnum = GetTerrainRoomFromPos(pos);
    if (roomnum == -1)
      return -1;
  }

  // Get next free object
  int objnum = ObjAllocate();
  if (objnum == -1) // no free objects
    return -1;

  object& obj = Objects[objnum];

  // Make sure the object is ok
  if (obj.type != OBJ_NONE)
    return -1;
  if (obj.roomnum != -1)
    return -1;

  // Compute the new handle
  int handle = obj.handle + HANDLE_COUNT_INCREMENT;

  // Initialize the object
  // The original passes the game clock (Gametime); the mini has no game clock,
  // so creation_time starts at 0.
  if (!ObjInit(obj, type, id, handle, pos, 0.0f, parent_handle)) { // Couldn't init!
    obj.type = OBJ_NONE;                                          // mark as unused
    ObjFree(objnum);                                               // de-allocate object
    return -1;
  }

  // Set the object's orientation
  // THIS MUST BE DONE AFTER ObjInit (as ObjInit loads a polymodel and sets
  // the anim and wall offsets).
  ObjSetOrient(obj, orient ? *orient : Identity_matrix);

  // Link object into room or terrain cell
  ObjLink(objnum, roomnum);

  // Type-specific init set up the size, so now we can compute the bounding box.
  ObjSetAABB(obj);

  ObjInitPositionHistory(obj);

  return objnum;
}

// Removes an object from the world.
void ObjDelete(int objnum) {
  object *obj = &Objects[objnum];
  if (objnum < 0 || objnum >= MAX_OBJECTS)
    return;

  if (obj->type == OBJ_NONE)
    return;

  if (obj->flags & OF_POLYGON_OBJECT) {
    polyobj_info *p_info = &obj->rtype.pobj_info();
    p_info->multi_turret_info.keyframes.clear();
    p_info->multi_turret_info.last_keyframes.clear();
  }

  if (obj == Viewer_object) // deleting the viewer?
    Viewer_object = Player_object;

  // (The original also tears down OSIRIS scripts and un-ghosts OBJ_DUMMY
  // objects here; the mini omits the script runtime.)

  ObjUnlink(objnum);

  obj->custom_default_script_name.clear();
  obj->custom_default_module_name.clear();

  obj->type = OBJ_NONE; // unused!
  obj->roomnum = -1;    // zero it!

  // Free lightmap memory
  if (obj->lm_object.used)
    ClearObjectLightmaps(obj);

  obj->attach_children.clear();

  obj->name.clear();

  // (dynamic_wb/ai_info/effect_info/lighting_info/osiris_script are
  // std::unique_ptr members; the implied destructor of the Objects array
  // releases any memory they still own.)

  ObjFree(objnum);
}

// Frees all the objects that are currently in use.
void FreeAllObjects() {
  for (int objnum = 0; objnum <= Highest_object_index; objnum++)
    if (Objects[objnum].type != OBJ_NONE) {
      Objects[objnum].flags |= OF_SERVER_SAYS_DELETE;
      Objects[objnum].flags &= ~OF_INPLAYERINVENTORY;
      ObjDelete(objnum);
    }
}

// ---------------------------------------------------------------------------
// Orientation / position
// ---------------------------------------------------------------------------

// Sets the orientation of an object.  This should be called to orient an object.
void ObjSetOrient(object& obj, const matrix& orient) {
  // Accounts for if the orientation was set and then this function is being
  // used to update the other stuff
  obj.orient = orient;

  // Recompute the orientation dependent information
  if (obj.flags & OF_POLYGON_OBJECT) {
    if (obj.type != OBJ_WEAPON && obj.type != OBJ_DEBRIS && obj.type != OBJ_POWERUP && obj.type != OBJ_ROOM) {
      int mn = obj.rtype.pobj_info().model_num;
      if (mn >= 0 && mn < MINI_POLY_MODELS) {
        matrix m;

        m = obj.orient;
        vm_TransposeMatrix(&m);

        obj.wall_sphere_offset = Poly_models[mn].wall_size_offset * m;
        obj.anim_sphere_offset = Poly_models[mn].anim_size_offset * m;
      }
    } else {
      obj.wall_sphere_offset = vector3{};
      obj.anim_sphere_offset = vector3{};
    }
  }
}

// Sets the position of an object.  This should be called to move an object.
void ObjSetPos(object& obj, vector3& pos, int roomnum, matrix* orient, bool f_update_attached_children) {
  (void)f_update_attached_children;

  int oldroomnum = obj.roomnum;
  vector3 old_pos = obj.pos;

  // Reset the position & recalculate the AABB
  obj.pos = pos;
  ObjSetAABB(obj);

  // Reset the orientation if changed
  if (orient != nullptr)
    ObjSetOrient(obj, *orient);

  // Clear the outside-mine flag
  obj.flags &= ~OF_OUTSIDE_MINE;

  // If changed rooms, do a bunch of stuff
  if (obj.roomnum != roomnum) {
    // (The original fires an OSIRIS EVT_CHANGESEG event and informs the
    // level goals here; the mini omits the script runtime.)

    // Relink the object
    ObjRelink(OBJNUM(&obj), roomnum);

    // Slowly change volume lighting if going between rooms, if not in the editor
    if ((obj.effect_info != nullptr) && (obj.effect_info->type_flags & EF_VOLUME_LIT)) {
      if (!ROOMNUM_OUTSIDE(oldroomnum) && !ROOMNUM_OUTSIDE(roomnum)) {
        if (!(obj.effect_info->type_flags & EF_VOLUME_CHANGING)) {
          obj.effect_info->type_flags |= EF_VOLUME_CHANGING;
          obj.effect_info->volume_change_time = 1.0f;
          obj.effect_info->volume_old_room = oldroomnum;
          obj.effect_info->volume_old_pos = old_pos;
        }
      } else // either old or new room was outside, so don't do volume changing
        obj.effect_info->type_flags &= ~EF_VOLUME_CHANGING;
    }
  }
}

// ---------------------------------------------------------------------------
// Lookup
// ---------------------------------------------------------------------------

// Returns a pointer to an object given its handle.  Returns NULL if the
// object no longer exists.
object *ObjGet(int handle) {
  if (handle == OBJECT_HANDLE_NONE)
    return nullptr;

  if (handle == OBJECT_HANDLE_BAD)
    return nullptr;

  int objnum = handle & HANDLE_OBJNUM_MASK;
  if (objnum < 0 || objnum >= MAX_OBJECTS)
    return nullptr;

  object *objp = &Objects[objnum];

  if ((objp->type != OBJ_NONE) && (objp->handle == handle))
    return objp;

  return nullptr;
}

// Returns a vertex of an object in WORLD coordinates.
void GetObjectPointInWorld(vector3& dest, object& obj, int subnum, int vertnum) {
  int mn_i = obj.rtype.pobj_info().model_num;
  if (mn_i < 0 || mn_i >= MINI_POLY_MODELS)
    return;

  poly_model *pm = &Poly_models[mn_i];
  bsp_info *sm = &pm->submodel[subnum];
  float normalized_time[MAX_SUBOBJECTS];

  if (!pm->new_style)
    return;

  for (int i = 0; i < MAX_SUBOBJECTS; i++)
    normalized_time[i] = 0.0f;

  SetModelAnglesAndPos(pm, normalized_time);

  vector3 pnt = sm->verts[vertnum];
  int mn = subnum;
  matrix m;

  // Instance up the tree for this gun
  while (mn != -1) {
    vector3 tpnt;

    vm_AnglesToMatrix(&m, pm->submodel[mn].angs.p(), pm->submodel[mn].angs.h(), pm->submodel[mn].angs.b());
    vm_TransposeMatrix(&m);

    tpnt = pnt * m;

    pnt = tpnt + pm->submodel[mn].offset + pm->submodel[mn].mod_pos;

    mn = pm->submodel[mn].parent;
  }

  // Now instance for the entire object
  m = obj.orient;
  vm_TransposeMatrix(&m);

  dest = pnt * m;
  dest += obj.pos;
}

// ---------------------------------------------------------------------------
// Death flag
// ---------------------------------------------------------------------------

void SetObjectDeadFlag(object& obj, bool tell_clients_to_remove, bool play_sound_on_clients) {
  int objnum = OBJNUM(&obj);
  if (objnum == -1 || objnum == 0)
    return;
  if (obj.type == OBJ_NONE)
    return;

  obj.flags |= OF_DEAD;

  if (tell_clients_to_remove) {
    if (play_sound_on_clients)
      obj.flags |= OF_SEND_MULTI_REMOVE_ON_DEATHWS;
    else
      obj.flags |= OF_SEND_MULTI_REMOVE_ON_DEATH;
  }
}
