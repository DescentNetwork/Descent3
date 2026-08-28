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
#include <string>
#include <vector>
#include <QtGlobal>

#include "posix_helpers.h"
#include <hog2_format.h>

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

// Reads a variable-length, NUL-terminated string field into a fixed char array
// (mirrors cf_ReadString writing through a caller buffer).  `dest_size` is the
// span of the destination array, so at most dest_size-1 bytes are copied and the
// copy is always NUL-terminated.
static void readStringField(posix_istream &infile, char *dest, size_t dest_size) {
  std::string s;
  psReadString(infile, s, dest_size - 1);
  std::strncpy(dest, s.c_str(), dest_size - 1);
  dest[dest_size - 1] = '\0';
}

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
  if (version >= 2)
    infile >> static_wb->flags;
  else {
    uint8_t b = 0;
    infile >> b;
    static_wb->flags = b;
  }
  infile >> static_wb->gp_quad_fire_mask;
}

static void mng_ReadLightingChunk(light_info *lighting_info, posix_istream &infile) {
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
  readStringField(infile, genericpage->objinfo_struct.name, sizeof(genericpage->objinfo_struct.name));

  // Read model names
  readStringField(infile, genericpage->image_name, sizeof(genericpage->image_name));
  readStringField(infile, genericpage->med_image_name, sizeof(genericpage->med_image_name));
  readStringField(infile, genericpage->lo_image_name, sizeof(genericpage->lo_image_name));

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

  // Read script name
  char dummy[256];
  readStringField(infile, dummy, sizeof(dummy)); // genericpage->objinfo_struct.script_name

  if (version >= 18) {
    readStringField(infile, genericpage->objinfo_struct.module_name, sizeof(genericpage->objinfo_struct.module_name));
  } else {
    genericpage->objinfo_struct.module_name[0] = '\0';
  }

  if (version >= 19) {
    readStringField(infile, genericpage->objinfo_struct.script_name_override, sizeof(genericpage->objinfo_struct.script_name_override));
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
    char tempbuf[1024];

    readStringField(infile, tempbuf, sizeof(tempbuf));
    size_t slen = strlen(tempbuf) + 1;

    genericpage->objinfo_struct.description = mem_rmalloc<char>(slen);
    Q_ASSERT(genericpage->objinfo_struct.description);
    strcpy(genericpage->objinfo_struct.description, tempbuf);
  } else
    genericpage->objinfo_struct.description = NULL;

  // Read icon name
  readStringField(infile, genericpage->objinfo_struct.icon_name, sizeof(genericpage->objinfo_struct.icon_name));

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

  // Read flags
  infile >> genericpage->objinfo_struct.flags;

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
  } else if ((genericpage->objinfo_struct.flags | OIF_USES_PHYSICS) && genericpage->ai_info.max_velocity > 0.0f) {
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
    readStringField(infile, genericpage->dspew_name[i], sizeof(genericpage->dspew_name[i]));
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
      readStringField(infile, genericpage->weapon_name[i][j], sizeof(genericpage->weapon_name[i][j]));
  }

  // read sounds
  Q_ASSERT(MAX_OBJ_SOUNDS == 2);
  for (i = 0; i < MAX_OBJ_SOUNDS; i++)
    readStringField(infile, genericpage->sound_name[i], sizeof(genericpage->sound_name[i]));
  if (version < 26) { // used to be three sounds
    char temp_sound_name[PAGENAME_LEN];
    readStringField(infile, temp_sound_name, sizeof(temp_sound_name));
  }

  for (i = 0; i < MAX_AI_SOUNDS; i++)
    readStringField(infile, genericpage->ai_sound_name[i], sizeof(genericpage->ai_sound_name[i]));

  for (i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      readStringField(infile, genericpage->fire_sound_name[i][j], sizeof(genericpage->fire_sound_name[i][j]));
  }

  for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (j = 0; j < NUM_ANIMS_PER_CLASS; j++)
      readStringField(infile, genericpage->anim_sound_name[i][j], sizeof(genericpage->anim_sound_name[i][j]));
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
    for (i = 0; i < n_death_types; i++) {
      int flags = 0;
      infile >> flags;
      if (version == 22) { // translate death flags
        Q_ASSERT(false);            // this version no longer supported
      }

      genericpage->objinfo_struct.death_types[i].flags = flags;
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
        (genericpage->objinfo_struct.type == OBJ_BUILDING && (genericpage->objinfo_struct.flags & OIF_CONTROL_AI)))
      if (genericpage->objinfo_struct.flags & OIF_DESTROYABLE)
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

int mng_ReadNewWeaponPage(posix_istream &infile, mngs_weapon_page *weaponpage) {
  int i;

  mng_InitWeaponPage(weaponpage);

  int16_t version = 0;
  infile >> version;

  readStringField(infile, weaponpage->weapon_struct.name, sizeof(weaponpage->weapon_struct.name));

  // Read hud image name
  readStringField(infile, weaponpage->hud_image_name, sizeof(weaponpage->hud_image_name));

  // Read fire image
  readStringField(infile, weaponpage->fire_image_name, sizeof(weaponpage->fire_image_name));

  // Read particle data
  readStringField(infile, weaponpage->particle_name, sizeof(weaponpage->particle_name));

  infile >> weaponpage->weapon_struct.particle_count;
  infile >> weaponpage->weapon_struct.particle_life;
  infile >> weaponpage->weapon_struct.particle_size;

  // Read flags
  infile >> weaponpage->weapon_struct.flags;

  // Read spawn data
  readStringField(infile, weaponpage->spawn_name, sizeof(weaponpage->spawn_name));
  infile >> weaponpage->weapon_struct.spawn_count;

  readStringField(infile, weaponpage->robot_spawn_name, sizeof(weaponpage->robot_spawn_name));
  readStringField(infile, weaponpage->alternate_spawn_name, sizeof(weaponpage->alternate_spawn_name));

  infile >> weaponpage->weapon_struct.alternate_chance;

  // Read gravity stuff
  infile >> weaponpage->weapon_struct.gravity_time;
  infile >> weaponpage->weapon_struct.gravity_size;

  // Read size and homing data
  infile >> weaponpage->weapon_struct.homing_fov;
  infile >> weaponpage->weapon_struct.custom_size;
  infile >> weaponpage->weapon_struct.size;
  infile >> weaponpage->weapon_struct.thrust_time;

  // Read physics info
  mng_ReadPhysicsChunk(&weaponpage->weapon_struct.phys_info, infile);

  // Read terrain damage
  infile >> weaponpage->weapon_struct.terrain_damage_size;
  infile >> weaponpage->weapon_struct.terrain_damage_depth;

  // Read alpha
  infile >> weaponpage->weapon_struct.alpha;

  // Read explosion data
  readStringField(infile, weaponpage->explode_image_name, sizeof(weaponpage->explode_image_name));
  infile >> weaponpage->weapon_struct.explode_time;
  infile >> weaponpage->weapon_struct.explode_size;

  // Read damage data
  infile >> weaponpage->weapon_struct.player_damage;

  if (version >= 7)
    infile >> weaponpage->weapon_struct.generic_damage;
  else
    weaponpage->weapon_struct.generic_damage = weaponpage->weapon_struct.player_damage;

  infile >> weaponpage->weapon_struct.impact_size;
  infile >> weaponpage->weapon_struct.impact_time;
  infile >> weaponpage->weapon_struct.impact_player_damage;

  if (version >= 7)
    infile >> weaponpage->weapon_struct.impact_generic_damage;
  else
    weaponpage->weapon_struct.impact_generic_damage = weaponpage->weapon_struct.impact_player_damage;

  infile >> weaponpage->weapon_struct.impact_force;

  // Read lifetime
  infile >> weaponpage->weapon_struct.life_time;

  // read lighting
  mng_ReadLightingChunk(&weaponpage->weapon_struct.lighting_info, infile);

  // read recoil force
  if (version >= 8)
    infile >> weaponpage->weapon_struct.recoil_force;
  else
    weaponpage->weapon_struct.recoil_force = 0.0f;

  // Read its sound names
  for (i = 0; i < MAX_WEAPON_SOUNDS; i++)
    readStringField(infile, weaponpage->sound_name[i], sizeof(weaponpage->sound_name[i]));

  // Read smoke name
  readStringField(infile, weaponpage->smoke_image_name, sizeof(weaponpage->smoke_image_name));

  // Read scorch data
  readStringField(infile, weaponpage->scorch_image_name, sizeof(weaponpage->scorch_image_name));
  infile >> weaponpage->weapon_struct.scorch_size;

  // Read icon name
  readStringField(infile, weaponpage->icon_name, sizeof(weaponpage->icon_name));

  weaponpage->weapon_struct.used = 1;

  // OEM hack was omitted (OEM is never defined in the mini build).

  if (!stricmp(weaponpage->weapon_struct.name, "EMDBlob")) {
    weaponpage->weapon_struct.life_time = 1.7f;
  }

  return 1; // successfully read
}

// Reads a weapon page from an open file.  Returns 0 on error.
int mng_ReadWeaponPage(posix_istream &infile, mngs_weapon_page *weaponpage) {
  if (!Old_table_method)
    return mng_ReadNewWeaponPage(infile, weaponpage);
  return 0; // old command-based method not supported in mini build
}

//-----------------------------------------------------------------------------
// Texture page (ported from texpage.cpp : 508-759)
//-----------------------------------------------------------------------------

static void mng_InitTexturePage(mngs_texture_page *texpage) {
  // Member-wise reset (NOT memset — the struct now holds std::string members
  // that memset would corrupt).
  *texpage = mngs_texture_page{};
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

  texpage->bitmap_name.clear();
  texpage->destroy_name.clear();
  texpage->sound_name.clear();
}

// Reads a texture page from an open file.  Returns 0 on error.
int mng_ReadNewTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  int i;

  mng_InitTexturePage(texpage);

  int16_t version = 0;
  infile >> version;

  readStringField(infile, texpage->tex_struct.name, sizeof(texpage->tex_struct.name));
  psReadString(infile, texpage->bitmap_name, PAGENAME_LEN - 1);
  psReadString(infile, texpage->destroy_name, PAGENAME_LEN - 1);

  infile >> texpage->tex_struct.r;
  infile >> texpage->tex_struct.g;
  infile >> texpage->tex_struct.b;
  infile >> texpage->tex_struct.alpha;

  infile >> texpage->tex_struct.speed;

  infile >> texpage->tex_struct.slide_u;
  infile >> texpage->tex_struct.slide_v;
  infile >> texpage->tex_struct.reflectivity;

  infile >> texpage->tex_struct.corona_type;
  infile >> texpage->tex_struct.damage;
  infile >> texpage->tex_struct.flags;

  if (texpage->tex_struct.flags & TF_PROCEDURAL) {
    for (i = 0; i < 255; i++) {
      uint16_t val = 0;
      infile >> val;
      texpage->proc_palette[i] = val;
    }

    infile >> texpage->proc_heat;
    infile >> texpage->proc_light;
    infile >> texpage->proc_thickness;
    infile >> texpage->proc_evaluation_time;

    if (version >= 6) {
      infile >> texpage->osc_time;
      infile >> texpage->osc_value;
    }

    int16_t npe = 0;
    infile >> npe;
    texpage->num_proc_elements = npe;

    if (texpage->num_proc_elements > MAX_PROC_ELEMENTS) {
      LOG_ERROR("Warning! Too many procedural elements!");
      Q_ASSERT(false);
    }

    for (i = 0; i < texpage->num_proc_elements; i++) {
      infile >> texpage->proc_type[i];
      infile >> texpage->proc_frequency[i];
      infile >> texpage->proc_speed[i];
      infile >> texpage->proc_size[i];
      infile >> texpage->proc_x1[i];
      infile >> texpage->proc_y1[i];

      infile >> texpage->proc_x2[i];
      infile >> texpage->proc_y2[i];
    }
  }

  if (texpage->tex_struct.flags & TF_PROCEDURAL) {
    if (texpage->num_proc_elements == 0)
      texpage->tex_struct.flags &= ~TF_PROCEDURAL;
  }

  if (!strnicmp(texpage->destroy_name.c_str(), "INVALID", 7))
    texpage->destroy_name.clear();

  if (version >= 5) {

    if (version < 7) {
      // Kill buggy version of sound resolving code
      int s;
      infile >> s;
      texpage->tex_struct.sound = s;
      texpage->tex_struct.sound = -1;
      texpage->sound_name.clear();
    } else
      psReadString(infile, texpage->sound_name, PAGENAME_LEN - 1);

    infile >> texpage->tex_struct.sound_volume;
  } else {
    texpage->tex_struct.sound = -1;
    texpage->tex_struct.sound_volume = 1.0;
  }

  texpage->tex_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadTexturePage(posix_istream &infile, mngs_texture_page *texpage) {
  if (!Old_table_method)
    return mng_ReadNewTexturePage(infile, texpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Sound page (ported from soundpage.cpp : 222-255)
//-----------------------------------------------------------------------------

int mng_ReadNewSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  /* int version = */ int16_t v; infile >> v;
  // read in name,rawfile name
  readStringField(infile, soundpage->sound_struct.name, sizeof(soundpage->sound_struct.name));
  readStringField(infile, soundpage->raw_name, sizeof(soundpage->raw_name));
  infile >> soundpage->sound_struct.flags;

  infile >> soundpage->sound_struct.loop_start;
  infile >> soundpage->sound_struct.loop_end;
  infile >> soundpage->sound_struct.outer_cone_volume;
  infile >> soundpage->sound_struct.inner_cone_angle;
  infile >> soundpage->sound_struct.outer_cone_angle;
  infile >> soundpage->sound_struct.max_distance;
  infile >> soundpage->sound_struct.min_distance;
  infile >> soundpage->sound_struct.import_volume;
  // The full engine has a DEMO-only import_volume adjustment block here; it is
  // compiled out unless DEMO is defined (never in the mini build).

  // This is a valid new page
  soundpage->sound_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadSoundPage(posix_istream &infile, mngs_sound_page *soundpage) {
  if (!Old_table_method)
    return mng_ReadNewSoundPage(infile, soundpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Door page (ported from doorpage.cpp : 261-292)
//-----------------------------------------------------------------------------

int mng_ReadNewDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  int16_t version = 0;
  infile >> version;

  readStringField(infile, doorpage->door_struct.name, sizeof(doorpage->door_struct.name));
  readStringField(infile, doorpage->image_name, sizeof(doorpage->image_name));

  infile >> doorpage->door_struct.total_open_time;
  infile >> doorpage->door_struct.total_close_time;
  infile >> doorpage->door_struct.total_time_open;

  infile >> doorpage->door_struct.flags;

  if (version >= 3)
    infile >> doorpage->door_struct.hit_points;
  else
    doorpage->door_struct.hit_points = 0;

  readStringField(infile, doorpage->open_sound_name, sizeof(doorpage->open_sound_name));
  readStringField(infile, doorpage->close_sound_name, sizeof(doorpage->close_sound_name));

  if (version >= 2)
    readStringField(infile, doorpage->door_struct.module_name, sizeof(doorpage->door_struct.module_name));
  else
    doorpage->door_struct.module_name[0] = '\0';

  // This is a valid new page
  doorpage->door_struct.used = 1;

  return 1; // successfully read
}

int mng_ReadDoorPage(posix_istream &infile, mngs_door_page *doorpage) {
  if (!Old_table_method)
    return mng_ReadNewDoorPage(infile, doorpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Megacell page (ported from megapage.cpp : 175-192)
//-----------------------------------------------------------------------------

int mng_ReadNewMegacellPage(posix_istream &infile, mngs_megacell_page *megacellpage) {
  int i;
  memset(megacellpage, 0, sizeof(mngs_megacell_page));
  /* int version = */ int16_t v; infile >> v;

  readStringField(infile, megacellpage->megacell_struct.name, sizeof(megacellpage->megacell_struct.name));

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    readStringField(infile, megacellpage->cellname[i], sizeof(megacellpage->cellname[i]));

  infile >> megacellpage->megacell_struct.width;
  infile >> megacellpage->megacell_struct.height;
  // This is a valid new page
  megacellpage->megacell_struct.used = 1;
  return 1; // successfully read
}

int mng_ReadMegacellPage(posix_istream &infile, mngs_megacell_page *megacellpage) {
  if (!Old_table_method)
    return mng_ReadNewMegacellPage(infile, megacellpage);
  return 0; // old command-based table not supported in mini build
}

//-----------------------------------------------------------------------------
// Ship page (ported from shippage.cpp : 539-611)
//-----------------------------------------------------------------------------

int mng_ReadNewShipPage(posix_istream &infile, mngs_ship_page *shippage) {
  int i, j;

  // Clear the page record.  The embedded ship holds std::string members, so
  // use member-wise reset rather than memset (which would corrupt them).
  *shippage = mngs_ship_page{};

  int16_t version = 0;
  infile >> version;

  // Read misc names.  name is a fixed char buffer (used by the manage API);
  // cockpit_name / hud_config_name are std::string members, so the psReadString
  // read stores into a caller std::string which is then assigned.
  readStringField(infile, shippage->ship_struct.name, sizeof(shippage->ship_struct.name));
  std::string cockpit_buf;
  psReadString(infile, cockpit_buf, PAGENAME_LEN);
  std::string hud_buf;
  psReadString(infile, hud_buf, PAGENAME_LEN);
  shippage->ship_struct.cockpit_name = cockpit_buf;
  shippage->ship_struct.hud_config_name = hud_buf;

  // Read in model names
  readStringField(infile, shippage->image_name, sizeof(shippage->image_name));
  readStringField(infile, shippage->dying_image_name, sizeof(shippage->dying_image_name));
  readStringField(infile, shippage->med_image_name, sizeof(shippage->med_image_name));
  readStringField(infile, shippage->lo_image_name, sizeof(shippage->lo_image_name));

  // read lod distance
  infile >> shippage->ship_struct.med_lod_distance;
  infile >> shippage->ship_struct.lo_lod_distance;

  // Read physics
  mng_ReadPhysicsChunk(&shippage->ship_struct.phys_info, infile);

  infile >> shippage->ship_struct.size;
  infile >> shippage->ship_struct.armor_scalar;
  infile >> shippage->ship_struct.flags;

  for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    infile >> shippage->ship_struct.fire_flags[i];
    readStringField(infile, shippage->firing_sound_name[i], sizeof(shippage->firing_sound_name[i]));
    readStringField(infile, shippage->release_sound_name[i], sizeof(shippage->release_sound_name[i]));
    readStringField(infile, shippage->spew_powerup_name[i], sizeof(shippage->spew_powerup_name[i]));
    infile >> shippage->ship_struct.max_ammo[i];

    if (version >= 6)
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 2);
    else
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 1);

    for (j = 0; j < MAX_WB_GUNPOINTS; j++)
      readStringField(infile, shippage->fire_sound_name[i][j], sizeof(shippage->fire_sound_name[i][j]));

    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      readStringField(infile, shippage->weapon_name[i][j], sizeof(shippage->weapon_name[i][j]));
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

int mng_ReadShipPage(posix_istream &infile, mngs_ship_page *shippage) {
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
static void discardBytes(posix_istream &infile, int count) {
  uint8_t buf[4096];
  while (count > 0) {
    int take = count > (int)sizeof(buf) ? (int)sizeof(buf) : count;
    infile.read(buf, take);
    if (infile.eof())
      break;
    count -= take;
  }
}

// Locates `img` inside the open HOG `archive`, reads its full payload from the
// still-open HOG stream `hogin`, and hands the bytes to the decoder via
// bm_LoadBitmapFromMemory (fmemopen posix_istream).  Returns the bitmap handle,
// or -1 if the image is not in the Hog or fails to decode.
static int loadTextureFromArchive(hog2::archive_t &archive, posix_istream &hogin, const char *img, int format) {
  auto entry = archive.end();
  const std::string needle = lowercase(std::string(img));
  for (auto it = archive.begin(); it != archive.end(); ++it) {
    if (lowercase(it->name.string()) == needle) {
      entry = it;
      break;
    }
  }
  if (entry == archive.end())
    return -1;

  const size_t off = archive.fileOffset(entry);
  const size_t len = entry->len;
  std::vector<uint8_t> buf(len);
  hogin.seek(off, std::ios_base::beg);
  hogin.read(buf.data(), len);

  return bm_LoadBitmapFromMemory(buf.data(), buf.size(), img, format, 0);
}

bool loadGameDataTable(const std::filesystem::path &d3HogPath) {
  // Table.gam and every texture image are pulled straight from the HOG payload
  // below using posix_istream + hog2::archive_t (independent of CFILE/cfopen).

  // Read the whole HOG into memory via posix_stream.
  posix_istream hogin;
  if (!hogin.open(d3HogPath, std::ios_base::in)) {
    return false;
  }

  hog2::archive_t archive;
  try {
    hogin >> archive;
  } catch (const std::invalid_argument &) {
    hogin.close();
    return false;
  }

  // Locate the table.gam entry so its payload offset/length can be computed.
  auto entry = archive.end();
  for (auto it = archive.begin(); it != archive.end(); ++it) {
    if (lowercase(std::string(it->name.string())) == "table.gam") {
      entry = it;
      break;
    }
  }
  if (entry == archive.end()) {
    hogin.close();
    return false;
  }

  const size_t payload_offset = archive.fileOffset(entry);
  const size_t payload_len = entry->len;

  // Read the whole Table.gam payload into memory, then wrap it in an in-memory
  // posix_istream (fmemopen) so the page loop below can reuse the same readers.
  // The HOG stream `hogin` is kept open so texture images can be looked up in
  // `archive` and seeked/read per-texture during the page loop.
  std::vector<uint8_t> payload(payload_len);
  hogin.seek(payload_offset, std::ios_base::beg);
  hogin.read(payload.data(), payload_len);

  posix_istream infile(payload.data(), payload.size(), std::ios_base::in);

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
  while (!infile.eof()) {
    uint8_t pagetype = 0;
    int32_t len = 0;
    infile >> pagetype;
    infile >> len;

    switch (pagetype) {
    case PAGETYPE_TEXTURE:
      if (Num_textures < MAX_TEXTURES) {
        if (!mng_ReadNewTexturePage(infile, &texpage))
          ok = false;
        GameTextures[Num_textures] = texpage.tex_struct;
        strcpy(GameTextures[Num_textures].name, texpage.tex_struct.name);
        // Load the texture's image so textured faces render: the payload is
        // read straight out of the open d3.hog archive and decoded from memory.
        GameTextures[Num_textures].bm_handle = -1;
        if (!texpage.bitmap_name.empty()) {
          int bm = loadTextureFromArchive(archive, hogin, texpage.bitmap_name.c_str(), BITMAP_FORMAT_1555);
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

  hogin.close();

  return ok;
}
