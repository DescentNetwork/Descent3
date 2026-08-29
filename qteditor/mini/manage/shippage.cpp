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

// Ship page reader (ported from shippage.cpp : 539-611).

#include "shippage.h"

#include <cstdint>
#include <string>

#include "manage.h"
#include "weapon_external.h" // FUSION_INDEX
#include "gamedata_helpers.h"

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

  // ship.name is a variable-length NUL-terminated string on disk (std::string).
  infile >> shippage->ship_struct.name;

  infile >> shippage->ship_struct.cockpit_name;
  infile >> shippage->ship_struct.hud_config_name;

  // Read in model names
  infile >> shippage->image_name;
  infile >> shippage->dying_image_name;
  infile >> shippage->med_image_name;
  infile >> shippage->lo_image_name;

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
    infile >> shippage->firing_sound_name[i];
    infile >> shippage->release_sound_name[i];
    infile >> shippage->spew_powerup_name[i];
    infile >> shippage->ship_struct.max_ammo[i];

    if (version >= 6)
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 2);
    else
      mng_ReadWeaponBatteryChunk(&shippage->ship_struct.static_wb[i], infile, 1);

    for (j = 0; j < MAX_WB_GUNPOINTS; j++)
      infile >> shippage->fire_sound_name[i][j];

    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      infile >> shippage->weapon_name[i][j];
  }

  // Mark the newly filled structure as used
  shippage->ship_struct.used = 1;

  // Bash Fusion recharge times for the ships
  if (shippage->ship_struct.name == "Pyro-GL") {
    // Pyro-GL
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.66f;
  } else if (shippage->ship_struct.name == "Phoenix") {
    // Phoenix
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.792f;
  } else if (shippage->ship_struct.name == "Magnum-AHT") {
    // Magnum
    for (j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 1.122f;
  } else {
    std::runtime_error("ship not found!");
  }

  return 1; // successfully read
}

int mng_ReadShipPage(posix_istream &infile, mngs_ship_page *shippage) {
  if (!Old_table_method)
    return mng_ReadNewShipPage(infile, shippage);
  return 0; // old command-based table not supported in mini build
}
