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

#ifndef DAMAGE_EXTERNAL_H_
#define DAMAGE_EXTERNAL_H_

#include <cstdint>

// Player Damage types.  Used only to make a sound.
enum class player_damage_type : uint8_t {
  none = 0,            // Make no sound
  energy_weapon = 1,   // Hit by laser, etc.
  matter_weapon = 2,   // Hit by missile, etc.
  melee_attack = 3,    // Whacked by robot
  concussive_force = 4,// Hit by shockwave
  wall_hit = 5,        // Crashed into a wall
  volatile_hiss = 6,   // Touched a volatile substance (such as acid)
};

// Generic damage types
enum class generic_damage_type : uint8_t {
  scripted = 0,     // Script is saying to do the damage
  electric = 1,     // Electrical weapons
  concussive = 2,   // Concussive damage
  fire = 3,         // Fire and napalm like stuff
  matter = 4,       // Matter weapons
  energy = 5,       // Energy weapons and fields
  physics = 6,      // Bumping into a wall or player too hard
  melee_attack = 7, // From a melee robot attack
  volatile_hiss = 8,// Touched a volatile substance (such as acid)
};

#endif
