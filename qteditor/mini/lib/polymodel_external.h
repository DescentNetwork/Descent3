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

 * $Logfile: /DescentIII/Main/lib/polymodel_external.h $
 * $Revision: 7 $
 * $Date: 10/02/01 8:56a $
 * $Author: Matt $
 *
 * Polymodel defines, flags and structs (anything that could be exported to a DLL)
 *
 * $Log: /DescentIII/Main/lib/polymodel_external.h $
 *
 * 7     10/02/01 8:56a Matt
 * Increased MAX_POLY_MODELS from 1000 to 1200
 *
 * 6     7/08/99 5:47p Jason
 * changes for new bumpmapping system in 1.1 update patch
 *
 * 5     6/08/99 1:00p Jason
 * changes for bumpmapping
 *
 * 4     4/20/99 1:02p 3dsmax
 * fixed polymodel loimt
 *
 * 3     4/08/99 11:45a Jason
 * greatly sped up the time it takes to get model anges/positions by
 * precalculation
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

#ifndef POLYMODEL_EXTERNAL_H_
#define POLYMODEL_EXTERNAL_H_

#include <cstdint>
#include <string>
#include <vector>

#include "grdefs.h"
#include "manage_external.h"
#include "robotfirestruct.h"
#include "vecmat_external.h"

#define MAX_POLY_MODELS 1200

#define MAX_MODEL_TEXTURES 35
#define MAX_POLYGON_VECS 2500
#define MAX_DETAIL_LEVELS 3
#define MAX_PROP_LEN 256
#define MAX_NAME_LEN 32

#define MAX_GROUND_PLANES_PER_MODEL 10
#define MAX_GUNS_PER_MODEL 64
#define MAX_SUBOBJECTS 30
#define MAX_POINTS_PER_SUBOBJECT 300

// gun bank
struct w_bank {
  int parent;
  vector3 pnt;
  vector3 norm;
};

// attach bank
struct a_bank {
  int parent;
  vector3 pnt;
  vector3 norm;
  vector3 uvec;
  bool f_uvec;
};

struct lightmap_object_face {
  uint8_t num_verts;
  uint16_t lmi_handle;
  vector3 rvec, uvec;
  float *u2, *v2;
};

struct lightmap_object {
  uint8_t num_models;

  std::vector<int16_t> num_faces;
  std::vector<std::vector<lightmap_object_face>> lightmap_faces;
  uint8_t used;

};

struct polyface {
  int8_t nverts;
  std::vector<int16_t> vertnums;
  std::vector<float> u;
  std::vector<float> v;

  // float *u2,*v2;						// For lightmaps only

  ddgr_color color;
  int16_t texnum;

  vector3 normal;
};

// glow info
struct glowinfo {
  float glow_r, glow_g, glow_b, glow_size, glow_length;
  vector3 center, normal;
};

struct [[gnu::packed]] subobject_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t : 6;               // Unused bits (26-31)
  uint32_t headlight : 1;     // This subobject is a headlight
  uint32_t jitter : 1;        // This object jitters by itself
  uint32_t thruster : 1;      // This is a thruster subobject
  uint32_t custom : 1;        // This subobject has textures/colors that are customizable
  uint32_t glow : 1;          // This subobject glows
  uint32_t wb_index : 5;      // Packed weapon-battery index (bits 16-20)
  uint32_t wb : 1;            // This subobject is part of a weapon battery
  uint32_t layer : 1;         // This subobject is marked as part of possible secondary model rendering
  uint32_t viewer : 1;        // This subobject is marked as a 'viewer'
  uint32_t facing : 1;        // This subobject always faces you
  uint32_t monitor8 : 1;      // This subobject contains its eighth monitor
  uint32_t monitor7 : 1;      // This subobject contains its seventh monitor
  uint32_t monitor6 : 1;      // This subobject contains its sixth monitor
  uint32_t monitor5 : 1;      // This subobject contains its fifth monitor
  uint32_t monitor4 : 1;      // This subobject contains its fourth monitor
  uint32_t monitor3 : 1;      // This subobject contains its third monitor
  uint32_t monitor2 : 1;      // This subobject contains its second monitor
  uint32_t monitor1 : 1;      // This subobject contains its first monitor
  uint32_t frontface : 1;     // This subobject contains the front face for the door
  uint32_t shell : 1;         // This subobject is a door housing
  uint32_t turret : 1;        // This subobject is a turret that tracks
  uint32_t rotate : 1;        // This subobject is a rotator
#else
  uint32_t rotate : 1;        // This subobject is a rotator
  uint32_t turret : 1;        // This subobject is a turret that tracks
  uint32_t shell : 1;         // This subobject is a door housing
  uint32_t frontface : 1;     // This subobject contains the front face for the door
  uint32_t monitor1 : 1;      // This subobject contains its first monitor
  uint32_t monitor2 : 1;      // This subobject contains its second monitor
  uint32_t monitor3 : 1;      // This subobject contains its third monitor
  uint32_t monitor4 : 1;      // This subobject contains its fourth monitor
  uint32_t monitor5 : 1;      // This subobject contains its fifth monitor
  uint32_t monitor6 : 1;      // This subobject contains its sixth monitor
  uint32_t monitor7 : 1;      // This subobject contains its seventh monitor
  uint32_t monitor8 : 1;      // This subobject contains its eighth monitor
  uint32_t facing : 1;        // This subobject always faces you
  uint32_t viewer : 1;        // This subobject is marked as a 'viewer'
  uint32_t layer : 1;         // This subobject is marked as part of possible secondary model rendering
  uint32_t wb : 1;            // This subobject is part of a weapon battery
  uint32_t wb_index : 5;      // Packed weapon-battery index (bits 16-20)
  uint32_t glow : 1;          // This subobject glows
  uint32_t custom : 1;        // This subobject has textures/colors that are customizable
  uint32_t thruster : 1;      // This is a thruster subobject
  uint32_t jitter : 1;        // This object jitters by itself
  uint32_t headlight : 1;     // This subobject is a headlight
  uint32_t : 6;               // Unused bits (26-31)
#endif
};
static_assert(sizeof(subobject_flags_t) == sizeof(uint32_t));

// bsp information
struct bsp_info {
  std::string name; // name of the subsystem.  Probably displayed on HUD
  int movement_type;       //-1 if no movement, otherwise rotational or positional movement -- subobjects only
  int movement_axis;       // which axis this subobject moves or rotates on.
  int tree_offset;         // offset of tree data (children included) into the model_data
  int data_offset;         // offset of data into the model_data
  vector3 offset;           // 3d offset from parent object
  vector3 norm;             // norm for sep plane
  float d;                 // norm d for sep plane
  vector3 pnt;              // point for sep plane
  vector3 geometric_center; // geometric center of this subobject.  In the same Frame Of
                           // Reference as all other vertices in this submodel. (Relative to pivot point)
  float rad;               // radius for each submodel

  std::vector<vector3> verts; // vertices for the submodel (NEWSTYLE)
  std::vector<vector3> vertnorms;
  std::vector<float> alpha;
  std::vector<polyface> faces; // faces for the submodel (NEWSTYLE)
  std::vector<vector3> face_min;
  std::vector<vector3> face_max;

  std::vector<int16_t> vertnum_memory;
  std::vector<float> u_memory;
  std::vector<float> v_memory;

  int nverts;
  int num_faces; // amount of faces (NEWSTYLE);

  int8_t children[MAX_SUBOBJECTS]; // children of this submodel
  int parent;                     // what is parent for each submodel
  uint8_t num_children;

  vector3 min;
  vector3 max;
  int blown_off; // If set, this subobject is blown off. Stuffed by model_set_instance

  angvec angs;
  matrix mod_matrix; // The angles from parent.  Stuffed by model_set_instance
  vector3 mod_pos;    // The modified position of this object.  Used for positional interpolation

  std::vector<vector3> keyframe_axis; // the axis of rotation for each keyframe
  std::vector<int> keyframe_angles;  // The destination angles for each key frame
  std::vector<vector3> keyframe_pos;
  std::vector<matrix> keyframe_matrix; // the combined rotation matrices up to frame n
  std::vector<uint16_t> tick_pos_remap;  // For looking up keyframes fast
  std::vector<uint16_t> tick_ang_remap;  // For looking up keyframes fast
  std::vector<int> rot_start_time;
  std::vector<int> pos_start_time;

  int num_key_angles;
  int num_key_pos;

  subobject_flags_t flags;

  int rot_track_min, rot_track_max;
  int pos_track_min, pos_track_max;

  float rps;            // if rotate or turret is set, this is the rotations per second
  float fov;            // Half the normalized angle that this turret is allowed to turn (amount in each direction)
  float think_interval; // How quickly a turret updates which way it should move

  float normalized_angle;

  std::vector<glowinfo> glow_info;
};

// polymodel flags
struct [[gnu::packed]] polymodel_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t : 26;              // Unused bits (6-31)
  uint32_t size_computed : 1; // This polymodel's size is computed
  uint32_t not_resident : 1;  // This polymodel is not in memory
  uint32_t facing : 1;        // Has a submodel that is always facing
  uint32_t alpha : 1;         // Has alpha per vertex qualities
  uint32_t timed : 1;         // Uses new timed animation
  uint32_t lightmap_res : 1;
#else
  uint32_t lightmap_res : 1;
  uint32_t timed : 1;         // Uses new timed animation
  uint32_t alpha : 1;         // Has alpha per vertex qualities
  uint32_t facing : 1;        // Has a submodel that is always facing
  uint32_t not_resident : 1;  // This polymodel is not in memory
  uint32_t size_computed : 1; // This polymodel's size is computed
  uint32_t : 26;              // Unused bits (6-31)
#endif
};
static_assert(sizeof(polymodel_flags_t) == sizeof(uint32_t));

static_assert(sizeof(bool) == sizeof(uint8_t));
// used to describe a polygon model
struct poly_model {
  uint16_t used;

  polymodel_flags_t flags;
  uint8_t new_style; // if 1, then this polymodel is in the new outrage format (oof)
  int id;          // what the polygon model number is.  (Index in Poly_models)
  int version;
  std::string name;

  int n_models;
  int model_data_size;
  std::vector<uint8_t> model_data;

  vector3 mins, maxs; // min,max for whole model
  vector3 view_pos;   // viewing position.  Default to {0,0,0}.

  float wall_size;
  vector3 wall_size_offset;

  float anim_size;
  vector3 anim_size_offset;

  float rad;
  int n_textures;
  int16_t textures[MAX_MODEL_TEXTURES]; // a list of bitmap indices

  std::vector<bsp_info> submodel; // an array of size n_models of submodel info.
  int num_key_angles;
  int num_key_pos;
  int max_keys; // the greater number of num_key_pos or num_key_angles

  int frame_min, frame_max; // For TIMED polymodels, the min/max frames

  int n_guns;
  std::vector<w_bank> gun_slots; // array of gun banks

  int n_ground;
  std::vector<w_bank> ground_slots; // array of ground planes

  int n_attach;
  std::vector<a_bank> attach_slots;

  int num_wbs;
  std::vector<poly_wb_info> poly_wb; // array of weapon batteries

  std::vector<int> render_order; // internal use
};

// Which kind of lighting model for this polymodel
enum polymodel_light_type {
  POLYMODEL_LIGHTING_STATIC,
  POLYMODEL_LIGHTING_GOURAUD,
  POLYMODEL_LIGHTING_LIGHTMAP,
};

// polymodel effects stuff
struct [[gnu::packed]] polymodel_effects_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t : 15;              // Unused bits (17-31)
  uint32_t bumpmapped : 1;
  uint32_t custom_glow : 1;
  uint32_t no_glows : 1;
  uint32_t draw_headlights : 1;
  uint32_t thruster_scalar : 1;
  uint32_t glow_scalar : 1;
  uint32_t specular_faces : 1;
  uint32_t specular_model : 1;
  uint32_t fogged_model : 1;
  uint32_t custom_texture : 1;
  uint32_t custom_color : 1;
  uint32_t fog : 1;
  uint32_t lo_res : 1;
  uint32_t med_res : 1;
  uint32_t color : 1;
  uint32_t deform : 1;
  uint32_t alpha : 1;
#else
  uint32_t alpha : 1;
  uint32_t deform : 1;
  uint32_t color : 1;
  uint32_t med_res : 1;
  uint32_t lo_res : 1;
  uint32_t fog : 1;
  uint32_t custom_color : 1;
  uint32_t custom_texture : 1;
  uint32_t fogged_model : 1;
  uint32_t specular_model : 1;
  uint32_t specular_faces : 1;
  uint32_t glow_scalar : 1;
  uint32_t thruster_scalar : 1;
  uint32_t draw_headlights : 1;
  uint32_t no_glows : 1;
  uint32_t custom_glow : 1;
  uint32_t bumpmapped : 1;
  uint32_t : 15;              // Unused bits (17-31)
#endif
};
static_assert(sizeof(polymodel_effects_flags_t) == sizeof(uint32_t));


struct polymodel_effect {
  polymodel_effects_flags_t type;
  float alpha;
  float deform_range;
  float r, g, b;
  float fog_r, fog_g, fog_b;
  int custom_texture;
  int custom_color;

  float spec_r, spec_g, spec_b;
  vector3 spec_light_pos;
  float spec_scalar;

  vector3 bump_light_pos;
  float bump_scalar;

  float fog_depth;
  float fog_eye_distance;
  float fog_distance;
  vector3 fog_plane, fog_portal_vert;
  int fog_plane_check;

  float glow_length_scalar;
  float glow_size_scalar;

  float glow_r, glow_g, glow_b;

};

#endif
