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
#include "object_external.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "object_lighting.h"
#include "string_helpers.h"

#include <cstring>
#include <limits>

//-----------------------------------------------------------------------------
// physics_info chunk (generic + ship + weapon pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, physics_info& data)
{
  // Historical quirk preserved: only the z component of velocity is stored.
  return input
         >> data.mass
         >> data.drag
         >> data.full_thrust
         >> reinterpret_cast<uint32_t&>(data.flags)
         >> data.rotdrag
         >> data.full_rotthrust
         >> data.num_bounces
         >> data.velocity.z()
         >> data.rotvel
         >> data.wiggle_amplitude
         >> data.wiggles_per_sec
         >> data.coeff_restitution
         >> data.hit_die_dot
         >> data.max_turnroll_rate
         >> data.turnroll_ratio;
}

byte_ostream& operator<<(byte_ostream& output, const physics_info& data)
{
  return output
         << data.mass
         << data.drag
         << data.full_thrust
         << reinterpret_cast<const uint32_t&>(data.flags)
         << data.rotdrag
         << data.full_rotthrust
         << data.num_bounces
         << data.velocity.z()
         << data.rotvel
         << data.wiggle_amplitude
         << data.wiggles_per_sec
         << data.coeff_restitution
         << data.hit_die_dot
         << data.max_turnroll_rate
         << data.turnroll_ratio;
}

//-----------------------------------------------------------------------------
// light_info chunk (generic + weapon pages)
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, light_info& data)
{
  input
         >> data.light_distance
         >> data.red_light1
         >> data.green_light1
         >> data.blue_light1
         >> data.time_interval
         >> data.flicker_distance
         >> data.directional_dot
         >> data.red_light2
         >> data.green_light2
         >> data.blue_light2
         >> reinterpret_cast<uint32_t&>(data.flags)
         >> data.timebits
         >> data.angle
         >> reinterpret_cast<uint8_t&>(data.lighting_render_type);
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const light_info& data)
{
  return output
         << data.light_distance
         << data.red_light1
         << data.green_light1
         << data.blue_light1
         << data.time_interval
         << data.flicker_distance
         << data.directional_dot
         << data.red_light2
         << data.green_light2
         << data.blue_light2
         << reinterpret_cast<const uint32_t&>(data.flags)
         << data.timebits
         << data.angle
         << reinterpret_cast<const uint8_t&>(data.lighting_render_type);
}

//-----------------------------------------------------------------------------
// Lightmap cleanup for the lightmap_object stream operator.  All storage is
// owned by the container's own vectors, so only the bookkeeping is reset here.
//-----------------------------------------------------------------------------

void ClearObjectLightmaps(object *obj) {
  if (!obj->lm_object.used)
    return;
  obj->lm_object.used = 0;

  for (auto &faces : obj->lm_object.lightmap_faces) {
    for (auto &f : faces) {
      f.u2.clear();
      f.v2.clear();
      f.num_verts = 0;
      f.lmi_handle = std::numeric_limits<uint16_t>::max();
    }
  }
  obj->lm_object.num_models = 0;
  obj->lm_object.num_faces.clear();
  obj->lm_object.lightmap_faces.clear();
}

// Reads one length-prefixed byte string (used for the object's custom default
// script/module names; the engine stores an 8-bit length followed by bytes).
static byte_istream& readByteString(byte_istream& input, std::string& data) {
  uint8_t len = 0;
  input >> len;
  if (len) {
    data.assign(len, '\0');
    input.read(data.data(), len);
  } else {
    data.clear();
  }
  return input;
}

static byte_ostream& writeByteString(byte_ostream& output, const std::string& data) {
  const size_t n = data.size();
  const uint8_t len = static_cast<uint8_t>(n & 0xff);
  output << len;
  if (len)
    output.write(data.data(), len);
  return output;
}

//-----------------------------------------------------------------------------
// object record (level-file OBJS chunk).  This is the current on-disk layout;
// see Descent3/LoadLevel.cpp WriteObject for the writes (ReadObject reads the
// same fields since file version 119, later versions only add makes this block
// the canonical representation).
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, object& data)
{
  input >> reinterpret_cast<uint8_t&>(data.type)
      >> data.id
      >> data.name
      >> reinterpret_cast<uint32_t&>(data.flags);

  if (data.type == object_type_e::door)
  {
    uint16_t shields = 0;
    input >> shields;
    data.shields = static_cast<float>(shields);
  }

  input >> data.roomnum
      >> data.pos
      >> data.orient
      >> reinterpret_cast<uint8_t&>(data.contains_type)
      >> data.contains_id
      >> data.contains_count
      >> data.lifeleft;

  // Sound-source objects carry their sound by name plus a volume.
  if (data.type == object_type_e::soundsource)
  {
    std::string soundname;
    input >> soundname;
    data.ctype.soundsource_info().sound_index = FindSoundName(soundname);
    input >> data.ctype.soundsource_info().volume;
  }

  readByteString(input, data.custom_default_script_name);
  readByteString(input, data.custom_default_module_name);

  input >> data.lm_object;
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const object& data) {
  output << reinterpret_cast<const uint8_t&>(data.type)
         << data.id
         << data.name
         << reinterpret_cast<const uint32_t&>(data.flags);
  if (data.type == object_type_e::door)
    output << static_cast<int16_t>(data.shields);
  output << data.roomnum
         << data.pos
         << data.orient
         << reinterpret_cast<const uint8_t&>(data.contains_type)
         << data.contains_id
         << data.contains_count
         << data.lifeleft;

  if (data.type == object_type_e::soundsource) {
    const std::string &soundname = data.ctype.soundsource_info().sound_index
                                       ? Sounds[*data.ctype.soundsource_info().sound_index].name : "";
    output << soundname
           << data.ctype.soundsource_info().volume;
  }

  writeByteString(output, data.custom_default_script_name);
  writeByteString(output, data.custom_default_module_name);

  output << data.lm_object;
  return output;
}
