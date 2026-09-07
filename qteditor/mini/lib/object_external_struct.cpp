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

// Table-file serialization for the physics_info / light_info chunk types
// embedded in the mngs_*_page layouts.  Each read is the exact mirror of its
// write, preserving the on-disk field order and encodings from the original
// D3 manage module so pages round-trip bit-for-bit.

#include "object_external_struct.h"

#include <cstring>

//-----------------------------------------------------------------------------
// physics flags bitfield (stored as a single little-endian uint32 on disk)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, physics_flags_t& data) {
  uint32_t host = 0;
  input.read(&host, sizeof(host));
  host = le_to_host(host);
  std::memcpy(&data, &host, sizeof(data));
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const physics_flags_t& data) {
  uint32_t host = 0;
  std::memcpy(&host, &data, sizeof(data));
  host = host_to_le(host);
  return output.write(&host, sizeof(host));
}

//-----------------------------------------------------------------------------
// physics_info chunk (generic + ship + weapon pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, physics_info& data) {
  // Historical quirk preserved: only the z component of velocity is stored.
  input >> data.mass >> data.drag >> data.full_thrust >> data.flags >> data.rotdrag >> data.full_rotthrust >>
      data.num_bounces >> data.velocity.z();
  float a, b, c;
  input >> a >> b >> c;
  data.rotvel = {a, b, c};
  return input >> data.wiggle_amplitude >> data.wiggles_per_sec >> data.coeff_restitution >> data.hit_die_dot >>
         data.max_turnroll_rate >> data.turnroll_ratio;
}

byte_ostream& operator<<(byte_ostream& output, const physics_info& data) {
  return output << data.mass << data.drag << data.full_thrust << data.flags << data.rotdrag << data.full_rotthrust <<
         data.num_bounces << data.velocity.z() << data.rotvel.x() << data.rotvel.y() << data.rotvel.z() <<
         data.wiggle_amplitude << data.wiggles_per_sec << data.coeff_restitution << data.hit_die_dot <<
         data.max_turnroll_rate << data.turnroll_ratio;
}

//-----------------------------------------------------------------------------
// light_info chunk (generic + weapon pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, light_info& data) {
  return input >> data.light_distance >> data.red_light1 >> data.green_light1 >> data.blue_light1 >>
         data.time_interval >> data.flicker_distance >> data.directional_dot >> data.red_light2 >> data.green_light2 >>
         data.blue_light2 >> data.flags >> data.timebits >> data.angle >> data.lighting_render_type;
}

byte_ostream& operator<<(byte_ostream& output, const light_info& data) {
  return output << data.light_distance << data.red_light1 << data.green_light1 << data.blue_light1 <<
         data.time_interval << data.flicker_distance << data.directional_dot << data.red_light2 << data.green_light2 <<
         data.blue_light2 << data.flags << data.timebits << data.angle << data.lighting_render_type;
}