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

// Generic page reader (ported from the full engine's manage/generic.cpp), plus
// the shared physics / weapon-battery / lighting chunk readers that are used by
// several per-page readers.  Ported for the mini editor so it reads the D3
// page-table metadata from Table.gam (inside d3.hog) into the mini editor's
// global arrays without touching the game renderer.  This file links only
// against Qt + OpenGL + the mini cfile implementation.

#include "genericpage.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <QtGlobal>

#include "manage.h"
#include "mem.h"     // mem_rmalloc
#include "objinfo.h" // object_info, OBJ_POWERUP, ...
#include "aistruct.h" // t_ai_info
#include "aistruct_external.h"
#include "object_external_struct.h" // physics_info, light_info, MAX_OBJECTS
#include "gamedata_helpers.h"

// Old delay types (originally #defined locally in the full-engine generic.cpp)
#ifndef OLD_DF_DELAY_MIN_MAX
#define OLD_DF_DELAY_MIN_MAX 0x0000001
#endif
#ifndef OLD_DF_DELAY_MASK
#define OLD_DF_DELAY_MASK 0x0000003
#endif

//-----------------------------------------------------------------------------
// Chunk readers (used by the generic + weapon + ship readers).  Pure data.
//-----------------------------------------------------------------------------

void mng_ReadPhysicsChunk(physics_info *phys_info, posix_istream &infile) {
  infile >> phys_info->mass;
  infile >> phys_info->drag;
  infile >> phys_info->full_thrust;
  infile >> phys_info->flags;
  infile >> phys_info->rotdrag;
  infile >> phys_info->full_rotthrust;
  infile >> phys_info->num_bounces;
  infile >> phys_info->velocity.z();
  {
    float a, b, c;
    infile >> a >> b >> c;
    phys_info->rotvel = {a, b, c};
  }
  infile >> phys_info->wiggle_amplitude;
  infile >> phys_info->wiggles_per_sec;
  infile >> phys_info->coeff_restitution;
  infile >> phys_info->hit_die_dot;
  infile >> phys_info->max_turnroll_rate;
  infile >> phys_info->turnroll_ratio;
}

void mng_ReadWeaponBatteryChunk(otype_wb_info *static_wb, posix_istream &infile, int version) {
  int j;

  infile >> static_wb->energy_usage;
  infile >> static_wb->ammo_usage;

  for (j = 0; j < MAX_WB_GUNPOINTS; j++) {
    infile >> static_wb->gp_weapon_index[j];
  }

  for (j = 0; j < MAX_WB_FIRING_MASKS; j++) {
    infile >> static_wb->gp_fire_masks[j];
    infile >> static_wb->gp_fire_wait[j];
    infile >> static_wb->anim_time[j];
    infile >> static_wb->anim_start_frame[j];
    infile >> static_wb->anim_fire_frame[j];
    infile >> static_wb->anim_end_frame[j];
  }
  infile >> static_wb->num_masks;
  infile >> static_wb->aiming_gp_index;
  infile >> static_wb->aiming_flags;
  infile >> static_wb->aiming_3d_dot;
  infile >> static_wb->aiming_3d_dist;
  infile >> static_wb->aiming_XZ_dot;
  if (version >= 2) {
    uint16_t flags_raw = 0;
    infile >> flags_raw;
    std::memcpy(&static_wb->flags, &flags_raw, sizeof(flags_raw));
  } else {
    uint8_t b = 0;
    infile >> b;
    uint16_t flags_raw = b;
    std::memcpy(&static_wb->flags, &flags_raw, sizeof(flags_raw));
  }
  infile >> static_wb->gp_quad_fire_mask;
}

void mng_ReadLightingChunk(light_info *lighting_info, posix_istream &infile) {
  infile >> lighting_info->light_distance;
  infile >> lighting_info->red_light1;
  infile >> lighting_info->green_light1;
  infile >> lighting_info->blue_light1;
  infile >> lighting_info->time_interval;
  infile >> lighting_info->flicker_distance;
  infile >> lighting_info->directional_dot;
  infile >> lighting_info->red_light2;
  infile >> lighting_info->green_light2;
  infile >> lighting_info->blue_light2;
  infile >> lighting_info->flags;
  infile >> lighting_info->timebits;
  infile >> lighting_info->angle;
  infile >> lighting_info->lighting_render_type;
}

//-----------------------------------------------------------------------------
// Generic page (manage/generic.cpp : 448-1337)
//-----------------------------------------------------------------------------

static void mng_InitGenericPage(mngs_generic_page *genericpage) {
  int i;

  *genericpage = mngs_generic_page{};
  genericpage->image_name.clear();

  genericpage->med_image_name.clear();
  genericpage->lo_image_name.clear();

  for (i = 0; i < MAX_OBJ_SOUNDS; i++)
    genericpage->sound_name[i].clear();

  for (i = 0; i < MAX_AI_SOUNDS; i++)
    genericpage->ai_sound_name[i].clear();

  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    genericpage->dspew_name[i].clear();
  }

  genericpage->objinfo_struct.description = NULL;
  genericpage->objinfo_struct.icon_name[0] = '\0';

  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++)
    for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++)
      genericpage->anim_sound_name[i][j].clear();

  genericpage->objinfo_struct.med_lod_distance = DEFAULT_MED_LOD_DISTANCE;
  genericpage->objinfo_struct.lo_lod_distance = DEFAULT_LO_LOD_DISTANCE;

  genericpage->objinfo_struct.phys_info.hit_die_dot = 1.0f;
  genericpage->objinfo_struct.respawn_scalar = 1.0f;

  genericpage->ai_info.curiousity = .5f;
  genericpage->ai_info.night_vision = .7f;
  genericpage->ai_info.fog_vision = .7f;
  genericpage->ai_info.lead_accuracy = 1.0f;
  genericpage->ai_info.lead_varience = 0.0f;
  genericpage->ai_info.fire_spread = 0.0f;
  genericpage->ai_info.fight_team = 0.15f;
  genericpage->ai_info.fight_same = 0.8f;
  genericpage->ai_info.aggression = 0.5f;
  genericpage->ai_info.hearing = 1.0f;
  genericpage->ai_info.frustration = 0.5f;
  genericpage->ai_info.roaming = 0.5f;
  genericpage->ai_info.life_preservation = 0.0f;
  genericpage->objinfo_struct.module_name[0] = '\0';

  for (i = 0; i < MAX_DEATH_TYPES; i++) {
    memset(&genericpage->objinfo_struct.death_types[i].flags, 0, sizeof(uint32_t));
    genericpage->objinfo_struct.death_types[i].delay_min = 0.0;
    genericpage->objinfo_struct.death_types[i].delay_max = 0.0;
    genericpage->objinfo_struct.death_probabilities[i] = 0;
  }
}

static void GenericPageSetPowerupDefaultAmmo(object_info *ip) {
  // Default is zero
  ip->ammo_count = 0;

  const std::unordered_map<std::string, uint32_t> default_ammo =
  {
    { "Vauss", 5000 },
    { "Napalm", 500 },
    { "MassDriver", 20 },
    { "Frag", 1 },
    { "ImpactMortar", 1 },
    { "NapalmRocket", 1 },
    { "Cyclone", 1 },
    { "BlackShark", 1 },
    { "Concussion", 1 },
    { "Homing", 1 },
    { "Smart", 1 },
    { "Mega", 1 },
    { "Guided", 1 },
    { "4PackHoming", 4 },
    { "4PackConc", 4 },
    { "4PackFrag", 4 },
    { "4PackGuided", 4 },
    { "Vauss clip", 1250 },
    { "MassDriverAmmo", 5 },
    { "NapalmTank", 100 },
  };

  ip->ammo_count = default_ammo.at(ip->name);
}

// Reads a generic page from an open file.  Returns 0 on error.
int mng_ReadNewGenericPage(posix_istream &infile, mngs_generic_page *genericpage) {
  int i, j;

  mng_InitGenericPage(genericpage);

  int16_t version_tmp = 0;
  infile >> version_tmp;
  int version = version_tmp;

  {
    uint8_t b = 0;
    infile >> b;
    genericpage->objinfo_struct.type = b;
  }

  // Read object name
  infile >> genericpage->objinfo_struct.name;

  // Read model names
  infile >> genericpage->image_name;
  infile >> genericpage->med_image_name;
  infile >> genericpage->lo_image_name;

  // Read out impact data
  infile >> genericpage->objinfo_struct.impact_size;
  infile >> genericpage->objinfo_struct.impact_time;
  infile >> genericpage->objinfo_struct.damage;

  // Read score
  if (version >= 24) {
    int16_t s = 0;
    infile >> s;
    genericpage->objinfo_struct.score = s;
  } else {
    uint8_t b = 0;
    infile >> b;
    genericpage->objinfo_struct.score = b;
  }

  // Read ammo
  if (genericpage->objinfo_struct.type == OBJ_POWERUP) {
    if (version >= 25) {
      int16_t a = 0;
      infile >> a;
      genericpage->objinfo_struct.ammo_count = a;
    } else
      GenericPageSetPowerupDefaultAmmo(&genericpage->objinfo_struct);
  } else
    genericpage->objinfo_struct.ammo_count = 0;

  // Read script name (discarded)
  std::string dummy;
  infile >> dummy; // genericpage->objinfo_struct.script_name

  if (version >= 18) {
    infile >> genericpage->objinfo_struct.module_name;
  } else {
    genericpage->objinfo_struct.module_name[0] = '\0';
  }

  if (version >= 19) {
    infile >> genericpage->objinfo_struct.script_name_override;
  } else {
    genericpage->objinfo_struct.script_name_override[0] = '\0';
  }

  int desc = 0;
  {
    uint8_t db = 0;
    infile >> db;
    desc = db;
  }
  if (desc) {
    // Read description if there is one
    std::string tempbuf;
    infile >> tempbuf;
    size_t slen = tempbuf.size() + 1;

    genericpage->objinfo_struct.description = mem_rmalloc<char>(slen);
    Q_ASSERT(genericpage->objinfo_struct.description);
    std::memcpy(genericpage->objinfo_struct.description, tempbuf.data(), slen);
  } else
    genericpage->objinfo_struct.description = NULL;

  // Read icon name
  infile >> genericpage->objinfo_struct.icon_name;

  // Read LOD distances
  infile >> genericpage->objinfo_struct.med_lod_distance;
  infile >> genericpage->objinfo_struct.lo_lod_distance;

  // Read physics stuff
  mng_ReadPhysicsChunk(&genericpage->objinfo_struct.phys_info, infile);

  // Read size
  infile >> genericpage->objinfo_struct.size;

  // Read light info
  mng_ReadLightingChunk(&genericpage->objinfo_struct.lighting_info, infile);

  // Read hit points
  infile >> genericpage->objinfo_struct.hit_points;

  // Read flags (stored as a raw 32-bit value on disk; copy into the packed
  // bitfield preserving its layout).
  uint32_t raw_flags = 0;
  infile >> raw_flags;
  std::memcpy(&genericpage->objinfo_struct.flags, &raw_flags, sizeof(raw_flags));

  // Read AI info
  infile >> genericpage->ai_info.flags;
  {
    int8_t b = 0;
    infile >> b;
    genericpage->ai_info.ai_class = b;
  }
  {
    int8_t b = 0;
    infile >> b;
    genericpage->ai_info.ai_type = b;
  }
  {
    int8_t b = 0;
    infile >> b;
    genericpage->ai_info.movement_type = b;
  }
  {
    int8_t b = 0;
    infile >> b;
    genericpage->ai_info.movement_subtype = b;
  }
  infile >> genericpage->ai_info.fov;

  infile >> genericpage->ai_info.max_velocity;
  infile >> genericpage->ai_info.max_delta_velocity;
  infile >> genericpage->ai_info.max_turn_rate;

  // Makes sure there are no bugs as things are added and removed  -- ask chris
  genericpage->ai_info.notify_flags &= ~AI_NOTIFIES_ALWAYS_ON;
  infile >> genericpage->ai_info.notify_flags;
  genericpage->ai_info.notify_flags |= AI_NOTIFIES_ALWAYS_ON;

  infile >> genericpage->ai_info.max_delta_turn_rate;
  infile >> genericpage->ai_info.circle_distance;
  infile >> genericpage->ai_info.attack_vel_percent;
  infile >> genericpage->ai_info.dodge_percent;
  infile >> genericpage->ai_info.dodge_vel_percent;
  infile >> genericpage->ai_info.flee_vel_percent;
  infile >> genericpage->ai_info.melee_damage[0];
  infile >> genericpage->ai_info.melee_damage[1];
  infile >> genericpage->ai_info.melee_latency[0];
  infile >> genericpage->ai_info.melee_latency[1];

  infile >> genericpage->ai_info.curiousity;
  infile >> genericpage->ai_info.night_vision;
  infile >> genericpage->ai_info.fog_vision;
  infile >> genericpage->ai_info.lead_accuracy;
  infile >> genericpage->ai_info.lead_varience;
  infile >> genericpage->ai_info.fire_spread;
  infile >> genericpage->ai_info.fight_team;
  infile >> genericpage->ai_info.fight_same;
  infile >> genericpage->ai_info.aggression;
  infile >> genericpage->ai_info.hearing;
  infile >> genericpage->ai_info.frustration;
  infile >> genericpage->ai_info.roaming;
  infile >> genericpage->ai_info.life_preservation;

  if (version >= 16) {
    infile >> genericpage->ai_info.avoid_friends_distance;
  } else if (genericpage->objinfo_struct.flags.uses_physics && genericpage->ai_info.max_velocity > 0.0f) {
    genericpage->ai_info.flags |= AIF_AUTO_AVOID_FRIENDS;
    genericpage->ai_info.avoid_friends_distance = genericpage->ai_info.circle_distance / 10.f;
    if (genericpage->ai_info.avoid_friends_distance < 4.0f)
      genericpage->ai_info.avoid_friends_distance = 4.0f;
  } else {
    genericpage->ai_info.avoid_friends_distance = 4.0f;
  }

  if (version >= 17) {
    infile >> genericpage->ai_info.biased_flight_importance;
    infile >> genericpage->ai_info.biased_flight_min;
    infile >> genericpage->ai_info.biased_flight_max;
  } else {
    genericpage->ai_info.biased_flight_importance = .5f;
    genericpage->ai_info.biased_flight_min = 10.0f;
    genericpage->ai_info.biased_flight_max = 50.0f;
  }

  // Read out objects spewed
  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    infile >> genericpage->objinfo_struct.f_dspew;
    infile >> genericpage->objinfo_struct.dspew_percent[i];
    infile >> genericpage->objinfo_struct.dspew_number[i];

    // Read spew name
    infile >> genericpage->dspew_name[i];
  }

  // Read out animation info
  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
      if (version < 20) {
        uint8_t f = 0, t = 0;
        infile >> f;
        infile >> t;
        genericpage->anim[i].elem[j].from = f;
        genericpage->anim[i].elem[j].to = t;
      } else {
        int16_t f = 0, t = 0;
        infile >> f;
        infile >> t;
        genericpage->anim[i].elem[j].from = f;
        genericpage->anim[i].elem[j].to = t;
      }
      infile >> genericpage->anim[i].elem[j].spc;
    }
  }

  // read weapon batteries
  for (i = 0; i < MAX_WBS_PER_OBJ; i++) {
    if (version >= 15)
      mng_ReadWeaponBatteryChunk(&genericpage->static_wb[i], infile, 2);
    else
      mng_ReadWeaponBatteryChunk(&genericpage->static_wb[i], infile, 1);
  }

  // read weapon names
  for (i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (j = 0; j < MAX_WB_GUNPOINTS; j++)
      infile >> genericpage->weapon_name[i][j];
  }

  // read sounds
  Q_ASSERT(MAX_OBJ_SOUNDS == 2);
  for (i = 0; i < MAX_OBJ_SOUNDS; i++)
    infile >> genericpage->sound_name[i];
  if (version < 26) { // used to be three sounds
    std::string temp_sound_name;
    infile >> temp_sound_name;
  }

  for (i = 0; i < MAX_AI_SOUNDS; i++)
    infile >> genericpage->ai_sound_name[i];

  for (i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      infile >> genericpage->fire_sound_name[i][j];
  }

  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (j = 0; j < NUM_ANIMS_PER_CLASS; j++)
      infile >> genericpage->anim_sound_name[i][j];
  }

  // Read respawn scalar
  if (version >= 21)
    infile >> genericpage->objinfo_struct.respawn_scalar;
  else
    genericpage->objinfo_struct.respawn_scalar = 1.0;

  if (version >= 22) {
    int16_t n = 0;
    infile >> n;
    int n_death_types = n;
    for (i = 0; i < n_death_types; i++)
    {
      uint32_t flags = 0;
      infile >> flags;
      if (version == 22) { // translate death flags
        Q_ASSERT(false);            // this version no longer supported
      }

      memcpy(&genericpage->objinfo_struct.death_types[i].flags, &flags, sizeof(uint32_t));
      infile >> genericpage->objinfo_struct.death_types[i].delay_min;
      infile >> genericpage->objinfo_struct.death_types[i].delay_max;
      infile >> genericpage->objinfo_struct.death_probabilities[i];

      // Fix up for changed flags
      if (version < 27) {
        if ((flags & OLD_DF_DELAY_MASK) != OLD_DF_DELAY_MIN_MAX) {
          genericpage->objinfo_struct.death_types[i].delay_min = 0.0;
          genericpage->objinfo_struct.death_types[i].delay_max = 0.0;
        }
        flags &= ~DF_UNUSED;
      }
    }
  }

  // Set score from hitpoints if old version
  if (version < 24) {
    if ((genericpage->objinfo_struct.type == OBJ_ROBOT) ||
        (genericpage->objinfo_struct.type == OBJ_BUILDING && genericpage->objinfo_struct.flags.control_ai))
      if (genericpage->objinfo_struct.flags.destroyable)
        genericpage->objinfo_struct.score = 3 * genericpage->objinfo_struct.hit_points;
  }

  Q_ASSERT(genericpage->objinfo_struct.type != OBJ_NONE);

  return 1; // successfully read
}

// Reads a generic page from an open file.  Returns 0 on error.
int mng_ReadGenericPage(posix_istream &infile, mngs_generic_page *genericpage) {
  if (!Old_table_method)
    return mng_ReadNewGenericPage(infile, genericpage);
  return 0; // old command-based table method not supported in mini build
}
