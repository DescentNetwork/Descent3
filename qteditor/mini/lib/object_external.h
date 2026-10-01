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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/object_external.h $
 * $Revision: 37 $
 * $Date: 10/26/99 10:32a $
 * $Author: Jeff $
 *
 * Object defines usable by both the main code & the DLLs
 *
 * $Log: /DescentIII/main/object_external.h $
 *
 * 37    10/26/99 10:32a Jeff
 * no red guidebot in non-Windows versions
 *
 * 36    10/20/99 5:40p Chris
 * Added the Red Guidebot
 *
 * 35    10/12/99 11:06a Jeff
 * added object effect flags for negative lighting and virus infection
 *
 * 34    6/08/99 1:01p Jason
 * changes for bumpmapping
 *
 * 33    4/21/99 3:01p Matt
 * Added a new type for dying objects that have AI, instead of keeping a
 * flag in the dying info.
 *
 * 32    4/20/99 8:14p Chris
 * Added support for object's that hit the ceiling and for making the
 * level always check for the ceiling (inside and outside the mine)
 *
 * 31    4/20/99 5:39p Matt
 * Added macro to check if an object is a robot.  Does the
 * type-is-robot-or-type-is-building-with-AI check.
 *
 * 30    4/18/99 10:55p Chris
 * Added ignore own concussive blasts
 *
 * 29    4/18/99 8:13p Chris
 * Fixed the floating flare problems (where windows where broken out and
 * the flare remained)
 *
 * 28    4/05/99 10:54a Matt
 * Added auto-waypoint system
 *
 * 27    3/31/99 3:59p Chris
 * made the MC_ stuff externalized to OSIRIS.
 *
 * 26    3/28/99 5:56p Matt
 * Added sparking effect for objects
 *
 * 25    3/26/99 3:26p Jeff
 * option to display hud message when cloaking
 *
 * 24    3/11/99 6:31p Jeff
 * numerous fixes to demo system in multiplayer games (when
 * recording/playback a demo in a multiplayer game)
 *
 * 23    2/25/99 11:01a Matt
 * Added new explosion system.
 *
 * 22    2/22/99 11:38p Matt
 * Deleted static debris objects, since they were never used
 *
 * 21    2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 20    2/21/99 4:20p Matt
 * Added SoundSource objects (and reformatted parts of the object header
 * files).
 *
 * 19    2/13/99 12:36a Jeff
 * new object flag.  set for when an object is currently in a player's
 * inventory
 *
 * 18    2/12/99 3:38p Jason
 * added client side interpolation
 *
 * 17    2/11/99 6:25p Chris
 * Added PF_NO_COLLIDE_DOORS
 *
 * 16    2/10/99 2:49p Matt
 * Renamed OBJECT_HANDLE_INVALID to OBJECT_HANDLE_BAD
 *
 * 15    2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 14    2/08/99 5:26p Jeff
 * removed all calls to MultiSendRemoveObject, incorportated into
 * SetObjectDeadFlag.  Fixes sequencing issues in multiplayer
 *
 * 13    2/05/99 1:26p Matt
 * Added a macro to check if a type is one of the generic types
 *
 * 12    1/27/99 6:08p Jason
 * first pass at markers
 *
 * 11    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 10    1/21/99 3:35p Jason
 * added liquid code
 *
 * 9     1/18/99 8:07p Chris
 * Added the no-collide same flag (for flocks and nests)
 *
 * 8     1/18/99 2:46p Matt
 * Combined flags & flags2 fields in object struct
 *
 * 7     1/13/99 3:25a Chris
 * Added Obj_Burning and Obj_IsEffect to OSIRIS
 *
 * 6     1/13/99 2:29a Chris
 * Massive AI, OSIRIS update
 *
 * 5     1/11/99 2:14p Chris
 * Massive work on OSIRIS and AI
 *
 * 4     1/05/99 5:09p Jason
 * added permissable server networking (ala Quake/Unreal) to Descent3
 *
 * 3     1/05/99 4:16p Matt
 * Added SourceSafe header
 *
 */

#ifndef OBJECT_EXTERNAL_H_
#define OBJECT_EXTERNAL_H_

#include <cstddef>
#include <cstdint>

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_BAD 0

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_NONE -1

// Object types (was the OBJ_* set of C macros)
enum class object_type : uint8_t {
  none = 255,       // unused object
  wall = 0,         // A wall... not really an object, but used for collisions
  fireball = 1,     // a fireball, part of an explosion
  robot = 2,        // an evil enemy
  shard = 3,        // a piece of glass
  player = 4,       // the player on the console
  weapon = 5,       // a laser, missile, etc
  viewer = 6,       // a viewed object in the editor
  powerup = 7,      // a powerup you can pick up
  debris = 8,       // a piece of robot
  camera = 9,       // a camera object in the game
  shockwave = 10,   // a shockwave
  clutter = 11,     // misc objects
  ghost = 12,       // what the player turns into when dead
  light = 13,       // a light source, & not much else
  coop = 14,        // a cooperative player object.
  marker = 15,      // a map marker
  building = 16,    // a building
  door = 17,        // a door
  room = 18,        // a room, visible on the terrain
  particle = 19,    // a particle
  splinter = 20,    // a splinter piece from an exploding object
  dummy = 21,       // a dummy object, ignored by everything
  observer = 22,    // an observer in a multiplayer game
  debug_line = 23,  // something for debugging, I guess.  I sure wish people would add comments.
  soundsource = 24, // an object that makes a sound but does nothing else
  waypoint = 25,    // a object that marks a waypoint
};
// NOTE: if you add a type here, you must add the name to Object_type_names[]
inline constexpr size_t MAX_OBJECT_TYPES = 26; // Update this when adding new types

// Index into the per-type arrays (Object_type_names, CollisionResult,
// Num_object_ids, ...).  object_type::none (255) is a sentinel, not a valid
// index, so callers must exclude it before using this.
inline size_t obj_type_index(object_type type) { return static_cast<size_t>(type); }

// Condition to check if the specified type in a generic type
#define IS_GENERIC(type)                                                                                               \
  ((type == object_type::clutter) || (type == object_type::building) || (type == object_type::robot) ||                \
   (type == object_type::powerup))

// Condition to check if the specified object is a robot (checks for buildings with AI)
#define IS_ROBOT(objp) ((objp->type == object_type::robot) || ((objp->type == object_type::building) && objp->ai_info))

// Control types - what tells this object what do do
enum class control_type : uint8_t {
  none = 0,          // doesn't move (or change movement)
  ai = 1,            // driven by AI
  explosion = 2,     // explosion sequencer
  flying = 4,        // the player is flying
  slew = 5,          // slewing
  flythrough = 6,    // the flythrough system
  weapon = 9,        // laser, etc.
  debris = 12,       // this is a piece of debris
  powerup = 13,      // animating powerup blob
  soar = 14,         // Soar object
  particle = 15,     // Particle
  splinter = 16,     // Splinter
  soundsource = 17,  // SoundSource
  dying = 18,        // slowly dying
  dying_and_ai = 19, // dying with AI
};

// Movement types
enum class movement_type : uint8_t {
  none = 0,        // Doesn't move
  physics = 1,     // Moves by physics
  walking = 2,     // Uses physics data structure, but uses a different physics code pipe
  at_rest = 3,     //
  shockwave = 4,   // Moves like a shockwave
                   // (actually this is for space conservation -
                   //  it could be more logically used as a
                   //  control type)
  obj_linked = 5,  // Allows sticky objects to link to polymodel objects
};

// Movement classes
enum class movement_class : uint8_t {
  standing = 0,
  flying = 1,
  rolling = 2,
  walking = 3,
  jumping = 4,
};

// Attach types
enum class attach_type : uint8_t {
  rad = 0,
  aligned = 1,
  unaligned = 2,
};

// Render types
enum class render_type : uint8_t {
  none = 0,          // does not render
  polyobj = 1,       // a polygon model
  fireball = 2,      // a fireball
  weapon = 3,        // a non-polygonal weapon
  line = 4,          // a line
  particle = 5,      // render as particle type
  splinter = 6,      // render as a splinter
  room = 7,          // rendered as a room, not an object
  editor_sphere = 8, // renderd as a sphere in the editor, else not rendered
  shard = 9,         // bits of broken glass
};

// How an object (or an object_info's light) is lit.
enum class lighting_render_type : uint8_t {
  static_lights = 0,
  gouraud = 1,
  lightmaps = 2,
};

// Generic Sound indices
#define GSI_AMBIENT 0
#define GSI_EXPLODE 1

// Static Robot ids
#define ROBOT_GUIDEBOT 0 // NOTE: this must match GENOBJ_GUIDEBOT
#define ROBOT_GUIDEBOTRED 2 // NOTE: this must match GENOBJ_GUIDEBOTRED

#endif
