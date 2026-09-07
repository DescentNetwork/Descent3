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

// Table-file serialization for the otype_wb_info (weapon battery) chunk type
// embedded in the ship/generic page layouts.  Each read is the exact mirror
// of its write, preserving the on-disk field order and encodings.

#include <cstdint>
#include <cstring>

#include "robotfirestruct.h"

//-----------------------------------------------------------------------------
// otype_wb_info (weapon battery) chunk (generic + ship pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, otype_wb_info& data) {
  input >> data.energy_usage;
  input >> data.ammo_usage;

  for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
    input >> data.gp_weapon_index[j];

  for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
    input >> data.gp_fire_masks[j]
          >> data.gp_fire_wait[j]
          >> data.anim_time[j]
          >> data.anim_start_frame[j]
          >> data.anim_fire_frame[j]
          >> data.anim_end_frame[j];

  input >> data.num_masks;
  input >> data.aiming_gp_index;
  input >> data.aiming_flags;
  input >> data.aiming_3d_dot;
  input >> data.aiming_3d_dist;
  input >> data.aiming_XZ_dot;

  uint16_t raw_flags = 0;
  input >> raw_flags;
  std::memcpy(&data.flags, &raw_flags, sizeof(raw_flags));
  input >> data.gp_quad_fire_mask;

  return input;
}

byte_ostream& operator<<(byte_ostream& output, const otype_wb_info& data) {
  output << data.energy_usage;
  output << data.ammo_usage;

  for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
    output << data.gp_weapon_index[j];

  for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
    output << data.gp_fire_masks[j]
           << data.gp_fire_wait[j]
           << data.anim_time[j]
           << data.anim_start_frame[j]
           << data.anim_fire_frame[j]
           << data.anim_end_frame[j];

  output << data.num_masks
         << data.aiming_gp_index
         << data.aiming_flags
         << data.aiming_3d_dot
         << data.aiming_3d_dist
         << data.aiming_XZ_dot;

  uint16_t raw_flags = 0;
  std::memcpy(&raw_flags, &data.flags, sizeof(raw_flags));
  output << raw_flags;
  output << data.gp_quad_fire_mask;

  return output;
}