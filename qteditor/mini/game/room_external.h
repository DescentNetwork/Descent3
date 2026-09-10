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

 * $Logfile: /DescentIII/main/room_external.h $
 * $Revision: 21 $
 * $Date: 10/21/99 1:32p $
 * $Author: Jeff $
 *
 * Defines and structs for room definitions (for DLL export)
 *
 * $Log: /DescentIII/main/room_external.h $
 *
 * 21    10/21/99 1:32p Jeff
 * added checkbox to prevent certain rooms from lighting
 *
 * 20    4/28/99 1:39p Chris
 * Added the ability to block portals
 *
 * 19    4/26/99 11:11a Chris
 * Updated Bnode system
 *
 * 18    4/18/99 5:39a Chris
 * Vastly improved the path node system
 *
 * 17    4/15/99 5:49p Chris
 * Fixed a bug with rendering the BOAPath nodes
 *
 * 16    4/15/99 12:20p Jason
 * made mirrors more robust and able to have multiple mirrored faces in
 * the same room (as long as they are all on the same plane)
 *
 * 15    4/14/99 3:13p Chris
 * Beginning to add BoaNode stuff
 *
 * 14    4/14/99 11:40a Jason
 * added secret flag
 *
 * 13    4/05/99 10:54a Matt
 * Added auto-waypoint system
 *
 * 12    3/31/99 5:14p Matt
 * Added shell flag for faces
 *
 * 11    3/11/99 1:08p Jason
 * more fixes for smooth specular lighting
 *
 * 10    3/01/99 8:10p Matt
 * Added flag for manually-set path points
 *
 * 9     2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 8     2/19/99 4:26p Jason
 * more work on Katmai support
 *
 * 7     2/09/99 9:59a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 6     2/03/99 5:49p Matt
 * Added room damage system
 *
 * 5     2/03/99 4:26p Jason
 * made multiplayer coop actually work!
 *
 * 4     2/01/99 4:17p Jason
 * more changes for multisafe
 *
 * 3     1/29/99 12:48p Matt
 * Rewrote the doorway system
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef _ROOM_EXTERNAL_H_
#define _ROOM_EXTERNAL_H_

#include "pstypes.h"
#include "vecmat_external.h"
#include "bnode.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <posix_stream.h>

#define MAX_FACES_PER_ROOM 3000  // max number of faces per room
#define MAX_VERTS_PER_ROOM 10000 // max vertices per room
#define MAX_VERTS_PER_FACE 64    // max vertices per face

// Face flags
struct [[gnu::packed]] face_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint16_t scorched : 1;          // This face has one or more scorch marks
  uint16_t not_facing : 1;        // This face is not facing us this frame (Valid only during render)
  uint16_t goalface : 1;          // This face is a goal texture face
  uint16_t touched : 1;           // This face has been touched by fvi_QuickDistFaceList
  uint16_t not_shell : 1;         // This face is not part of the room shell
  uint16_t visible : 1;           // This face is visible this frame (Valid only during render)
  uint16_t triangulated : 1;      // ??
  uint16_t volumetric : 1;        // This face is a volumetric face
  uint16_t destroyed : 1;         // This face has been blown up
  uint16_t floating_trig : 1;     // This face only exists as a floating trigger
  uint16_t spec_invisible : 1;    // This face needs to be not rendered during specularity pass
  uint16_t has_trigger : 1;       // This face has a trigger
  uint16_t texture_changed : 1;   // The texture on this face has changed
  uint16_t corona : 1;            // This face has a lighting corona
  uint16_t vertex_alpha : 1;      // This face has vertex alpha blending
  uint16_t lightmap : 1;          // Render this face with a lightmap on top
#else
  uint16_t lightmap : 1;          // Render this face with a lightmap on top
  uint16_t vertex_alpha : 1;      // This face has vertex alpha blending
  uint16_t corona : 1;            // This face has a lighting corona
  uint16_t texture_changed : 1;   // The texture on this face has changed
  uint16_t has_trigger : 1;       // This face has a trigger
  uint16_t spec_invisible : 1;    // This face needs to be not rendered during specularity pass
  uint16_t floating_trig : 1;     // This face only exists as a floating trigger
  uint16_t destroyed : 1;         // This face has been blown up
  uint16_t volumetric : 1;        // This face is a volumetric face
  uint16_t triangulated : 1;      // ??
  uint16_t visible : 1;           // This face is visible this frame (Valid only during render)
  uint16_t not_shell : 1;         // This face is not part of the room shell
  uint16_t touched : 1;           // This face has been touched by fvi_QuickDistFaceList
  uint16_t goalface : 1;          // This face is a goal texture face
  uint16_t not_facing : 1;        // This face is not facing us this frame (Valid only during render)
  uint16_t scorched : 1;          // This face has one or more scorch marks
#endif
};
static_assert(sizeof(face_flags_t) == sizeof(uint16_t));

// Serialized as the raw uint16 face-flag word on disk.
inline byte_istream& operator>>(byte_istream& input, face_flags_t& data) {
  uint16_t raw = 0;
  input >> raw;
  data = std::bit_cast<face_flags_t>(raw);
  return input;
}
inline byte_ostream& operator<<(byte_ostream& output, const face_flags_t& data) {
  return output << std::bit_cast<uint16_t>(data);
}

// UVLs for room verts
struct roomUVL {
  float u, v; // texture coordinates
  float u2, v2;
  uint8_t alpha; // alpha for this vertex
};

// an n-sided polygon used as part of a room or portal
struct face {
  face_flags_t flags;         // flags for this face (see above)
  uint8_t num_verts;  // how many vertices in this face
  int8_t portal_num; // which portal this face is part of, or -1 if none

  std::vector<int16_t> face_verts;   // indices into the room's vertex list
  std::vector<roomUVL> face_uvls;    // per-vertex uvls
  vector3 normal;           // the surface normal of this face
  int16_t tmap;              // texture numbers for this face
  uint16_t lmi_handle;       // the lightmap info number for this face
  int16_t special_handle;    // the index into the special_faces array
  uint8_t renderframe;       // what frame this face was last rendered (for lighting)
  uint8_t light_multiple;    // what multiple to times by
  vector3 min_xyz, max_xyz; // min & max extents of this face (for FVI)
};

// Portal flags
struct [[gnu::packed]] portal_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 25;
  uint32_t block_removable : 1;
  uint32_t block : 1;              // This portal is blocked
  uint32_t changed : 1;            // Used for multiplayer - this portal has been changed
  uint32_t combined : 1;           // This portal has been combined with another for rendering purposes
  uint32_t too_small_for_robot : 1; // Too small for a robot to use for path following
  uint32_t rendered_flythrough : 1; // allow flythrough of rendered faces
  uint32_t render_faces : 1;       // render the face(s) in the portal
#else
  uint32_t render_faces : 1;       // render the face(s) in the portal
  uint32_t rendered_flythrough : 1; // allow flythrough of rendered faces
  uint32_t too_small_for_robot : 1; // Too small for a robot to use for path following
  uint32_t combined : 1;           // This portal has been combined with another for rendering purposes
  uint32_t changed : 1;            // Used for multiplayer - this portal has been changed
  uint32_t block : 1;              // This portal is blocked
  uint32_t block_removable : 1;
  uint32_t padding : 25;
#endif
};
static_assert(sizeof(portal_flags_t) == sizeof(uint32_t));

// Serialized as the raw uint32 portal-flag word on disk.
inline byte_istream& operator>>(byte_istream& input, portal_flags_t& data) {
  uint32_t raw = 0;
  input >> raw;
  data = std::bit_cast<portal_flags_t>(raw);
  return input;
}
inline byte_ostream& operator<<(byte_ostream& output, const portal_flags_t& data) {
  return output << std::bit_cast<uint32_t>(data);
}

// a connection between two rooms
struct portal {
  portal_flags_t flags;      // flags for this portal
  int16_t portal_face; // the face for this portal
  int16_t croom;       // the room this portal connects to
  int16_t cportal;     // the portal in croom this portal connects to
  int16_t bnode_index;
  int combine_master; // For rendering combined portals
  vector3 path_pnt;    // Point used by the path system
};

#define ROOM_NAME_LEN 19 // how long a room name can be (not counting null terminator)

struct doorway;

struct [[gnu::packed]] room_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 2;             // Unused padding to complete 32 bits
  uint32_t no_light : 1;            // This room does not get lit
  uint32_t secret : 1;              // This room is a secret room
  uint32_t waypoint : 1;            // This room has a waypoint in it
  uint32_t manual_path_pnt : 1;     // The room path_pnt has been set manually (i.e. by the designer)
  uint32_t inform_relink_to_lg : 1; // Informs the level goal system on player relinking to this room
  uint32_t mine : 5;                // Mine index of this room (we support up to 32 individual mines without a problem)
  uint32_t flicker : 1;             // This room flickers with pulse lighting
  uint32_t strobe : 1;              // This room strobes with pulse lighting
  uint32_t triangulate : 1;         // All the faces in this room should be drawn with triagulation on
  uint32_t mirror_visible : 1;      // The mirror is this room is visible
  uint32_t special6 : 1;            // This room is a special room
  uint32_t special5 : 1;            // This room is a special room
  uint32_t special4 : 1;            // This room is a special room
  uint32_t special3 : 1;            // This room is a special room
  uint32_t special2 : 1;            // This room is a special room
  uint32_t special1 : 1;            // This room is a special room
  uint32_t fog : 1;                 // this room is fogged
  uint32_t goal4 : 1;               // this room is goal 4
  uint32_t goal3 : 1;               // this room is goal 3
  uint32_t sorted_inc_y : 1;        // Faces are sorted with increasing y
  uint32_t touches_terrain : 1;     // this room should recieve lighting from satellites
  uint32_t goal2 : 1;               // this room is goal 2
  uint32_t goal1 : 1;               // this room is goal 1
  uint32_t external : 1;            // this is an external room (i.e. a building)
  uint32_t door : 1;                // a 3d door is here.
  uint32_t fuelcen : 1;             // room is a refueling center
#else
  uint32_t fuelcen : 1;             // room is a refueling center
  uint32_t door : 1;                // a 3d door is here.
  uint32_t external : 1;            // this is an external room (i.e. a building)
  uint32_t goal1 : 1;               // this room is goal 1
  uint32_t goal2 : 1;               // this room is goal 2
  uint32_t touches_terrain : 1;     // this room should recieve lighting from satellites
  uint32_t sorted_inc_y : 1;        // Faces are sorted with increasing y
  uint32_t goal3 : 1;               // this room is goal 3
  uint32_t goal4 : 1;               // this room is goal 4
  uint32_t fog : 1;                 // this room is fogged
  uint32_t special1 : 1;            // This room is a special room
  uint32_t special2 : 1;            // This room is a special room
  uint32_t special3 : 1;            // This room is a special room
  uint32_t special4 : 1;            // This room is a special room
  uint32_t special5 : 1;            // This room is a special room
  uint32_t special6 : 1;            // This room is a special room
  uint32_t mirror_visible : 1;      // The mirror is this room is visible
  uint32_t triangulate : 1;         // All the faces in this room should be drawn with triagulation on
  uint32_t strobe : 1;              // This room strobes with pulse lighting
  uint32_t flicker : 1;             // This room flickers with pulse lighting
  uint32_t mine : 5;                // Mine index of this room (we support up to 32 individual mines without a problem)
  uint32_t inform_relink_to_lg : 1; // Informs the level goal system on player relinking to this room
  uint32_t manual_path_pnt : 1;     // The room path_pnt has been set manually (i.e. by the designer)
  uint32_t waypoint : 1;            // This room has a waypoint in it
  uint32_t secret : 1;              // This room is a secret room
  uint32_t no_light : 1;            // This room does not get lit
  uint32_t padding : 2;             // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(room_flags_t) == sizeof(uint32_t));

// Serialized as the raw uint32 room-flag word on disk.
inline byte_istream& operator>>(byte_istream& input, room_flags_t& data) {
  uint32_t raw = 0;
  input >> raw;
  data = std::bit_cast<room_flags_t>(raw);
  return input;
}
inline byte_ostream& operator<<(byte_ostream& output, const room_flags_t& data) {
  return output << std::bit_cast<uint32_t>(data);
}

// the basic building-block of a Descent 3 level
struct room {
  room_flags_t flags;

  int num_faces;   // how many poygons in this room
  int num_portals; // how many connections in this room
  int num_verts;   // how many verts in the room
  std::vector<face> faces;     // list of faces (num_faces)
  std::vector<portal> portals; // list of portals (num_portals)
  std::vector<vector3> verts;  // array of vertices for this room (num_verts)
  std::vector<vector4> verts4; // array of 16byte vertices for this room (num_verts, Katmai)

  std::unique_ptr<doorway> doorway_data;   // this room's doorway data, or null if not a doorway
  std::string name;              // name of this room, or NULL
  int objects;             // index of first object in this room
  vector3 max_xyz, min_xyz; // for external room visibility checking

  float last_render_time; // Last time we rendered this room

  // Hierarchical bounding boxes for this room
  vector3 bbf_min_xyz;
  vector3 bbf_max_xyz;
  int16_t num_bbf_regions;
  int16_t pad1;
  std::vector<std::vector<int16_t>> bbf_list; // faces per region (num_bbf_regions lists)
  std::vector<int16_t> num_bbf;               // face count per region
  std::vector<vector3> bbf_list_min_xyz;       // min extent per region
  std::vector<vector3> bbf_list_max_xyz;       // max extent per region
  std::vector<uint8_t> bbf_list_sector;        // sector id per region

  bn_list bn_info;

  int16_t wpb_index;           // world point buffer index - where this room starts
  uint8_t pulse_time;          // each room can has a pulse time
  uint8_t pulse_offset;        // each room has a timer offset for which it pulses
  vector3 wind;               // Wind vector for the room
  int ambient_sound;         // Index of ambient sound pattern for this room, or -1 if none
  int16_t vis_effects;         // index of first visual effect in this room
  int16_t mirror_face;         // Index of face that this room is to be mirrored by
  uint8_t num_mirror_faces;    // Number of faces in this room that have the same texture as the mirror
  std::vector<uint16_t> mirror_faces_list; // the list of faces in this room that have the same texture as the mirror
  float damage;              // The damage per second applied to players (& maybe others) in room

  vector3 path_pnt;      // Point used by the path system
  std::vector<uint8_t> volume_lights; // memory for our volumetric lighting
  int16_t volume_width;   // The dimensions of our volumetric room
  int16_t volume_height;
  int16_t volume_depth;
  float fog_depth;           // How far until fog is totally opaque
  float fog_r, fog_g, fog_b; // Fog color

  uint8_t env_reverb;        // environmental reverb preset
  uint8_t room_change_flags; // For multiplayer, detects what characteristics have to be sent
  uint8_t damage_type;       // What type of damage this rooms does (for sound) if damage > 0
  uint8_t used;              // is this room holding data?

};

// Level-file (ROOM chunk) record serialization.  These use the current
// on-disk layouts (file version >= 127) and are the exact mirror of the
// engine's readers/writers for those versions.  Older-file quirks are handled
// by the version-gated helpers in level_loader.cpp.
byte_istream& operator>>(byte_istream& input, face& data);
byte_ostream& operator<<(byte_ostream& output, const face& data);
byte_istream& operator>>(byte_istream& input, portal& data);
byte_ostream& operator<<(byte_ostream& output, const portal& data);
byte_istream& operator>>(byte_istream& input, room& data);
byte_ostream& operator<<(byte_ostream& output, const room& data);

#endif
