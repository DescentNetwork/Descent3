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
*/

#ifndef ROBOTFIRESTRUCT_H_
#define ROBOTFIRESTRUCT_H_

#include <cstdint>

#include "robotfirestruct_external.h"
#include "vecmat.h"

// NOTE: CHANGE gunbattery.h (IN POFGEN PROJECT) if constants are changed
// NOTE: Robots are limited to the number of wb configurations of the player.  This seem like an
//       adequit number (currently 21)
// (MAX_PRIMARY_WEAPONS + MAX_SECONDARY_WEAPONS + 1/*Flare*/)
// NOTE: Cannot include weapon.h because of circular dependances.
#define MAX_WBS_PER_OBJ 21

// Attach to the polymodel
struct poly_wb_info {
  // Static Data  (Add to robot generic page)
  uint16_t num_gps;
  uint8_t gp_index[MAX_WB_GUNPOINTS];

  // Turrets are listed from most important (greatest mobility) to least important
  uint8_t num_turrets;
  uint16_t turret_index[MAX_WB_TURRETS];

};

// Next free WBF is 32


struct [[gnu::packed]] otype_wb_info_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint16_t padding : 4;                // Unused padding to complete 16 bits
  uint16_t fire_target : 1;            // WBF_FIRE_TARGET (2048)
  uint16_t aim_fvec : 1;               // WBF_AIM_FVEC (1024)
  uint16_t fire_fvec : 1;              // WBF_FIRE_FVEC (512)
  uint16_t user_timeout : 1;           // WBF_USER_TIMEOUT (256)
  uint16_t use_custom_max_dist : 1;    // WBF_USE_CUSTOM_MAX_DIST (128)
  uint16_t on_off : 1;                 // WBF_ON_OFF (64)
  uint16_t use_custom_fov : 1;         // WBF_USE_CUSTOM_FOV (32)
  uint16_t guided : 1;                 // WBF_GUIDED (16)
  uint16_t random_fire_order : 1;      // WBF_RANDOM_FIRE_ORDER (8)
  uint16_t anim_full : 1;              // WBF_ANIM_FULL (4)
  uint16_t anim_local : 1;             // WBF_ANIM_LOCAL (2)
  uint16_t spray : 1;                  // WBF_SPRAY (1)
#else
  uint16_t spray : 1;                  // WBF_SPRAY (1)
  uint16_t anim_local : 1;             // WBF_ANIM_LOCAL (2)
  uint16_t anim_full : 1;              // WBF_ANIM_FULL (4)
  uint16_t random_fire_order : 1;      // WBF_RANDOM_FIRE_ORDER (8)
  uint16_t guided : 1;                 // WBF_GUIDED (16)
  uint16_t use_custom_fov : 1;         // WBF_USE_CUSTOM_FOV (32)
  uint16_t on_off : 1;                 // WBF_ON_OFF (64)
  uint16_t use_custom_max_dist : 1;    // WBF_USE_CUSTOM_MAX_DIST (128)
  uint16_t user_timeout : 1;           // WBF_USER_TIMEOUT (256)
  uint16_t fire_fvec : 1;              // WBF_FIRE_FVEC (512)
  uint16_t aim_fvec : 1;               // WBF_AIM_FVEC (1024)
  uint16_t fire_target : 1;            // WBF_FIRE_TARGET (2048)
  uint16_t padding : 4;                // Unused padding to complete 16 bits
#endif
};
static_assert(sizeof(otype_wb_info_flags_t) == sizeof(uint16_t));


// Attach to a object type
struct otype_wb_info {
  uint16_t gp_weapon_index[MAX_WB_GUNPOINTS];
  uint16_t fm_fire_sound_index[MAX_WB_FIRING_MASKS];
  uint16_t aiming_gp_index;

  uint8_t num_masks;
  uint8_t gp_fire_masks[MAX_WB_FIRING_MASKS];
  float gp_fire_wait[MAX_WB_FIRING_MASKS];

  uint8_t gp_quad_fire_mask;

  uint8_t num_levels;
  uint16_t gp_level_weapon_index[MAX_WB_UPGRADES];
  uint16_t gp_level_fire_sound_index[MAX_WB_UPGRADES];

  uint8_t aiming_flags;
  float aiming_3d_dot; // These can be reused.
  float aiming_3d_dist;
  float aiming_XZ_dot;

  float anim_start_frame[MAX_WB_FIRING_MASKS];
  float anim_fire_frame[MAX_WB_FIRING_MASKS];
  float anim_end_frame[MAX_WB_FIRING_MASKS];
  float anim_time[MAX_WB_FIRING_MASKS];

  otype_wb_info_flags_t flags;

  float energy_usage, ammo_usage;
};

#define WB_MOVE_STILL 0
#define WB_MOVE_RIGHT 1
#define WB_MOVE_LEFT 2


struct [[gnu::packed]] dynamic_wb_info_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 26;
  uint32_t upgraded : 1;
  uint32_t quad : 1;
  uint32_t anim_fired : 1;
  uint32_t animating : 1;
  uint32_t automatic : 1;
  uint32_t enabled : 1;
#else
  uint32_t enabled : 1;
  uint32_t automatic : 1;
  uint32_t animating : 1;
  uint32_t anim_fired : 1;
  uint32_t quad : 1;
  uint32_t upgraded : 1;
  uint32_t padding : 26;
#endif
};
static_assert(sizeof(dynamic_wb_info_flags_t) == sizeof(uint32_t));

// Goes with an individual robot's instance
struct dynamic_wb_info {
  // Dynamic Data
  float last_fire_time;
  uint8_t cur_firing_mask;

  float norm_turret_angle[MAX_WB_TURRETS];
  float turret_next_think_time[MAX_WB_TURRETS];
  uint8_t turret_direction[MAX_WB_TURRETS];

  uint8_t wb_anim_mask;
  float wb_anim_frame;

  vector3 cur_target;

  char upgrade_level; // For multi-level weapons ( 0 to MAX_WB_UPGRADES-1)

  dynamic_wb_info_flags_t flags;
};

#endif
