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

// Table-file serialization for the anim_entry / anim_elem animation glue
// embedded in the generic page layout.  Only the from/to/spc triplet of each
// anim_entry is stored on disk (anim_sound_index and used are runtime-only).

#include "objinfo.h"
#include "robotfire.h"
#include "robotfirestruct.h"

object_info Object_info[MAX_OBJECTS];

// First object page slot with the given type (the engine's objinfo.cpp
// GetObjectID), used by FindValidID during level object-id translation.
std::optional<uint32_t> GetObjectID(int type) {
  for (int i = 0; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == type)
      return i;
  return std::nullopt;
}

//-----------------------------------------------------------------------------
// Animation glue (generic pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, anim_entry& data) {
  return input >> data.from >> data.to >> data.spc;
}

byte_ostream& operator<<(byte_ostream& output, const anim_entry& data) {
  return output << data.from << data.to << data.spc;
}

byte_istream& operator>>(byte_istream& input, anim_elem& data) {
  for (anim_entry& e : data.elem)
    input >> e;
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const anim_elem& data) {
  for (const anim_entry& e : data.elem)
    output << e;
  return output;
}

byte_istream& operator>>(byte_istream& input, t_ai_info& data) {
  input >> reinterpret_cast<uint32_t&>(data.flags);
  input >> data.ai_class;
  input >> data.ai_type;
  input >> data.movement_type;
  input >> data.movement_subtype;
  input >> data.fov;
  input >> data.max_velocity;
  input >> data.max_delta_velocity;
  input >> data.max_turn_rate;
  uint32_t raw_notify = 0;
  input >> raw_notify;
  raw_notify |= static_cast<uint32_t>(AI_NOTIFIES_ALWAYS_ON);
  data.notify_flags = *reinterpret_cast<ai_notify_flags_t*>(&raw_notify);
  input >> data.max_delta_turn_rate;
  input >> data.circle_distance;
  input >> data.attack_vel_percent;
  input >> data.dodge_percent;
  input >> data.dodge_vel_percent;
  input >> data.flee_vel_percent;
  input >> data.melee_damage[0];
  input >> data.melee_damage[1];
  input >> data.melee_latency[0];
  input >> data.melee_latency[1];
  input >> data.curiousity;
  input >> data.night_vision;
  input >> data.fog_vision;
  input >> data.lead_accuracy;
  input >> data.lead_varience;
  input >> data.fire_spread;
  input >> data.fight_team;
  input >> data.fight_same;
  input >> data.aggression;
  input >> data.hearing;
  input >> data.frustration;
  input >> data.roaming;
  input >> data.life_preservation;
  input >> data.avoid_friends_distance;
  input >> data.biased_flight_importance;
  input >> data.biased_flight_min;
  input >> data.biased_flight_max;
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const t_ai_info& data) {
  output << reinterpret_cast<const uint32_t&>(data.flags);
  output << data.ai_class;
  output << data.ai_type;
  output << data.movement_type;
  output << data.movement_subtype;
  output << data.fov;
  output << data.max_velocity;
  output << data.max_delta_velocity;
  output << data.max_turn_rate;
  output << (reinterpret_cast<const uint32_t&>(data.notify_flags) & ~static_cast<uint32_t>(AI_NOTIFIES_ALWAYS_ON));
  output << data.max_delta_turn_rate;
  output << data.circle_distance;
  output << data.attack_vel_percent;
  output << data.dodge_percent;
  output << data.dodge_vel_percent;
  output << data.flee_vel_percent;
  output << data.melee_damage[0];
  output << data.melee_damage[1];
  output << data.melee_latency[0];
  output << data.melee_latency[1];
  output << data.curiousity;
  output << data.night_vision;
  output << data.fog_vision;
  output << data.lead_accuracy;
  output << data.lead_varience;
  output << data.fire_spread;
  output << data.fight_team;
  output << data.fight_same;
  output << data.aggression;
  output << data.hearing;
  output << data.frustration;
  output << data.roaming;
  output << data.life_preservation;
  output << data.avoid_friends_distance;
  output << data.biased_flight_importance;
  output << data.biased_flight_min;
  output << data.biased_flight_max;
  return output;
}

// ============================================================================
// Object-id slot management (ported from the engine's objinfo.cpp).
// ============================================================================

namespace {
constexpr float DEFAULT_OBJECT_SIZE = 4.0f;
constexpr float DEFAULT_OBJECT_MASS = 1.0f;
constexpr float DEFAULT_OBJECT_DRAG = 0.1f;
constexpr float DEFAULT_OBJECT_ROTDRAG = 0.01f;
}

// Builds a fresh object_info row as AllocObjectID does: value-initialized
// first (so untouched members stay zero/empty), then the object-type
// defaults and the f_anim/f_weapons/f_ai-dependent allocations.
object_info::object_info(int type, bool f_anim, bool f_weapons, bool f_ai) : object_info{} {
  this->type = type;
  size = DEFAULT_OBJECT_SIZE;

  if (f_ai) {
    ai_info = { t_ai_info{} };
  }
  // Make sure the weapon battery info is cleared for a new object
  if (f_weapons) {
    static_wb.assign(MAX_WBS_PER_OBJ, otype_wb_info{});
  }

  if (f_anim) {
    anim.assign(NUM_MOVEMENT_CLASSES, anim_elem{});
    for (int j = 0; j < NUM_MOVEMENT_CLASSES; j++)
      for (int k = 0; k < NUM_ANIMS_PER_CLASS; k++) {
        anim[j].elem[k].spc = 1.0f;
        anim[j].elem[k].anim_sound_index = -1;
      }
  }

  phys_info.mass = DEFAULT_OBJECT_MASS;
  phys_info.drag = DEFAULT_OBJECT_DRAG;
  phys_info.rotdrag = DEFAULT_OBJECT_ROTDRAG;

  phys_info.flags.bounce = true; // PF_BOUNCE
  phys_info.num_bounces = -1;
  phys_info.coeff_restitution = 1.0f;
  phys_info.hit_die_dot = -1; // -1 means doesn't apply

  med_render_handle = -1;
  lo_render_handle = -1;
  med_lod_distance = DEFAULT_MED_LOD_DISTANCE;
  lo_lod_distance = DEFAULT_LO_LOD_DISTANCE;
  respawn_scalar = 1.0f;

  if (type == OBJ_CLUTTER || type == OBJ_ROBOT) {
    med_lod_distance *= 10;
    lo_lod_distance *= 10;
  }

  flags.inven_selectable = true; // OIF_INVEN_SELECTABLE

  // init spew types
  for (int j = 0; j < MAX_DSPEW_TYPES; j++) {
    dspew[j] = -1;
    dspew_number[j] = 0;
  }

  // init ammo count
  ammo_count = 0;
  multi_allowed = true;
}

// Allocs a object for use, returns -1 if error, else index on success
int AllocObjectID(int type, bool f_anim, bool f_weapons, bool f_ai) {
  for (int i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == OBJ_NONE) {
      Object_info[i] = object_info(type, f_anim, f_weapons, f_ai);
      Num_object_ids[type]++;
      return i;
    }
  }

  Q_ASSERT(false); // No slots free!
  return -1;
}

// Frees object index n
void FreeObjectID(int n) {
  Q_ASSERT(Object_info[n].type != OBJ_NONE);

  Num_object_ids[Object_info[n].type]--;
  Object_info[n].type = OBJ_NONE;
  Object_info[n].name.clear();
  Object_info[n].icon_name.clear();
  Object_info[n].script_name_override.clear();
  Object_info[n].module_name.clear();
  Object_info[n].description.clear();

  Object_info[n].anim.clear();
  Object_info[n].ai_info = {};
  Object_info[n].static_wb = {};
}

int GetNextObjectID(int n) {
  int type = Object_info[n].type;
  Q_ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
  if (Num_object_ids[type] == 0)
    return -1;
  for (int i = n + 1; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  for (int i = 0; i <= n; i++)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  return n;
}

int GetPrevObjectID(int n) {
  int type = Object_info[n].type;
  Q_ASSERT(n >= 0 && n < MAX_OBJECT_IDS);
  if (Num_object_ids[type] == 0)
    return -1;
  for (int i = n - 1; i >= 0; i--)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  for (int i = MAX_OBJECT_IDS - 1; i >= n; i--)
    if (Object_info[i].type == Object_info[n].type)
      return i;
  return n;
}
