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

// On-disk order (SHIPPAGE_VERSION 6) is: version, name, cockpit_name,
// hud_config_name, image_name, dying_image_name, med_image_name, lo_image_name,
// med_lod_distance, lo_lod_distance, phys_info, size, armor_scalar, flags,
// then per-weapon: fire_flags, firing_sound_name, release_sound_name,
// spew_powerup_name, max_ammo, static_wb, [gunpoints] fire_sound_name,
// [masks] weapon_name.

byte_istream& operator>>(byte_istream& input, mngs_ship_page& data) {
  int16_t version = 0;
  input >> version;

  input >> data.ship_struct.name
        >> data.ship_struct.cockpit_name
        >> data.ship_struct.hud_config_name
        >> data.image_name
        >> data.dying_image_name
        >> data.med_image_name
        >> data.lo_image_name
        >> data.ship_struct.med_lod_distance
        >> data.ship_struct.lo_lod_distance
        >> data.ship_struct.phys_info
        >> data.ship_struct.size
        >> data.ship_struct.armor_scalar
        >> data.ship_struct.flags;

  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    input >> data.ship_struct.fire_flags[i]
          >> data.firing_sound_name[i]
          >> data.release_sound_name[i]
          >> data.spew_powerup_name[i]
          >> data.ship_struct.max_ammo[i]
          >> data.ship_struct.static_wb[i];

    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      input >> data.fire_sound_name[i][j];

    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      input >> data.weapon_name[i][j];
  }

  return input;
}

byte_ostream& operator<<(byte_ostream& output, const mngs_ship_page& data) {
  output << static_cast<int16_t>(SHIPPAGE_VERSION);

  output << data.ship_struct.name
         << data.ship_struct.cockpit_name
         << data.ship_struct.hud_config_name
         << data.image_name
         << data.dying_image_name
         << data.med_image_name
         << data.lo_image_name
         << data.ship_struct.med_lod_distance
         << data.ship_struct.lo_lod_distance
         << data.ship_struct.phys_info
         << data.ship_struct.size
         << data.ship_struct.armor_scalar
         << data.ship_struct.flags;

  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    output << data.ship_struct.fire_flags[i]
           << data.firing_sound_name[i]
           << data.release_sound_name[i]
           << data.spew_powerup_name[i]
           << data.ship_struct.max_ammo[i]
           << data.ship_struct.static_wb[i];

    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      output << data.fire_sound_name[i][j];

    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      output << data.weapon_name[i][j];
  }

  return output;
}

int mng_ReadNewShipPage(posix_istream &infile, mngs_ship_page *shippage) {
  // Clear the page record.  The embedded ship holds std::string members, so
  // use member-wise reset rather than memset (which would corrupt them).
  *shippage = mngs_ship_page{};

  infile >> *shippage;

  // Mark the newly filled structure as used
  shippage->ship_struct.used = 1;

  // Bash Fusion recharge times for the ships
  if (shippage->ship_struct.name == "Pyro-GL") {
    // Pyro-GL
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.66f;
  } else if (shippage->ship_struct.name == "Phoenix") {
    // Phoenix
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 0.792f;
  } else if (shippage->ship_struct.name == "Magnum-AHT") {
    // Magnum
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      shippage->ship_struct.static_wb[FUSION_INDEX].gp_fire_wait[j] = 1.122f;
  } else {
    // Any other ship: nothing to bash.
  }

  return 1; // successfully read
}

int mng_ReadShipPage(posix_istream &infile, mngs_ship_page *shippage) {
  if (!Old_table_method)
    return mng_ReadNewShipPage(infile, shippage);
  return 0; // old command-based table not supported in mini build
}
