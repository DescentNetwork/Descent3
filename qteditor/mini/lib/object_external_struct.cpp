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

#include <cstring>
#include <limits>

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

//-----------------------------------------------------------------------------
// Lightmap cleanup for the per-face u2/v2 arrays the object record loader
// allocates (below).  The container vectors free themselves; only the float
// blocks are heap owned.
//-----------------------------------------------------------------------------

void ClearObjectLightmaps(object *obj) {
  if (!obj->lm_object.used)
    return;
  obj->lm_object.used = 0;

  for (auto &faces : obj->lm_object.lightmap_faces) {
    for (auto &f : faces) {
      delete[] f.u2;
      delete[] f.v2;
      f.u2 = nullptr;
      f.v2 = nullptr;
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

// Reads the lightmap block that follows an object record in a level file
// (present whenever the lmdata byte is non-zero).  The per-face u2/v2 arrays
// are heap-allocated here and released by ClearObjectLightmaps().  format >=
// 58 files store the face rvec/uvec vectors; older ones do not.
static byte_istream& readObjectLightmaps(byte_istream& input, object& data, int format) {
  uint8_t num_models = 0;
  input >> num_models;
  data.lm_object.num_models = num_models;
  data.lm_object.num_faces.clear();
  data.lm_object.lightmap_faces.clear();

  for (int m = 0; m < num_models; m++) {
    int16_t num_faces = 0;
    input >> num_faces;
    data.lm_object.num_faces.push_back(num_faces);
    auto &faces = data.lm_object.lightmap_faces.emplace_back();
    faces.resize(num_faces);
    for (int t = 0; t < num_faces; t++) {
      lightmap_object_face &f = faces[t];
      input >> f.lmi_handle;
      if (format >= 58) {
        if (format <= 59) {
          vector3 pad;
          input >> pad;
        }
        input >> f.rvec >> f.uvec;
      } else {
        f.rvec = {};
        f.uvec = {};
        f.uvec.y() = 1;
      }
      uint8_t nv = 0;
      input >> nv;
      f.num_verts = nv;
      f.u2 = new float[nv];
      f.v2 = new float[nv];
      for (int k = 0; k < nv; k++)
        input >> f.u2[k] >> f.v2[k];
    }
  }
  data.lm_object.used = 1;
  return input;
}

static byte_ostream& writeObjectLightmaps(byte_ostream& output, const object& data) {
  if (!data.lm_object.used)
    return output << static_cast<uint8_t>(0);

  output << static_cast<uint8_t>(1);
  output << data.lm_object.num_models;
  for (size_t m = 0; m < data.lm_object.lightmap_faces.size(); m++) {
    output << static_cast<int16_t>(data.lm_object.num_faces[m]);
    const auto &faces = data.lm_object.lightmap_faces[m];
    for (const lightmap_object_face &f : faces) {
      output << f.lmi_handle << f.rvec << f.uvec;
      output << f.num_verts;
      for (int k = 0; k < f.num_verts; k++)
        output << f.u2[k] << f.v2[k];
    }
  }
  return output;
}

//-----------------------------------------------------------------------------
// object record (level-file OBJS chunk).  This is the current on-disk layout;
// see Descent3/LoadLevel.cpp WriteObject for the writes (ReadObject reads the
// same fields since file version 119, later versions only add makes this block
// the canonical representation).
//-----------------------------------------------------------------------------

byte_istream& operator>>(byte_istream& input, object& data) {
  uint8_t type = 0;
  input >> type;
  data.type = type;
  data.id = 0;
  input >> data.id;
  input >> data.name;
  data.flags = {};
  input >> data.flags;

  if (data.type == OBJ_DOOR) {
    int16_t shields = 0;
    input >> shields;
    data.shields = static_cast<float>(shields);
  }

  input >> data.roomnum;
  input >> data.pos;
  input >> data.orient;

  int8_t c = 0;
  input >> c;
  data.contains_type = c;
  input >> c;
  data.contains_id = c;
  input >> c;
  data.contains_count = c;

  input >> data.lifeleft;

  // Sound-source objects carry their sound by name plus a volume.
  if (data.type == OBJ_SOUNDSOURCE) {
    std::string soundname;
    input >> soundname;
    data.ctype.soundsource_info.sound_index = soundname.empty() ? -1 : FindSoundName(soundname);
    input >> data.ctype.soundsource_info.volume;
  }

  readByteString(input, data.custom_default_script_name);
  readByteString(input, data.custom_default_module_name);

  uint8_t lmdata = 0;
  input >> lmdata;
  if (lmdata)
    readObjectLightmaps(input, data, 119);
  return input;
}

byte_ostream& operator<<(byte_ostream& output, const object& data) {
  output << data.type << data.id << data.name << data.flags;
  if (data.type == OBJ_DOOR)
    output << static_cast<int16_t>(data.shields);
  output << data.roomnum << data.pos << data.orient;
  output << data.contains_type << data.contains_id << data.contains_count << data.lifeleft;

  if (data.type == OBJ_SOUNDSOURCE) {
    const std::string &soundname = (data.ctype.soundsource_info.sound_index < 0)
                                       ? ""
                                       : Sounds[data.ctype.soundsource_info.sound_index].name;
    output << soundname << data.ctype.soundsource_info.volume;
  }

  writeByteString(output, data.custom_default_script_name);
  writeByteString(output, data.custom_default_module_name);

  return writeObjectLightmaps(output, data);
}