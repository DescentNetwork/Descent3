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

// Ported from the full engine's manage/ module (manage.cpp, generic.cpp,
// shippage.cpp, weaponpage.cpp, texpage.cpp, soundpage.cpp, doorpage.cpp,
// megapage.cpp).  Reads the D3 page-table metadata from Table.gam (inside
// d3.hog) into the mini editor's global arrays without touching the game
// renderer (no model/bitmap/sound/procedural loading).  This file links only
// against Qt + OpenGL + the mini cfile implementation.

#include "gamedata_loader.h"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <QtGlobal>

#include "manage.h"
#include "crossplat.h" // stricmp
#include "aistruct.h"  // t_ai_info
#include "aistruct_external.h"
#include "mem.h"     // mem_rmalloc
#include "log.h"     // LOG_ERROR
#include "objinfo.h"
#include "ship.h"
#include "weapon.h"
#include "gametexture.h"
#include "door.h"
#include "ssl_lib.h" // sound_info
#include "object_external_struct.h" // physics_info, light_info, MAX_OBJECTS
#include "genericpage.h"
#include "shippage.h"
#include "weaponpage.h"
#include "texpage.h"
#include "soundpage.h"
#include "doorpage.h"
#include "megacell.h"
#include "iff.h"
#include "bitmap.h"

// Old delay types (originally #defined locally in the full-engine generic.cpp)
#ifndef OLD_DF_DELAY_MIN_MAX
#define OLD_DF_DELAY_MIN_MAX 0x0000001
#endif
#ifndef OLD_DF_DELAY_MASK
#define OLD_DF_DELAY_MASK 0x0000003
#endif

// The mini manage.h declares this extern but no mini source defines it.  Net
// tables never use the old command-based method, so it is always 0 here.  A
// single translation-unit definition satisfies the extern (ODR) requirement.
int Old_table_method = 0;

// The mini tree defines `object_info Object_info[MAX_OBJECTS];` in stubs.cpp
// but no header declares the count; declare it here so the loader can track it.
extern int Num_objects;

//-----------------------------------------------------------------------------
// Chunk readers (used by the generic + weapon + ship readers).  Pure data.
//-----------------------------------------------------------------------------

void mng_ReadPhysicsChunk(physics_info *phys_info, CFILE *infile) {
  phys_info->mass = cf_ReadFloat(infile);
  phys_info->drag = cf_ReadFloat(infile);
  phys_info->full_thrust = cf_ReadFloat(infile);
  phys_info->flags = cf_ReadInt(infile);
  phys_info->rotdrag = cf_ReadFloat(infile);
  phys_info->full_rotthrust = cf_ReadFloat(infile);
  phys_info->num_bounces = cf_ReadInt(infile);
  phys_info->velocity.z() = cf_ReadFloat(infile);
  phys_info->rotvel = {cf_ReadFloat(infile), cf_ReadFloat(infile), cf_ReadFloat(infile)};
  phys_info->wiggle_amplitude = cf_ReadFloat(infile);
  phys_info->wiggles_per_sec = cf_ReadFloat(infile);
  phys_info->coeff_restitution = cf_ReadFloat(infile);
  phys_info->hit_die_dot = cf_ReadFloat(infile);
  phys_info->max_turnroll_rate = cf_ReadFloat(infile);
  phys_info->turnroll_ratio = cf_ReadFloat(infile);
}

void mng_ReadWeaponBatteryChunk(otype_wb_info *static_wb, CFILE *infile, int version) {
  int j;

  static_wb->energy_usage = cf_ReadFloat(infile);
  static_wb->ammo_usage = cf_ReadFloat(infile);

  for (j = 0; j < MAX_WB_GUNPOINTS; j++) {
    static_wb->gp_weapon_index[j] = cf_ReadShort(infile);
  }

  for (j = 0; j < MAX_WB_FIRING_MASKS; j++) {
    static_wb->gp_fire_masks[j] = cf_ReadByte(infile);
    static_wb->gp_fire_wait[j] = cf_ReadFloat(infile);
    static_wb->anim_time[j] = cf_ReadFloat(infile);
    static_wb->anim_start_frame[j] = cf_ReadFloat(infile);
    static_wb->anim_fire_frame[j] = cf_ReadFloat(infile);
    static_wb->anim_end_frame[j] = cf_ReadFloat(infile);
  }
  static_wb->num_masks = cf_ReadByte(infile);
  static_wb->aiming_gp_index = cf_ReadShort(infile);
  static_wb->aiming_flags = cf_ReadByte(infile);
  static_wb->aiming_3d_dot = cf_ReadFloat(infile);
  static_wb->aiming_3d_dist = cf_ReadFloat(infile);
  static_wb->aiming_XZ_dot = cf_ReadFloat(infile);
  if (version >= 2)
    static_wb->flags = cf_ReadShort(infile);
  else
    static_wb->flags = cf_ReadByte(infile);
  static_wb->gp_quad_fire_mask = cf_ReadByte(infile);
}

static void mng_ReadLightingChunk(light_info *lighting_info, CFILE *infile) {
  lighting_info->light_distance = cf_ReadFloat(infile);
  lighting_info->red_light1 = cf_ReadFloat(infile);
  lighting_info->green_light1 = cf_ReadFloat(infile);
  lighting_info->blue_light1 = cf_ReadFloat(infile);
  lighting_info->time_interval = cf_ReadFloat(infile);
  lighting_info->flicker_distance = cf_ReadFloat(infile);
  lighting_info->directional_dot = cf_ReadFloat(infile);
  lighting_info->red_light2 = cf_ReadFloat(infile);
  lighting_info->green_light2 = cf_ReadFloat(infile);
  lighting_info->blue_light2 = cf_ReadFloat(infile);
  lighting_info->flags = cf_ReadInt(infile);
  lighting_info->timebits = cf_ReadInt(infile);
  lighting_info->angle = cf_ReadByte(infile);
  lighting_info->lighting_render_type = cf_ReadByte(infile);
}

//-----------------------------------------------------------------------------
// Generic page (manage/generic.cpp : 448-1337)
//-----------------------------------------------------------------------------

static void mng_InitGenericPage(mngs_generic_page *genericpage) {
  int i;

  memset(genericpage, 0, sizeof(mngs_generic_page));
  strcpy(genericpage->image_name, "");

  strcpy(genericpage->med_image_name, "");
  strcpy(genericpage->lo_image_name, "");

  for (i = 0; i < MAX_OBJ_SOUNDS; i++)
    strcpy(genericpage->sound_name[i], "");

  for (i = 0; i < MAX_AI_SOUNDS; i++)
    strcpy(genericpage->ai_sound_name[i], "");

  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    strcpy(genericpage->dspew_name[i], "\0");
  }

  genericpage->objinfo_struct.description = NULL;
  genericpage->objinfo_struct.icon_name[0] = '\0';

  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++)
    for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++)
      strcpy(genericpage->anim_sound_name[i][j], "");

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
    genericpage->objinfo_struct.death_types[i].flags = 0;
    genericpage->objinfo_struct.death_types[i].delay_min = 0.0;
    genericpage->objinfo_struct.death_types[i].delay_max = 0.0;
    genericpage->objinfo_struct.death_probabilities[i] = 0;
  }
}

static void GenericPageSetPowerupDefaultAmmo(object_info *ip) {
  // Default is zero
  ip->ammo_count = 0;

  // Set for specific types
  if (!stricmp(ip->name, "Vauss"))
    ip->ammo_count = 5000;
  if (!stricmp(ip->name, "Napalm"))
    ip->ammo_count = 500;
  if (!stricmp(ip->name, "MassDriver"))
    ip->ammo_count = 20;

  if (!stricmp(ip->name, "Frag"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "ImpactMortar"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "NapalmRocket"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Cyclone"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "BlackShark"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Concussion"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Homing"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Smart"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Mega"))
    ip->ammo_count = 1;
  if (!stricmp(ip->name, "Guided"))
    ip->ammo_count = 1;

  if (!stricmp(ip->name, "4PackHoming"))
    ip->ammo_count = 4;
  if (!stricmp(ip->name, "4PackConc"))
    ip->ammo_count = 4;
  if (!stricmp(ip->name, "4PackFrag"))
    ip->ammo_count = 4;
  if (!stricmp(ip->name, "4PackGuided"))
    ip->ammo_count = 4;

  if (!stricmp(ip->name, "Vauss clip"))
    ip->ammo_count = 1250;
  if (!stricmp(ip->name, "MassDriverAmmo"))
    ip->ammo_count = 5;
  if (!stricmp(ip->name, "NapalmTank"))
    ip->ammo_count = 100;
}

// Reads a generic page from an open file.  Returns 0 on error.
int mng_ReadNewGenericPage(CFILE *infile, mngs_generic_page *genericpage) {
  int i, j;

  Q_ASSERT(infile != NULL);
  mng_InitGenericPage(genericpage);

  int version = cf_ReadShort(infile);

  genericpage->objinfo_struct.type = cf_ReadByte(infile);

  // Read object name
  cf_ReadString(genericpage->objinfo_struct.name, PAGENAME_LEN, infile);

  // Read model names
  cf_ReadString(genericpage->image_name, PAGENAME_LEN, infile);
  cf_ReadString(genericpage->med_image_name, PAGENAME_LEN, infile);
  cf_ReadString(genericpage->lo_image_name, PAGENAME_LEN, infile);

  // Read out impact data
  genericpage->objinfo_struct.impact_size = cf_ReadFloat(infile);
  genericpage->objinfo_struct.impact_time = cf_ReadFloat(infile);
  genericpage->objinfo_struct.damage = cf_ReadFloat(infile);

  // Read score
  if (version >= 24)
    genericpage->objinfo_struct.score = cf_ReadShort(infile);
  else
    genericpage->objinfo_struct.score = cf_ReadByte(infile);

  // Read ammo
  if (genericpage->objinfo_struct.type == OBJ_POWERUP) {
    if (version >= 25)
      genericpage->objinfo_struct.ammo_count = cf_ReadShort(infile);
    else
      GenericPageSetPowerupDefaultAmmo(&genericpage->objinfo_struct);
  } else
    genericpage->objinfo_struct.ammo_count = 0;

  // Read script name
  char dummy[256];
  cf_ReadString(dummy, PAGENAME_LEN, infile); // genericpage->objinfo_struct.script_name

  if (version >= 18) {
    cf_ReadString(genericpage->objinfo_struct.module_name, MAX_MODULENAME_LEN, infile);
  } else {
    genericpage->objinfo_struct.module_name[0] = '\0';
  }

  if (version >= 19) {
    cf_ReadString(genericpage->objinfo_struct.script_name_override, PAGENAME_LEN, infile);
  } else {
    genericpage->objinfo_struct.script_name_override[0] = '\0';
  }

  int desc = cf_ReadByte(infile);
  if (desc) {
    // Read description if there is one
    char tempbuf[1024];

    cf_ReadString(tempbuf, 1024, infile);
    size_t slen = strlen(tempbuf) + 1;

    genericpage->objinfo_struct.description = mem_rmalloc<char>(slen);
    Q_ASSERT(genericpage->objinfo_struct.description);
    strcpy(genericpage->objinfo_struct.description, tempbuf);
  } else
    genericpage->objinfo_struct.description = NULL;

  // Read icon name
  cf_ReadString(genericpage->objinfo_struct.icon_name, PAGENAME_LEN, infile);

  // Read LOD distances
  genericpage->objinfo_struct.med_lod_distance = cf_ReadFloat(infile);
  genericpage->objinfo_struct.lo_lod_distance = cf_ReadFloat(infile);

  // Read physics stuff
  mng_ReadPhysicsChunk(&genericpage->objinfo_struct.phys_info, infile);

  // Read size
  genericpage->objinfo_struct.size = cf_ReadFloat(infile);

  // Read light info
  mng_ReadLightingChunk(&genericpage->objinfo_struct.lighting_info, infile);

  // Read hit points
  genericpage->objinfo_struct.hit_points = cf_ReadInt(infile);

  // Read flags
  genericpage->objinfo_struct.flags = cf_ReadInt(infile);

  // Read AI info
  genericpage->ai_info.flags = cf_ReadInt(infile);
  genericpage->ai_info.ai_class = cf_ReadByte(infile);
  genericpage->ai_info.ai_type = cf_ReadByte(infile);
  genericpage->ai_info.movement_type = cf_ReadByte(infile);
  genericpage->ai_info.movement_subtype = cf_ReadByte(infile);
  genericpage->ai_info.fov = cf_ReadFloat(infile);

  genericpage->ai_info.max_velocity = cf_ReadFloat(infile);
  genericpage->ai_info.max_delta_velocity = cf_ReadFloat(infile);
  genericpage->ai_info.max_turn_rate = cf_ReadFloat(infile);

  // Makes sure there are no bugs as things are added and removed  -- ask chris
  genericpage->ai_info.notify_flags &= ~AI_NOTIFIES_ALWAYS_ON;
  genericpage->ai_info.notify_flags = cf_ReadInt(infile);
  genericpage->ai_info.notify_flags |= AI_NOTIFIES_ALWAYS_ON;

  genericpage->ai_info.max_delta_turn_rate = cf_ReadFloat(infile);
  genericpage->ai_info.circle_distance = cf_ReadFloat(infile);
  genericpage->ai_info.attack_vel_percent = cf_ReadFloat(infile);
  genericpage->ai_info.dodge_percent = cf_ReadFloat(infile);
  genericpage->ai_info.dodge_vel_percent = cf_ReadFloat(infile);
  genericpage->ai_info.flee_vel_percent = cf_ReadFloat(infile);
  genericpage->ai_info.melee_damage[0] = cf_ReadFloat(infile);
  genericpage->ai_info.melee_damage[1] = cf_ReadFloat(infile);
  genericpage->ai_info.melee_latency[0] = cf_ReadFloat(infile);
  genericpage->ai_info.melee_latency[1] = cf_ReadFloat(infile);

  genericpage->ai_info.curiousity = cf_ReadFloat(infile);
  genericpage->ai_info.night_vision = cf_ReadFloat(infile);
  genericpage->ai_info.fog_vision = cf_ReadFloat(infile);
  genericpage->ai_info.lead_accuracy = cf_ReadFloat(infile);
  genericpage->ai_info.lead_varience = cf_ReadFloat(infile);
  genericpage->ai_info.fire_spread = cf_ReadFloat(infile);
  genericpage->ai_info.fight_team = cf_ReadFloat(infile);
  genericpage->ai_info.fight_same = cf_ReadFloat(infile);
  genericpage->ai_info.aggression = cf_ReadFloat(infile);
  genericpage->ai_info.hearing = cf_ReadFloat(infile);
  genericpage->ai_info.frustration = cf_ReadFloat(infile);
  genericpage->ai_info.roaming = cf_ReadFloat(infile);
  genericpage->ai_info.life_preservation = cf_ReadFloat(infile);

  if (version >= 16) {
    genericpage->ai_info.avoid_friends_distance = cf_ReadFloat(infile);
  } else if ((genericpage->objinfo_struct.flags | OIF_USES_PHYSICS) && genericpage->ai_info.max_velocity > 0.0f) {
    genericpage->ai_info.flags |= AIF_AUTO_AVOID_FRIENDS;
    genericpage->ai_info.avoid_friends_distance = genericpage->ai_info.circle_distance / 10.f;
    if (genericpage->ai_info.avoid_friends_distance < 4.0f)
      genericpage->ai_info.avoid_friends_distance = 4.0f;
  } else {
    genericpage->ai_info.avoid_friends_distance = 4.0f;
  }

  if (version >= 17) {
    genericpage->ai_info.biased_flight_importance = cf_ReadFloat(infile);
    genericpage->ai_info.biased_flight_min = cf_ReadFloat(infile);
    genericpage->ai_info.biased_flight_max = cf_ReadFloat(infile);
  } else {
    genericpage->ai_info.biased_flight_importance = .5f;
    genericpage->ai_info.biased_flight_min = 10.0f;
    genericpage->ai_info.biased_flight_max = 50.0f;
  }

  // Read out objects spewed
  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    genericpage->objinfo_struct.f_dspew = cf_ReadByte(infile);
    genericpage->objinfo_struct.dspew_percent[i] = cf_ReadFloat(infile);
    genericpage->objinfo_struct.dspew_number[i] = cf_ReadShort(infile);

    // Read spew name
    cf_ReadString(genericpage->dspew_name[i], PAGENAME_LEN, infile);
  }

  // Read out animation info
  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
      if (version < 20) {
        genericpage->anim[i].elem[j].from = cf_ReadByte(infile);
        genericpage->anim[i].elem[j].to = cf_ReadByte(infile);
      } else {
        genericpage->anim[i].elem[j].from = cf_ReadShort(infile);
        genericpage->anim[i].elem[j].to = cf_ReadShort(infile);
      }
      genericpage->anim[i].elem[j].spc = cf_ReadFloat(infile);
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
      cf_ReadString(genericpage->weapon_name[i][j], PAGENAME_LEN, infile);
  }

  // read sounds
  Q_ASSERT(MAX_OBJ_SOUNDS == 2);
  for (i = 0; i < MAX_OBJ_SOUNDS; i++)
    cf_ReadString(genericpage->sound_name[i], PAGENAME_LEN, infile);
  if (version < 26) { // used to be three sounds
    char temp_sound_name[PAGENAME_LEN];
    cf_ReadString(temp_sound_name, PAGENAME_LEN, infile);
  }

  for (i = 0; i < MAX_AI_SOUNDS; i++)
    cf_ReadString(genericpage->ai_sound_name[i], PAGENAME_LEN, infile);

  for (i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      cf_ReadString(genericpage->fire_sound_name[i][j], PAGENAME_LEN, infile);
  }

  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (j = 0; j < NUM_ANIMS_PER_CLASS; j++)
      cf_ReadString(genericpage->anim_sound_name[i][j], PAGENAME_LEN, infile);
  }

  // Read respawn scalar
  if (version >= 21)
    genericpage->objinfo_struct.respawn_scalar = cf_ReadFloat(infile);
  else
    genericpage->objinfo_struct.respawn_scalar = 1.0;

  if (version >= 22) {
    int n_death_types = cf_ReadShort(infile);
    for (i = 0; i < n_death_types; i++) {
      int flags = cf_ReadInt(infile);
      if (version == 22) { // translate death flags
        Q_ASSERT(false);            // this version no longer supported
      }

      genericpage->objinfo_struct.death_types[i].flags = flags;
      genericpage->objinfo_struct.death_types[i].delay_min = cf_ReadFloat(infile);
      genericpage->objinfo_struct.death_types[i].delay_max = cf_ReadFloat(infile);
      genericpage->objinfo_struct.death_probabilities[i] = cf_ReadByte(infile);

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
        (genericpage->objinfo_struct.type == OBJ_BUILDING && (genericpage->objinfo_struct.flags & OIF_CONTROL_AI)))
      if (genericpage->objinfo_struct.flags & OIF_DESTROYABLE)
        genericpage->objinfo_struct.score = 3 * genericpage->objinfo_struct.hit_points;
  }

  Q_ASSERT(genericpage->objinfo_struct.type != OBJ_NONE);

  return 1; // successfully read
}

// Reads a generic page from an open file.  Returns 0 on error.
int mng_ReadGenericPage(CFILE *infile, mngs_generic_page *genericpage) {
  if (!Old_table_method)
    return mng_ReadNewGenericPage(infile, genericpage);
  return 0; // old command-based table method not supported in mini build
}

//-----------------------------------------------------------------------------
// Weapon page (ported from weaponpage.cpp : 382-413, 782-901)
//-----------------------------------------------------------------------------

static void mng_InitWeaponPage(mngs_weapon_page *weaponpage) {
  int i;

  memset(weaponpage, 0, sizeof(mngs_weapon_page));
  strcpy(weaponpage->hud_image_name, "");
  strcpy(weaponpage->fire_image_name, "");
  strcpy(weaponpage->explode_image_name, "");
  strcpy(weaponpage->spawn_name, "");
  strcpy(weaponpage->alternate_spawn_name, "");
  strcpy(weaponpage->robot_spawn_name, "");
  strcpy(weaponpage->smoke_image_name, "");
  strcpy(weaponpage->scorch_image_name, "");
  strcpy(weaponpage->icon_name, "");
  strcpy(weaponpage->particle_name, "");

  weaponpage->weapon_struct.alpha = 1.0;
  weaponpage->weapon_struct.alternate_chance = 0;
  weaponpage->weapon_struct.explode_time = 1.0;
  weaponpage->weapon_struct.explode_size = 1.0;
  weaponpage->weapon_struct.particle_count = 0;
  weaponpage->weapon_struct.scorch_size = 1.0;
  weaponpage->weapon_struct.terrain_damage_size = 0;
  weaponpage->weapon_struct.terrain_damage_depth = 0;
  weaponpage->weapon_struct.homing_fov = 0.4f;
  weaponpage->weapon_struct.custom_size = 0.0f;
  weaponpage->weapon_struct.recoil_force = 0.0f;

  weaponpage->weapon_struct.phys_info.hit_die_dot = 1.0f;

  for (i = 0; i < MAX_WEAPON_SOUNDS; i++)
    strcpy(weaponpage->sound_name[i], "");
}

int mng_ReadNewWeaponPage(CFILE *infile, mngs_weapon_page *weaponpage) {
  int i;

  mng_InitWeaponPage(weaponpage);

  int version = cf_ReadShort(infile);

  cf_ReadString(weaponpage->weapon_struct.name, PAGENAME_LEN, infile);

  // Read hud image name
  cf_ReadString(weaponpage->hud_image_name, PAGENAME_LEN, infile);

  // Read fire image
  cf_ReadString(weaponpage->fire_image_name, PAGENAME_LEN, infile);

  // Read particle data
  cf_ReadString(weaponpage->particle_name, PAGENAME_LEN, infile);

  weaponpage->weapon_struct.particle_count = cf_ReadByte(infile);
  weaponpage->weapon_struct.particle_life = cf_ReadFloat(infile);
  weaponpage->weapon_struct.particle_size = cf_ReadFloat(infile);

  // Read flags
  weaponpage->weapon_struct.flags = cf_ReadInt(infile);

  // Read spawn data
  cf_ReadString(weaponpage->spawn_name, PAGENAME_LEN, infile);
  weaponpage->weapon_struct.spawn_count = cf_ReadByte(infile);

  cf_ReadString(weaponpage->robot_spawn_name, PAGENAME_LEN, infile);
  cf_ReadString(weaponpage->alternate_spawn_name, PAGENAME_LEN, infile);

  weaponpage->weapon_struct.alternate_chance = cf_ReadByte(infile);

  // Read gravity stuff
  weaponpage->weapon_struct.gravity_time = cf_ReadFloat(infile);
  weaponpage->weapon_struct.gravity_size = cf_ReadFloat(infile);

  // Read size and homing data
  weaponpage->weapon_struct.homing_fov = cf_ReadFloat(infile);
  weaponpage->weapon_struct.custom_size = cf_ReadFloat(infile);
  weaponpage->weapon_struct.size = cf_ReadFloat(infile);
  weaponpage->weapon_struct.thrust_time = cf_ReadFloat(infile);

  // Read physics info
  mng_ReadPhysicsChunk(&weaponpage->weapon_struct.phys_info, infile);

  // Read terrain damage
  weaponpage->weapon_struct.terrain_damage_size = cf_ReadFloat(infile);
  weaponpage->weapon_struct.terrain_damage_depth = cf_ReadByte(infile);

  // Read alpha
  weaponpage->weapon_struct.alpha = cf_ReadFloat(infile);

  // Read explosion data
  cf_ReadString(weaponpage->explode_image_name, PAGENAME_LEN, infile);
  weaponpage->weapon_struct.explode_time = cf_ReadFloat(infile);
  weaponpage->weapon_struct.explode_size = cf_ReadFloat(infile);

  // Read damage data
  weaponpage->weapon_struct.player_damage = cf_ReadFloat(infile);

  if (version >= 7)
    weaponpage->weapon_struct.generic_damage = cf_ReadFloat(infile);
  else
    weaponpage->weapon_struct.generic_damage = weaponpage->weapon_struct.player_damage;

  weaponpage->weapon_struct.impact_size = cf_ReadFloat(infile);
  weaponpage->weapon_struct.impact_time = cf_ReadFloat(infile);
  weaponpage->weapon_struct.impact_player_damage = cf_ReadFloat(infile);

  if (version >= 7)
    weaponpage->weapon_struct.impact_generic_damage = cf_ReadFloat(infile);
  else
    weaponpage->weapon_struct.impact_generic_damage = weaponpage->weapon_struct.impact_player_damage;

  weaponpage->weapon_struct.impact_force = cf_ReadFloat(infile);

  // Read lifetime
  weaponpage->weapon_struct.life_time = cf_ReadFloat(infile);

  // read lighting
  mng_ReadLightingChunk(&weaponpage->weapon_struct.lighting_info, infile);

  // read recoil force
  if (version >= 8)
    weaponpage->weapon_struct.recoil_force = cf_ReadFloat(infile);
  else
    weaponpage->weapon_struct.recoil_force = 0.0f;

  // Read its sound names
  for (i = 0; i < MAX_WEAPON_SOUNDS; i++)
    cf_ReadString(weaponpage->sound_name[i], PAGENAME_LEN, infile);

  // Read smoke name
  cf_ReadString(weaponpage->smoke_image_name, PAGENAME_LEN, infile);

  // Read scorch data
  cf_ReadString(weaponpage->scorch_image_name, PAGENAME_LEN, infile);
  weaponpage->weapon_struct.scorch_size = cf_ReadFloat(infile);

  // Read icon name
  cf_ReadString(weaponpage->icon_name, PAGENAME_LEN, infile);

  weaponpage->weapon_struct.used = 1;

  // OEM hack was omitted (OEM is never defined in the mini build).

  if (!stricmp(weaponpage->weapon_struct.name, "EMDBlob")) {
    weaponpage->weapon_struct.life_time = 1.7f;
  }

  return 1; // successfully read
}

// Reads a weapon page from an open file.  Returns 0 on error.
int mng_ReadWeaponPage(CFILE *infile, mngs_weapon_page *weaponpage) {
  if (!Old_table_method)
    return mng_ReadNewWeaponPage(infile, weaponpage);
  return 0; // old command-based method not supported in mini build
}

//-----------------------------------------------------------------------------
// Texture page (ported from texpage.cpp : 508-759)
//-----------------------------------------------------------------------------

static void mng_InitTexturePage(mngs_texture_page *texpage) {
  memset(texpage, 0, sizeof(mngs_texture_page));
  texpage->proc_thickness = 4;
  texpage->proc_heat = 200;
  texpage->proc_light = 1;
  texpage->num_proc_elements = 0;
  texpage->proc_evaluation_time = 0;
  texpage->osc_time = 0;
  texpage->osc_value = 8;

  texpage->tex_struct.alpha = 1.0;
  texpage->tex_struct.speed = 1.0;
  texpage->tex_struct.reflectivity = .5;
  texpage->tex_struct.corona_type = 0;
  texpage->tex_struct.slide_v = 0;
  texpage->tex_struct.slide_u = 0;
  texpage->tex_struct.bumpmap = -1;
  texpage->tex_struct.sound = -1;
  texpage->tex_struct.sound_volume = 1.0;

  strcpy(texpage->bitmap_name, "");
  strcpy(texpage->destroy_name, "");
  strcpy(texpage->sound_name, "");
}

// Reads a texture page from an open file.  Returns 0 on error.
int mng_ReadNewTexturePage(CFILE *infile, mngs_texture_page *texpage) {
  int i;

  Q_ASSERT(infile != NULL);
  mng_InitTexturePage(texpage);

  int version = cf_ReadShort(infile);

  cf_ReadString(texpage->tex_struct.name, PAGENAME_LEN, infile);
  cf_ReadString(texpage->bitmap_name, PAGENAME_LEN, infile);
  cf_ReadString(texpage->destroy_name, PAGENAME_LEN, infile);

  texpage->tex_struct.r = cf_ReadFloat(infile);
  texpage->tex_struct.g = cf_ReadFloat(infile);
  texpage->tex_struct.b = cf_ReadFloat(infile);
  texpage->tex_struct.alpha = cf_ReadFloat(infile);

  texpage->tex_struct.speed = cf_ReadFloat(infile);

  texpage->tex_struct.slide_u = cf_ReadFloat(infile);
  texpage->tex_struct.slide_v = cf_ReadFloat(infile);
  texpage->tex_struct.reflectivity = cf_ReadFloat(infile);

  texpage->tex_struct.corona_type = cf_ReadByte(infile);
  texpage->tex_struct.damage = cf_ReadInt(infile);
  texpage->tex_struct.flags = cf_ReadInt(infile);

  if (texpage->tex_struct.flags & TF_PROCEDURAL) {
    for (i = 0; i < 255; i++) {
      uint16_t val = cf_ReadShort(infile);
      texpage->proc_palette[i] = val;
    }

    texpage->proc_heat = cf_ReadByte(infile);
    texpage->proc_light = cf_ReadByte(infile);
    texpage->proc_thickness = cf_ReadByte(infile);
    texpage->proc_evaluation_time = cf_ReadFloat(infile);

    if (version >= 6) {
      texpage->osc_time = cf_ReadFloat(infile);
      texpage->osc_value = cf_ReadByte(infile);
    }

    texpage->num_proc_elements = cf_ReadShort(infile);

    if (texpage->num_proc_elements > MAX_PROC_ELEMENTS) {
      LOG_ERROR("Warning! Too many procedural elements!");
      Q_ASSERT(false);
    }

    for (i = 0; i < texpage->num_proc_elements; i++) {
      texpage->proc_type[i] = cf_ReadByte(infile);
      texpage->proc_frequency[i] = cf_ReadByte(infile);
      texpage->proc_speed[i] = cf_ReadByte(infile);
      texpage->proc_size[i] = cf_ReadByte(infile);
      texpage->proc_x1[i] = cf_ReadByte(infile);
      texpage->proc_y1[i] = cf_ReadByte(infile);

      texpage->proc_x2[i] = cf_ReadByte(infile);
      texpage->proc_y2[i] = cf_ReadByte(infile);
    }
  }

  if (texpage->tex_struct.flags & TF_PROCEDURAL) {
    if (texpage->num_proc_elements == 0)
      texpage->tex_struct.flags &= ~TF_PROCEDURAL;
  }

  if (!strnicmp(texpage->destroy_name, "INVALID", 7))
    strcpy(texpage->destroy_name, "");

  if (version >= 5) {

    if (version < 7) {
      // Kill buggy version of sound resolving code
      texpage->tex_struct.sound = cf_ReadInt(infile);
      texpage->tex_struct.sound = -1;
      strcpy(texpage->sound_name, "");
    } else
      cf_ReadString(texpage->sound_name, PAGENAME_LEN, infile);

    texpage->tex_struct.sound_volume = cf_ReadFloat(infile);
  } else {
    texpage->tex_struct.sound = -1;
    texpage->tex_struct.sound_volume = 1.0;
  }

  texpage->tex_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadTexturePage(CFILE *infile, mngs_texture_page *texpage) {
  if (!Old_table_method)
    return mng_ReadNewTexturePage(infile, texpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Sound page (ported from soundpage.cpp : 222-255)
//-----------------------------------------------------------------------------

int mng_ReadNewSoundPage(CFILE *infile, mngs_sound_page *soundpage) {
  Q_ASSERT(infile != NULL);
  /* int version = */ cf_ReadShort(infile);
  // read in name,rawfile name
  cf_ReadString(soundpage->sound_struct.name, PAGENAME_LEN, infile);
  cf_ReadString(soundpage->raw_name, PAGENAME_LEN, infile);
  soundpage->sound_struct.flags = cf_ReadInt(infile);

  soundpage->sound_struct.loop_start = cf_ReadInt(infile);
  soundpage->sound_struct.loop_end = cf_ReadInt(infile);
  soundpage->sound_struct.outer_cone_volume = cf_ReadFloat(infile);
  soundpage->sound_struct.inner_cone_angle = cf_ReadInt(infile);
  soundpage->sound_struct.outer_cone_angle = cf_ReadInt(infile);
  soundpage->sound_struct.max_distance = cf_ReadFloat(infile);
  soundpage->sound_struct.min_distance = cf_ReadFloat(infile);
  soundpage->sound_struct.import_volume = cf_ReadFloat(infile);
  // The full engine has a DEMO-only import_volume adjustment block here; it is
  // compiled out unless DEMO is defined (never in the mini build).

  // This is a valid new page
  soundpage->sound_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadSoundPage(CFILE *infile, mngs_sound_page *soundpage) {
  if (!Old_table_method)
    return mng_ReadNewSoundPage(infile, soundpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Door page (ported from doorpage.cpp : 261-292)
//-----------------------------------------------------------------------------

int mng_ReadNewDoorPage(CFILE *infile, mngs_door_page *doorpage) {
  Q_ASSERT(infile != NULL);

  int version = cf_ReadShort(infile);

  cf_ReadString(doorpage->door_struct.name, PAGENAME_LEN, infile);
  cf_ReadString(doorpage->image_name, PAGENAME_LEN, infile);

  doorpage->door_struct.total_open_time = cf_ReadFloat(infile);
  doorpage->door_struct.total_close_time = cf_ReadFloat(infile);
  doorpage->door_struct.total_time_open = cf_ReadFloat(infile);

  doorpage->door_struct.flags = cf_ReadByte(infile);

  if (version >= 3)
    doorpage->door_struct.hit_points = cf_ReadShort(infile);
  else
    doorpage->door_struct.hit_points = 0;

  cf_ReadString(doorpage->open_sound_name, PAGENAME_LEN, infile);
  cf_ReadString(doorpage->close_sound_name, PAGENAME_LEN, infile);

  if (version >= 2)
    cf_ReadString(doorpage->door_struct.module_name, MAX_MODULENAME_LEN, infile);
  else
    doorpage->door_struct.module_name[0] = '\0';

  // This is a valid new page
  doorpage->door_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadDoorPage(CFILE *infile, mngs_door_page *doorpage) {
  if (!Old_table_method)
    return mng_ReadNewDoorPage(infile, doorpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Megacell page (ported from megapage.cpp : 175-192)
//-----------------------------------------------------------------------------

int mng_ReadNewMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage) {
  int i;
  Q_ASSERT(infile != NULL);
  memset(megacellpage, 0, sizeof(mngs_megacell_page));
  /* int version = */ cf_ReadShort(infile);

  cf_ReadString(megacellpage->megacell_struct.name, PAGENAME_LEN, infile);

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    cf_ReadString(megacellpage->cellname[i], PAGENAME_LEN, infile);

  megacellpage->megacell_struct.width = cf_ReadByte(infile);
  megacellpage->megacell_struct.height = cf_ReadByte(infile);
  // This is a valid new page
  megacellpage->megacell_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage) {
  if (!Old_table_method)
    return mng_ReadNewMegacellPage(infile, megacellpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Ship page (ported from shippage.cpp : 539-611)
//-----------------------------------------------------------------------------

int mng_ReadNewShipPage(CFILE *infile, mngs_ship_page *shippage) {
  int i, j;

  Q_ASSERT(infile != NULL);

  // Defaults
  memset(shippage, 0, sizeof(mngs_ship_page));

  int version = cf_ReadShort(infile);

  // Read In misc names
  cf_ReadString(shippage->ship_struct.name, PAGENAME_LEN, infile);
  cf_ReadString(shippage->ship_struct.cockpit_name, PAGENAME_LEN, infile);
  cf_ReadString(shippage->ship_struct.hud_config_name, PAGENAME_LEN, infile);

  // Read in model names
  cf_ReadString(shippage->image_name, PAGENAME_LEN, infile);
  cf_ReadString(shippage->dying_image_name, PAGENAME_LEN, infile);
  cf_ReadString(shippage->med_image_name, PAGENAME_LEN, infile);
  cf_ReadString(shippage->lo_image_name, PAGENAME_LEN, infile);

  // read lod distance
  shippage->ship_struct.med_lod_distance = cf_ReadFloat(infile);
  shippage->ship_struct.lo_lod_distance = cf_ReadFloat(infile);

  // Read physics
  mng_ReadPhysicsChunk(&shippage->ship_struct.phys_info, infile);

  shippage->ship_struct.size = cf_ReadFloat(infile);
  shippage->ship_struct.armor_scalar = cf_ReadFloat(infile);
  shippage->ship_struct.flags = cf_ReadInt(infile);

  for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    shippage->ship_struct.fire_flags[i] = cf_ReadByte(infile);
    cf_ReadString(shippage->firing_sound_name[i], PAGENAME_LEN, infile);
    cf_ReadString(shippage->release_sound_name[i], PAGENAME_LEN, infile);
    cf_ReadString(shippage->spew_powerup_name[i], PAGENAME_LEN, infile);
    shippage->ship_struct.max_ammo[i] = cf_ReadInt(infile);

    if (version >= 6)
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 2);
    else
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 1);

    for (j = 0; j < MAX_WB_GUNPOINTS; j++)
      cf_ReadString(shippage->fire_sound_name[i][j], PAGENAME_LEN, infile);

    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      cf_ReadString(shippage->weapon_name[i][j], PAGENAME_LEN, infile);
  }

  // Mark the newly filled structure as used
  shippage->ship_struct.used = 1;

  // Bash Fusion recharge times for the ships
  if (!stricmp(shippage->ship_struct.name, "Pyro-GL")) {
    // Pyro-GL
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.66f;
  } else if (!stricmp(shippage->ship_struct.name, "Phoenix")) {
    // Phoenix
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.792f;
  } else if (!stricmp(shippage->ship_struct.name, "Magnum-AHT")) {
    // Magnum
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 1.122f;
  } else {
    // Other
  }

  return 1; // successfully read
}

int mng_ReadShipPage(CFILE *infile, mngs_ship_page *shippage) {
  if (!Old_table_method)
    return mng_ReadNewShipPage(infile, shippage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Top-level loader
//-----------------------------------------------------------------------------

// Discards exactly `count` bytes from an open page file (used to skip pages
// whose bodies are not consumed by a reader, e.g. game-only ROBOT/POWERUP
// pages or pages that overflow a global array).
static void discardBytes(CFILE *infile, int count) {
  uint8_t buf[4096];
  while (count > 0) {
    int take = count > (int)sizeof(buf) ? (int)sizeof(buf) : count;
    if (cf_ReadBytes(buf, take, infile) < take)
      break;
    count -= take;
  }
}

bool loadGameDataTable(const std::filesystem::path &d3HogPath) {
  // Point cfile at the directory that holds d3.hog, then open the hog.
  cf_AddBaseDirectory(d3HogPath.parent_path());
  int hog = cf_OpenLibrary(d3HogPath.filename());
  if (!hog)
    return false;

  CFILE *infile = cfopen("table.gam", "rb");
  if (!infile) {
    cf_CloseLibrary(hog);
    return false;
  }

  // Always read new-style (net) pages.
  Old_table_method = 0;

  // local page containers
  mngs_generic_page genericpage;
  mngs_ship_page shippage;
  mngs_weapon_page weaponpage;
  mngs_texture_page texpage;
  mngs_sound_page soundpage;
  mngs_door_page doorpage;
  mngs_megacell_page megacellpage;

  bool ok = true;
  while (!cfeof(infile)) {
    uint8_t pagetype = cf_ReadByte(infile);
    int32_t len = cf_ReadInt(infile);

    switch (pagetype) {
    case PAGETYPE_TEXTURE:
      if (Num_textures < MAX_TEXTURES) {
        if (!mng_ReadNewTexturePage(infile, &texpage))
          ok = false;
        GameTextures[Num_textures] = texpage.tex_struct;
        strcpy(GameTextures[Num_textures].name, texpage.tex_struct.name);
        // Load the texture's image so textured faces render: resolves inside the
        // open d3.hog library (which cfopen sees while loadGameDataTable runs).
        GameTextures[Num_textures].bm_handle = -1;
        const char *img = texpage.bitmap_name;
        if (img && img[0] != '\0') {
          int bm = bm_AllocLoadFileBitmap(img, 0, BITMAP_FORMAT_1555);
          if (bm >= 0)
            GameTextures[Num_textures].bm_handle = bm;
        }
        Num_textures++;
      } else {
        // Metadata only: we do NOT load bitmaps/procedurals, so just discard.
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_WEAPON:
      if (Num_weapons < MAX_WEAPONS) {
        if (!mng_ReadNewWeaponPage(infile, &weaponpage))
          ok = false;
        Weapons[Num_weapons] = weaponpage.weapon_struct;
        Num_weapons++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_DOOR:
      if (Num_doors < MAX_DOORS) {
        if (!mng_ReadNewDoorPage(infile, &doorpage))
          ok = false;
        Doors[Num_doors] = doorpage.door_struct;
        Num_doors++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_SHIP:
      if (Num_ships < MAX_SHIPS) {
        if (!mng_ReadNewShipPage(infile, &shippage))
          ok = false;
        Ships[Num_ships] = shippage.ship_struct;
        Num_ships++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_SOUND:
      if (Num_sounds < MAX_SOUNDS) {
        if (!mng_ReadNewSoundPage(infile, &soundpage))
          ok = false;
        Sounds[Num_sounds] = soundpage.sound_struct;
        Num_sounds++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_GENERIC:
      if (Num_objects < MAX_OBJECTS) {
        if (!mng_ReadNewGenericPage(infile, &genericpage))
          ok = false;
        Object_info[Num_objects] = genericpage.objinfo_struct;
        strcpy(Object_info[Num_objects].name, genericpage.objinfo_struct.name);
        Num_objects++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_MEGACELL:
      if (Num_megacells < MAX_MEGACELLS) {
        if (!mng_ReadNewMegacellPage(infile, &megacellpage))
          ok = false;
        Megacells[Num_megacells] = megacellpage.megacell_struct;
        Num_megacells++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_ROBOT:
    case PAGETYPE_POWERUP:
    case PAGETYPE_GAMEFILE:
    case PAGETYPE_UNKNOWN:
    default:
      // Unsupported/game-only page types: read and discard the payload.
      discardBytes(infile, len);
      break;
    }

    if (!ok)
      break;
  }

  cfclose(infile);
  cf_CloseLibrary(hog);

  return ok;
}
