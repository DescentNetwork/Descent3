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

#ifndef POWERUP_H
#define POWERUP_H

#include <array>
#include <string>
#include <cstdint>

#include "manage.h"
#include "object.h"

#define MAX_POWERUPS 100
#define MAX_STATIC_POWERUPS 50

// powerup::flags bits
struct [[gnu::packed]] powerup_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 31;
  uint32_t image_bitmap : 1; // PF_IMAGE_BITMAP (1)
#else
  uint32_t image_bitmap : 1; // PF_IMAGE_BITMAP (1)
  uint32_t padding : 31;
#endif
};
static_assert(sizeof(powerup_flags_t) == sizeof(uint32_t));

// These enumerators must correspond to the Static_powerup_names array
enum class powerup_id : uint8_t {
  shield = 0,
  energy = 1,

  laser = 2,
  vulcan_weapon = 3,
  spreadfire_weapon = 4,
  plasma_weapon = 5,
  fusion_weapon = 6,

  super_laser = 7,
  gauss_weapon = 8,
  helix_weapon = 9,
  phoenix_weapon = 10,
  omega_weapon = 11,

  missile_1 = 12,
  missile_4 = 13, // 4-pack MUST follow single missile
  homing_missile_1 = 14,
  homing_missile_4 = 15, // 4-pack MUST follow single missile
  proximity_weapon = 16,
  smart_missile_weapon = 17,
  mega_weapon = 18,

  flash_missile_1 = 19,
  flash_missile_4 = 20, // 4-pack MUST follow single missile
  guided_missile_1 = 21,
  guided_missile_4 = 22, // 4-pack MUST follow single missile
  smart_mine = 23,
  mercury_missile_1 = 24,
  mercury_missile_4 = 25, // 4-pack MUST follow single missile
  earthshaker_missile = 26,

  extra_life = 27,
  quad_fire = 28,
  vulcan_ammo = 29,
  cloak = 30,
  turbo = 31,
  invulnerability = 32,
  full_map = 33,
  converter = 34,
  ammo_rack = 35,
  afterburner = 36,
  headlight = 37,

  flag_blue = 38,
  flag_red = 39,
  hoard_orb = 40,
};
// sound stuff

#define MAX_POWERUP_SOUNDS 7

enum class powerup_sound_index : uint8_t {
  pickup = 0,
};

struct powerup {
  std::string name;
  float size;
  int score;
  int image_handle;              // Either a vclip or a polygon model
  std::string model_name; // used for remapping powerups which contain models
  powerup_flags_t flags = {};
  uint16_t used;

  std::array<int16_t, MAX_POWERUP_SOUNDS> sounds;

  // Default physics information for this powerup type
  physics_info phys_info; // the physics data for this obj type.

};

extern char *Static_powerup_names[MAX_STATIC_POWERUPS];

// Sets all powerups to unused
void InitPowerups();

// Allocs a powerup for use, returns -1 if error, else index on success
int AllocPowerup();

// Frees powerup index n
void FreePowerup(int n);

// Searches thru all powerups for a specific name, returns -1 if not found
// or index of powerup with name
int FindPowerupName(const std::string &name);

// Given a filename, loads either the model or vclip found in that file.  If type
// is not NULL, sets it to 1 if file is model, otherwise sets it to zero
int LoadPowerupImage(const std::string &filename, int *type);

// Given a powerup handle, returns that powerups image for framenum
int GetPowerupImage(int handle, int framenum);

// Given an object, renders the representation of this powerup
// Currently only handles bitmap types, not poly models
void DrawPowerupObject(object *obj);

// Given a powerup name, assigns that powerup to a specific index into
// the Powerups array.  Returns -1 if the named powerup is not found, 0 if the powerup
// is already in its place, or 1 if successfully moved
int MatchPowerupToIndex(const std::string &name, int dest_index);

// Moves a powerup from a given index into a new one (above MAX_STATIC_POWERUPS)
// returns new index
int MovePowerupFromIndex(int index);

// This is a very confusing function.  It takes all the powerups that we have loaded
// and remaps then into their proper places (if they are static).
void RemapPowerups();

// goes thru every entity that could possible have a powerup index (ie objects, robots, etc)
// and changes the old index to the new index
void RemapAllPowerupObjects(int old_index, int new_index);

// Player activated this powerup
int DoPowerup(object *obj);

#endif
