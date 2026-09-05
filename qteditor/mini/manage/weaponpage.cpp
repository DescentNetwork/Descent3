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

int mng_ReadNewWeaponPage(posix_istream &infile, mngs_weapon_page *weaponpage) {
  int i;

  mng_InitWeaponPage(weaponpage);

  int16_t version = 0;
  infile >> version;

  infile >> weaponpage->weapon_struct.name;

  // Read hud image name
  infile >> weaponpage->hud_image_name;

  // Read fire image
  infile >> weaponpage->fire_image_name;

  // Read particle data
  infile >> weaponpage->particle_name;

  infile >> weaponpage->weapon_struct.particle_count;
  infile >> weaponpage->weapon_struct.particle_life;
  infile >> weaponpage->weapon_struct.particle_size;

  // Read flags
  uint32_t flags_raw = 0;
  infile >> flags_raw;
  std::memcpy(&weaponpage->weapon_struct.flags, &flags_raw, sizeof(flags_raw));

  // Read spawn data
  infile >> weaponpage->spawn_name;
  infile >> weaponpage->weapon_struct.spawn_count;

  infile >> weaponpage->robot_spawn_name;
  infile >> weaponpage->alternate_spawn_name;

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
  infile >> weaponpage->explode_image_name;
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
    infile >> weaponpage->sound_name[i];

  // Read smoke name
  infile >> weaponpage->smoke_image_name;

  // Read scorch data
  infile >> weaponpage->scorch_image_name;
  infile >> weaponpage->weapon_struct.scorch_size;

  // Read icon name
  infile >> weaponpage->icon_name;

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
