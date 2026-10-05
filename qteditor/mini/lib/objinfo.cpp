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

// Table-file serialization for the ai_info_t animation/AI glue embedded in the
// generic page layout.  Only the from/to/spc triplet of each anim_entry is
// stored on disk (anim_sound_index and used are runtime-only).

#include "objinfo.h"
#include "robotfire.h"
#include "robotfirestruct.h"

object_info Object_info[MAX_OBJECTS];

// First object page slot with the given type (the engine's objinfo.cpp
// GetObjectID), used by FindValidID during level object-id translation.
index_t GetObjectID(object_type type) {
  for (uint32_t i = 0; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == type)
      return i;
  return std::nullopt;
}

//-----------------------------------------------------------------------------
// Animation glue (generic pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, ai_info_t& data) {
  return input >> reinterpret_cast<uint32_t&>(data.flags)
         >> data.ai_class
         >> data.ai_type
         >> data.movement_type
         >> data.movement_subtype
         >> data.fov
         >> data.max_velocity
         >> data.max_delta_velocity
         >> data.max_turn_rate
         >> reinterpret_cast<uint32_t&>(data.notify_flags)
         >> data.max_delta_turn_rate
         >> data.circle_distance
         >> data.attack_vel_percent
         >> data.dodge_percent
         >> data.dodge_vel_percent
         >> data.flee_vel_percent
         >> data.melee_damage
         >> data.melee_latency
         >> data.curiousity
         >> data.night_vision
         >> data.fog_vision
         >> data.lead_accuracy
         >> data.lead_varience
         >> data.fire_spread
         >> data.fight_team
         >> data.fight_same
         >> data.aggression
         >> data.hearing
         >> data.frustration
         >> data.roaming
         >> data.life_preservation
         >> data.avoid_friends_distance
         >> data.biased_flight_importance
         >> data.biased_flight_min
         >> data.biased_flight_max;
}

byte_ostream& operator<<(byte_ostream& output, const ai_info_t& data) {
  return output << reinterpret_cast<const uint32_t&>(data.flags)
         << data.ai_class
         << data.ai_type
         << data.movement_type
         << data.movement_subtype
         << data.fov
         << data.max_velocity
         << data.max_delta_velocity
         << data.max_turn_rate
         << reinterpret_cast<const uint32_t&>(data.notify_flags)
         << data.max_delta_turn_rate
         << data.circle_distance
         << data.attack_vel_percent
         << data.dodge_percent
         << data.dodge_vel_percent
         << data.flee_vel_percent
         << data.melee_damage
         << data.melee_latency
         << data.curiousity
         << data.night_vision
         << data.fog_vision
         << data.lead_accuracy
         << data.lead_varience
         << data.fire_spread
         << data.fight_team
         << data.fight_same
         << data.aggression
         << data.hearing
         << data.frustration
         << data.roaming
         << data.life_preservation
         << data.avoid_friends_distance
         << data.biased_flight_importance
         << data.biased_flight_min
         << data.biased_flight_max;
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
object_info::object_info(object_type type, bool f_anim, bool f_weapons, bool f_ai) : object_info{} {
  this->type = type;
  size = DEFAULT_OBJECT_SIZE;

  if (f_ai) {
    ai_info = { ai_info_t{} };
  }
  // Make sure the weapon battery info is cleared for a new object
  if (f_weapons) {
    static_wb.assign(MAX_WBS_PER_OBJ, otype_wb_info{});
  }

  if (f_anim) {
    // anim_entry carries the object's per-class defaults (spc = 1.0f,
    // anim_sound_index = SOUND_NONE_INDEX), so a value-initialized
    // anim_elem is already the correct "fresh object" state.
    anim.assign(NUM_MOVEMENT_CLASSES, anim_elem{});
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

  if (type == object_type::clutter || type == object_type::robot) {
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
index_t AllocObjectID(object_type type, bool f_anim, bool f_weapons, bool f_ai) {
  for (uint32_t i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == object_type::none) {
      Object_info[i] = object_info(type, f_anim, f_weapons, f_ai);
      Num_object_ids[obj_type_index(type)]++;
      return i;
    }
  }

  Q_ASSERT(false); // No slots free!
  return std::nullopt;
}

// Frees object index n
void FreeObjectID(index_t obj) {
  if(obj)
  {
    uint32_t n = *obj;
    Q_ASSERT(Object_info[n].type != object_type::none);

    Num_object_ids[obj_type_index(Object_info[n].type)]--;
    Object_info[n].type = object_type::none;
    Object_info[n].name.clear();
    Object_info[n].icon_name.clear();
    Object_info[n].script_name_override.clear();
    Object_info[n].module_name.clear();
    Object_info[n].description.clear();

    Object_info[n].anim.clear();
    Object_info[n].ai_info = {};
    Object_info[n].static_wb = {};
  }
}

index_t GetNextObjectID(index_t n) {
  if(!n)
    return std::nullopt;
  Q_ASSERT(*n < MAX_OBJECT_IDS);
  const object_type t = Object_info[*n].type;
  if (Num_object_ids[obj_type_index(t)] == 0)
    return std::nullopt;;
  for (int i = *n + 1; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == t)
      return i;
  for (int i = 0; i <= n; i++)
    if (Object_info[i].type == t)
      return i;
  return n;
}

index_t GetPrevObjectID(index_t n) {
  Q_ASSERT(*n < MAX_OBJECT_IDS);
  if(!n)
    return std::nullopt;
  const object_type t = Object_info[*n].type;
  if (Num_object_ids[obj_type_index(t)] == 0)
    return std::nullopt;
  for (int i = *n - 1; i >= 0; i--)
    if (Object_info[i].type == t)
      return i;
  for (int i = MAX_OBJECT_IDS - 1; i >= n; i--)
    if (Object_info[i].type == t)
      return i;
  return n;
}
