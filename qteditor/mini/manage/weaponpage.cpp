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

// Weapon page reader (ported from weaponpage.cpp : 382-413, 782-901).

#include "weaponpage.h"

#include <cstdint>
#include <cstring>
#include <string>

#include "manage.h"
#include "object_external_struct.h" // light_info
#include "gamedata_helpers.h"

//-----------------------------------------------------------------------------
// Weapon page (ported from weaponpage.cpp : 382-413, 782-901)
//-----------------------------------------------------------------------------

static void mng_InitWeaponPage(mngs_weapon_page *weaponpage) {
  int i;

  *weaponpage = mngs_weapon_page{};
  weaponpage->hud_image_name.clear();
  weaponpage->fire_image_name.clear();
  weaponpage->explode_image_name.clear();
  weaponpage->spawn_name.clear();
  weaponpage->alternate_spawn_name.clear();
  weaponpage->robot_spawn_name.clear();
  weaponpage->smoke_image_name.clear();
  weaponpage->scorch_image_name.clear();
  weaponpage->icon_name.clear();
  weaponpage->particle_name.clear();

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
    weaponpage->sound_name[i].clear();
}

// Reads a weapon page from an open stream into the page structure.
byte_istream& operator>>(byte_istream& input, mngs_weapon_page& data) {
  int16_t version = 0;
  input >> version;

  input >> data.weapon_struct.name;

  // Read hud image name
  input >> data.hud_image_name;

  // Read fire image
  input >> data.fire_image_name;

  // Read particle data
  input >> data.particle_name;

  input >> data.weapon_struct.particle_count;
  input >> data.weapon_struct.particle_life;
  input >> data.weapon_struct.particle_size;

  // Read flags
  uint32_t flags_raw = 0;
  input >> flags_raw;
  std::memcpy(&data.weapon_struct.flags, &flags_raw, sizeof(flags_raw));

  // Read spawn data
  input >> data.spawn_name;
  input >> data.weapon_struct.spawn_count;

  input >> data.robot_spawn_name;
  input >> data.alternate_spawn_name;

  input >> data.weapon_struct.alternate_chance;

  // Read gravity stuff
  input >> data.weapon_struct.gravity_time;
  input >> data.weapon_struct.gravity_size;

  // Read size and homing data
  input >> data.weapon_struct.homing_fov;
  input >> data.weapon_struct.custom_size;
  input >> data.weapon_struct.size;
  input >> data.weapon_struct.thrust_time;

  // Read physics info
  input >> data.weapon_struct.phys_info;

  // Read terrain damage
  input >> data.weapon_struct.terrain_damage_size;
  input >> data.weapon_struct.terrain_damage_depth;

  // Read alpha
  input >> data.weapon_struct.alpha;

  // Read explosion data
  input >> data.explode_image_name;
  input >> data.weapon_struct.explode_time;
  input >> data.weapon_struct.explode_size;

  // Read damage data
  input >> data.weapon_struct.player_damage;

  if (version >= 7)
    input >> data.weapon_struct.generic_damage;
  else
    data.weapon_struct.generic_damage = data.weapon_struct.player_damage;

  input >> data.weapon_struct.impact_size;
  input >> data.weapon_struct.impact_time;
  input >> data.weapon_struct.impact_player_damage;

  if (version >= 7)
    input >> data.weapon_struct.impact_generic_damage;
  else
    data.weapon_struct.impact_generic_damage = data.weapon_struct.impact_player_damage;

  input >> data.weapon_struct.impact_force;

  // Read lifetime
  input >> data.weapon_struct.life_time;

  // read lighting
  input >> data.weapon_struct.lighting_info;

  // read recoil force
  if (version >= 8)
    input >> data.weapon_struct.recoil_force;
  else
    data.weapon_struct.recoil_force = 0.0f;

  // Read its sound names
  for (int i = 0; i < MAX_WEAPON_SOUNDS; i++)
    input >> data.sound_name[i];

  // Read smoke name
  input >> data.smoke_image_name;

  // Read scorch data
  input >> data.scorch_image_name;
  input >> data.weapon_struct.scorch_size;

  // Read icon name
  input >> data.icon_name;

  return input;
}

// Writes a weapon page in the current (WEAPONPAGE_VERSION) format, the exact
// mirror of operator>> (same field order and encodings).
byte_ostream& operator<<(byte_ostream& output, const mngs_weapon_page& data) {
  output << static_cast<int16_t>(WEAPONPAGE_VERSION);
  output << data.weapon_struct.name;

  // Write out hud image name
  output << data.hud_image_name;

  // Write out fire image
  output << data.fire_image_name;

  // Write out particle data
  output << data.particle_name;

  output << data.weapon_struct.particle_count;
  output << data.weapon_struct.particle_life;
  output << data.weapon_struct.particle_size;

  // Write out flags
  uint32_t flags_raw = 0;
  std::memcpy(&flags_raw, &data.weapon_struct.flags, sizeof(flags_raw));
  output << flags_raw;

  // Write out spawn data
  output << data.spawn_name;
  output << data.weapon_struct.spawn_count;

  output << data.robot_spawn_name;
  output << data.alternate_spawn_name;

  output << data.weapon_struct.alternate_chance;

  // Write out gravity stuff
  output << data.weapon_struct.gravity_time;
  output << data.weapon_struct.gravity_size;

  // Write out size and homing data
  output << data.weapon_struct.homing_fov;
  output << data.weapon_struct.custom_size;
  output << data.weapon_struct.size;
  output << data.weapon_struct.thrust_time;

  // Write out physics info
  output << data.weapon_struct.phys_info;

  // Write out terrain damage
  output << data.weapon_struct.terrain_damage_size;
  output << data.weapon_struct.terrain_damage_depth;

  // Write out alpha
  output << data.weapon_struct.alpha;

  // Write out explosion data
  output << data.explode_image_name;
  output << data.weapon_struct.explode_time;
  output << data.weapon_struct.explode_size;

  // Write out damage data
  output << data.weapon_struct.player_damage;
  output << data.weapon_struct.generic_damage;

  output << data.weapon_struct.impact_size;
  output << data.weapon_struct.impact_time;
  output << data.weapon_struct.impact_player_damage;
  output << data.weapon_struct.impact_generic_damage;
  output << data.weapon_struct.impact_force;

  // Write out lifetime
  output << data.weapon_struct.life_time;

  // Write out lighting
  output << data.weapon_struct.lighting_info;

  // Write out recoil force
  output << data.weapon_struct.recoil_force;

  // Write out its sound names
  for (int i = 0; i < MAX_WEAPON_SOUNDS; i++)
    output << data.sound_name[i];

  // Write out smoke name
  output << data.smoke_image_name;

  // Write out scorch data
  output << data.scorch_image_name;
  output << data.weapon_struct.scorch_size;

  // Write out icon name
  output << data.icon_name;

  return output;
}

int mng_ReadNewWeaponPage(posix_istream &infile, mngs_weapon_page *weaponpage) {
  mng_InitWeaponPage(weaponpage);

  infile >> *weaponpage;

  weaponpage->weapon_struct.used = 1;

  // OEM hack was omitted (OEM is never defined in the mini build).

  if (weaponpage->weapon_struct.name == "EMDBlob") {
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
