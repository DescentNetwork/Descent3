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

// On-disk serialization for the lightmap structs in polymodel_external.h.
// These back the object lightmap block in the level-file OBJS chunk.  Each
// read is the exact mirror of its write, preserving the on-disk field order
// and encodings from the original D3 LoadLevel code.

#include "polymodel_external.h"

byte_istream& operator>>(byte_istream& input, lightmap_object_face& data) {
  uint16_t lmi_handle = 0;
  input >> lmi_handle;
  data.lmi_handle = u16_to_index(lmi_handle);
  input >> data.rvec >> data.uvec;

  uint8_t num_verts = 0;
  input >> num_verts;
  data.num_verts = num_verts;
  data.u2.resize(num_verts);
  data.v2.resize(num_verts);
  input >> data.u2 >> data.v2;

  return input;
}

byte_ostream& operator<<(byte_ostream& output, const lightmap_object_face& data) {
  output << index_to_u16(data.lmi_handle);
  output << data.rvec << data.uvec;
  output << data.num_verts;
  output << data.u2 << data.v2;

  return output;
}

byte_istream& operator>>(byte_istream& input, lightmap_object& data) {
  uint8_t lmdata = 0;
  input >> lmdata;
  data.used = lmdata;

  data.num_faces.clear();
  data.lightmap_faces.clear();
  if (!lmdata)
    return input;

  input >> data.num_models;
  data.num_faces.resize(data.num_models);
  data.lightmap_faces.resize(data.num_models);
  for (int m = 0; m < data.num_models; m++) {
    input >> data.num_faces[m];
    auto& faces = data.lightmap_faces[m];
    faces.resize(data.num_faces[m]);
    input >> faces;
  }

  return input;
}

byte_ostream& operator<<(byte_ostream& output, const lightmap_object& data) {
  if (!data.used)
    return output << static_cast<uint8_t>(0);

  output << static_cast<uint8_t>(1);
  output << data.num_models;
  for (size_t m = 0; m < data.lightmap_faces.size(); m++) {
    output << data.num_faces[m];
    output << data.lightmap_faces[m];
  }

  return output;
}
