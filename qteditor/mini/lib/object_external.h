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

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_BAD 0

// Use this handle when you want a handle that will never be a valid object
#define OBJECT_HANDLE_NONE -1

// Object types
#define OBJ_NONE 255        // unused object
#define OBJ_WALL 0          // A wall... not really an object, but used for collisions
#define OBJ_FIREBALL 1      // a fireball, part of an explosion
#define OBJ_ROBOT 2         // an evil enemy
#define OBJ_SHARD 3         // a piece of glass
#define OBJ_PLAYER 4        // the player on the console
#define OBJ_WEAPON 5        // a laser, missile, etc
#define OBJ_VIEWER 6        // a viewed object in the editor
#define OBJ_POWERUP 7       // a powerup you can pick up
#define OBJ_DEBRIS 8        // a piece of robot
#define OBJ_CAMERA 9        // a camera object in the game
#define OBJ_SHOCKWAVE 10    // a shockwave
#define OBJ_CLUTTER 11      // misc objects
#define OBJ_GHOST 12        // what the player turns into when dead
#define OBJ_LIGHT 13        // a light source, & not much else
#define OBJ_COOP 14         // a cooperative player object.
#define OBJ_MARKER 15       // a map marker
#define OBJ_BUILDING 16     // a building
#define OBJ_DOOR 17         // a door
#define OBJ_ROOM 18         // a room, visible on the terrain
#define OBJ_PARTICLE 19     // a particle
#define OBJ_SPLINTER 20     // a splinter piece from an exploding object
#define OBJ_DUMMY 21        // a dummy object, ignored by everything
#define OBJ_OBSERVER 22     // an observer in a multiplayer game
#define OBJ_DEBUG_LINE 23   // something for debugging, I guess.  I sure wish people would add comments.
#define OBJ_SOUNDSOURCE 24  // an object that makes a sound but does nothing else
#define OBJ_WAYPOINT 25     // a object that marks a waypoint
#define MAX_OBJECT_TYPES 26 // Update this when adding new types
// NOTE: if you add a type here, you must add the name to Object_type_names[]

// Condition to check if the specified type in a generic type
#define IS_GENERIC(type)                                                                                               \
  ((type == OBJ_CLUTTER) || (type == OBJ_BUILDING) || (type == OBJ_ROBOT) || (type == OBJ_POWERUP))

// Condition to check if the specified object is a robot (checks for buildings with AI)
#define IS_ROBOT(objp) ((objp->type == OBJ_ROBOT) || ((objp->type == OBJ_BUILDING) && objp->ai_info))

// Control types - what tells this object what do do
#define CT_NONE 0          // doesn't move (or change movement)
#define CT_AI 1            // driven by AI
#define CT_EXPLOSION 2     // explosion sequencer
#define CT_FLYING 4        // the player is flying
#define CT_SLEW 5          // slewing
#define CT_FLYTHROUGH 6    // the flythrough system
#define CT_WEAPON 9        // laser, etc.
#define CT_DEBRIS 12       // this is a piece of debris
#define CT_POWERUP 13      // animating powerup blob
#define CT_SOAR 14         // Soar object
#define CT_PARTICLE 15     // Particle
#define CT_SPLINTER 16     // Splinter
#define CT_SOUNDSOURCE 17  // SoundSource
#define CT_DYING 18        // slowly dying
#define CT_DYING_AND_AI 19 // dying with AI

// Movement types
#define MT_NONE 0    // Doesn't move
#define MT_PHYSICS 1 // Moves by physics
#define MT_WALKING 2 // Uses physics data structure, but uses a different physics code pipe
#define MT_AT_REST 3
#define MT_SHOCKWAVE                                                                                                   \
  4                     // Moves like a shockwave
                        // (actually this is for space conservation -
                        //  it could be more logically used as a
                        //  control type)
#define MT_OBJ_LINKED 5 // Allows sticky objects to link to polymodel objects

// Movement classes
#define MC_STANDING 0
#define MC_FLYING 1
#define MC_ROLLING 2
#define MC_WALKING 3
#define MC_JUMPING 4

// Attach types
#define AT_RAD 0
#define AT_ALIGNED 1
#define AT_UNALIGNED 2

// Render types
#define RT_NONE 0          // does not render
#define RT_POLYOBJ 1       // a polygon model
#define RT_FIREBALL 2      // a fireball
#define RT_WEAPON 3        // a non-polygonal weapon
#define RT_LINE 4          // a line
#define RT_PARTICLE 5      // render as particle type
#define RT_SPLINTER 6      // render as a splinter
#define RT_ROOM 7          // rendered as a room, not an object
#define RT_EDITOR_SPHERE 8 // renderd as a sphere in the editor, else not rendered
#define RT_SHARD 9         // bits of broken glass


// Generic Sound indices
#define GSI_AMBIENT 0
#define GSI_EXPLODE 1

// Static Robot ids
#define ROBOT_GUIDEBOT 0 // NOTE: this must match GENOBJ_GUIDEBOT
#define ROBOT_GUIDEBOTRED 2 // NOTE: this must match GENOBJ_GUIDEBOTRED

#endif
