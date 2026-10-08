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

// Port of the engine's matcen.cpp serialization (SaveData/LoadData) plus the
// construction/reset state needed to round-trip the MTCN chunk.  The engine's
// runtime production helpers (ComputeCreatePnt/ComputeNextProdInfo/
// CheckActivateStatus, DoThinkFrame/DoRenderFrame, ...) are NOT ported; the
// editor level loader only needs to persist the serialized fields verbatim.

#include "matcen.h"

#include <QtGlobal>

#include <algorithm>

#include "gametexture.h"
#include "objinfo.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "string_helpers.h"

#include <posix_stream.h>

int FindMatcenIndex(const std::string &name) {
  for (uint32_t i = 0; i < static_cast<uint32_t>(Matcen.size()); i++) {
    if (match(Matcen[i].GetName(), name))
      return static_cast<int>(i);
  }
  return MATCEN_ERROR;
}

int CreateMatcen(const std::string &name, bool &f_name_changed) {
  if (Matcen.size() >= MAX_MATCENS)
    return MATCEN_ERROR;

  Matcen.emplace_back();
  f_name_changed = !Matcen.back().SetName(name);

  return static_cast<int>(Matcen.size() - 1);
}

void InitMatcens() { Matcen.clear(); }

bool MatcenValid(int32_t id) {
  return id >= 0 && id < static_cast<int32_t>(Matcen.size());
}

void InitMatcensForLevel() {
  for (matcen &m : Matcen) {
    m.Reset();
  }
}

void DestroyMatcen(int32_t id, bool f_resort) {
  if (!MatcenValid(id))
    return;

  if (f_resort) {
    // Drop the entry and shift the tail down so the table stays dense: the
    // matcen table is rebuilt wholesale on load and is indexed by position.
    Matcen.erase(Matcen.begin() + id);
  } else {
    Matcen[id].Reset();
  }
}

void DestroyAllMatcens() {
  Matcen.clear();
}

matcen::matcen() {
  m_prod_mode = matcen_prod_mode::notprod;
  m_cur_saturation_count = 0;

  m_num_prod_types = 0;
  m_control_type = matcen_control_type::script;
  m_type = matcen_type::unassigned;
  m_creation_effect = matcen_effect::line_lightning;

  // Get lightning handle in case we need it
  m_creation_texture = FindTextureName("Matcen Lightning").value_or(0);

  m_num_spawn_pnts = 0;

  m_roomnum = MATCEN_ERROR;

  m_spawn_pnt.assign(MAX_SPAWN_PNTS, index_t{});
  m_spawn_vec.assign(MAX_SPAWN_PNTS, vector3{});
  m_spawn_normal.assign(MAX_SPAWN_PNTS, vector3{});

  m_max_prod = 0;

  m_prod_type.assign(MAX_PROD_TYPES, index_t{});
  m_prod_time.assign(MAX_PROD_TYPES, 1.0f);
  m_prod_priority.assign(MAX_PROD_TYPES, 100);
  m_max_prod_type.assign(MAX_PROD_TYPES, 3);

  m_sounds.push_back(FindSoundName("AmbMatCenRun"));
  m_sounds.push_back(index_t{});
  m_sounds.push_back(FindSoundName("AmbMatCenProduce"));

  m_speed_multi = 1.0f;

  m_max_alive_children = -1;
  m_num_alive = 0;

  m_preprod_time = 1.5f;
  m_postprod_time = 1.0f;

  m_status = matcen_status_flags_t{};

  m_create_pnt = vector3{};
  m_create_room = MATCEN_ERROR;

  m_cached_prod_index = -1;
  m_cached_prod_time = 0.0f;

  Reset();
}

matcen::~matcen() {}

void matcen::Reset() {
  // Serialization-relevant subset of the engine's Reset(); the engine also
  // re-runs its gameplay helpers ComputeCreatePnt/ComputeNextProdInfo, which
  // the mini does not port.  Reset() is deliberately NOT called before
  // SaveData: the values stored in a saved level were already reset by the
  // engine at the time that file was written, so re-saving the loaded state
  // verbatim keeps the chunk byte-stable.
  m_status.done_prod = false;
  m_status.active = false;
  m_status.active_pause = false;
  m_status.prod_till_done = false;
  m_status.prod_one_pause = false;
  m_status.prod_one_disable = false;

  m_num_prod = 0;
  m_last_prod_type_index = MATCEN_ERROR;
  m_last_prod_finish_time = 0.0f;

  m_last_active_check_result = false;
  m_next_active_check_time = 0.0f;

  m_prod_mode_time = 0.0f;

  std::fill(m_num_prod_type.begin(), m_num_prod_type.end(), 0);

  m_last_prod_objref = OBJECT_HANDLE_NONE;

  if (m_prod_mode == matcen_prod_mode::notprod) {
    m_cached_prod_index = -1;
    m_cached_prod_time = 0.0f;
  }
}

std::string matcen::GetName(void) { return m_name; }

bool matcen::SetName(const std::string &name) {
  if (name.size() < MAX_MATCEN_NAME_LEN) {
    m_name = name;
    return true;
  }

  return false;
}

// Reads one length-prefixed, NUL-terminated C-style name field: a short byte
// count followed by that many raw bytes.  Always consumes exactly the declared
// byte count so the enclosing record stays aligned.  The string is truncated
// at the first embedded NUL because writers terminate names with one.
static std::string readLevelName(posix_istream &ifile) {
  int16_t len = 0;
  ifile >> len;

  if (len <= 0)
    return std::string();

  // Strings are writer-generated (strlen + 1, so realistically < 32 bytes);
  // a larger value indicates corrupt data but must still be consumed to keep
  // the record aligned.
  const size_t count = static_cast<size_t>(len);
  std::string raw(count, '\0');
  ifile.read(&raw[0], count);

  return std::string(raw.c_str());
}

void matcen::SaveData(posix_ostream &ofile) const {
  ofile << static_cast<int32_t>(MATCEN_LOADSAVE_VERSION);

  ofile << static_cast<int16_t>(m_spawn_pnt.size());
  ofile << static_cast<int16_t>(m_prod_type.size());
  ofile << static_cast<int16_t>(m_sounds.size());

  ofile << static_cast<int16_t>(m_name.size() + 1) << m_name;

  ofile << static_cast<int8_t>(m_num_prod_types);
  ofile << static_cast<int8_t>(m_control_type);
  ofile << static_cast<int8_t>(m_type);
  ofile << static_cast<int8_t>(m_creation_effect);
  ofile << m_creation_texture; // int16_t

  ofile << static_cast<int32_t>(m_num_spawn_pnts);
  ofile << static_cast<int32_t>(m_roomnum);

  ofile << m_create_pnt;
  ofile << static_cast<int32_t>(m_create_room);

  // The original engine writer duplicates x() for the z component of both
  // spawn vectors and normals (a long-standing bug); the corrected z() is
  // written here.  Load+Save is still an identity because LoadData stores the
  // stored z value and SaveData writes that same value back.
  for (size_t i = 0; i < m_spawn_pnt.size(); i++) {
    ofile << static_cast<int32_t>(index_to_int(m_spawn_pnt[i]))
          << m_spawn_vec[i]
          << m_spawn_normal[i];
  }

  ofile << static_cast<int32_t>(m_max_prod);

  for (size_t i = 0; i < m_prod_type.size(); i++) {
    const index_t type = m_prod_type[i];
    if (type && *type < MAX_OBJECT_IDS) {
      ofile << static_cast<int16_t>(Object_info[*type].name.size() + 1) << Object_info[*type].name;
    } else {
      ofile << static_cast<int16_t>(1);
      ofile.put(0);
    }

    ofile << m_prod_time[i];
    ofile << static_cast<int32_t>(m_prod_priority[i]);
    ofile << static_cast<int32_t>(m_max_prod_type[i]);
  }

  ofile << m_max_alive_children;
  if (m_max_alive_children > 0) {
    ofile << m_num_alive;
    for (int i = 0; i < m_num_alive && i < static_cast<int>(m_alive_list.size()); i++)
      ofile << static_cast<int32_t>(m_alive_list[i]);
  }

  ofile << m_preprod_time << m_postprod_time;

  // Convert these to names
  for (size_t i = 0; i < m_sounds.size(); i++) {
    if (!m_sounds[i]) {
      ofile << static_cast<int16_t>(0);
      continue;
    }
    ofile << static_cast<int16_t>(Sounds[*m_sounds[i]].name.size() + 1) << Sounds[*m_sounds[i]].name;
  }

  ofile << m_speed_multi;

  ofile << static_cast<uint8_t>(m_prod_mode);
  ofile << m_prod_mode_time;

  ofile << reinterpret_cast<const int32_t &>(m_status);
  ofile << static_cast<int32_t>(m_num_prod);
  ofile << static_cast<int32_t>(m_last_prod_type_index);
  ofile << m_last_prod_finish_time;

  ofile << static_cast<int32_t>(m_cached_prod_index);
  ofile << m_cached_prod_time;

  ofile << m_next_active_check_time;
  ofile << static_cast<uint8_t>(m_last_active_check_result ? 1 : 0);

  ofile << static_cast<int32_t>(m_last_prod_objref);

  for (size_t i = 0; i < m_num_prod_type.size(); i++)
    ofile << static_cast<int32_t>(m_num_prod_type[i]);

  ofile << static_cast<int32_t>(m_sound_active_handle);
}

void matcen::LoadData(posix_istream &ifile, const int *texture_xlate) {
  int32_t version = 0;
  ifile >> version;

  int16_t max_spawn_pnts = 0;
  int16_t max_prod_types = 0;
  int16_t max_matcen_sounds = 0;
  ifile >> max_spawn_pnts >> max_prod_types >> max_matcen_sounds;

  // The engine asserts these against its compile-time maxima; clamp instead so
  // corrupt files cannot overrun the fixed-size arrays.
  if (max_spawn_pnts < 0) max_spawn_pnts = 0;
  if (max_spawn_pnts > MAX_SPAWN_PNTS) max_spawn_pnts = MAX_SPAWN_PNTS;
  if (max_prod_types < 0) max_prod_types = 0;
  if (max_prod_types > MAX_PROD_TYPES) max_prod_types = MAX_PROD_TYPES;
  if (max_matcen_sounds < 0) max_matcen_sounds = 0;
  if (max_matcen_sounds > static_cast<int32_t>(matcen_sound::count))
    max_matcen_sounds = static_cast<int32_t>(matcen_sound::count);

  m_name = readLevelName(ifile);

  ifile >> reinterpret_cast<int8_t &>(m_num_prod_types);
  ifile >> reinterpret_cast<int8_t &>(m_control_type);
  ifile >> reinterpret_cast<int8_t &>(m_type);
  ifile >> reinterpret_cast<int8_t &>(m_creation_effect);

  if (version >= 2) {
    int16_t raw = 0;
    ifile >> raw;
    m_creation_texture = (raw >= 0 && raw < MAX_TEXTURES) ? static_cast<int16_t>(texture_xlate[raw]) : 0;
  }

  ifile >> m_num_spawn_pnts;

  m_roomnum = 0;
  ifile >> m_roomnum;

  ifile >> m_create_pnt.x() >> m_create_pnt.y() >> m_create_pnt.z();

  ifile >> m_create_room;

  m_spawn_pnt.clear();
  m_spawn_vec.clear();
  m_spawn_normal.clear();
  for (int i = 0; i < max_spawn_pnts; i++) {
    int32_t roomnum = MATCEN_ERROR;
    ifile >> roomnum;
    m_spawn_pnt.push_back(roomnum < 0 ? index_t{} : index_t{static_cast<uint32_t>(roomnum)});

    vector3 spawn_vec{};
    ifile >> spawn_vec.x() >> spawn_vec.y() >> spawn_vec.z();
    m_spawn_vec.push_back(spawn_vec);

    vector3 spawn_normal{};
    ifile >> spawn_normal.x() >> spawn_normal.y() >> spawn_normal.z();
    m_spawn_normal.push_back(spawn_normal);
  }

  ifile >> m_max_prod;

  m_prod_type.clear();
  m_prod_time.clear();
  m_prod_priority.clear();
  m_max_prod_type.clear();
  for (int i = 0; i < max_prod_types; i++) {
    const std::string prod_name = readLevelName(ifile);
    m_prod_type.push_back(FindObjectIDName(prod_name));

    float prod_time = 0.0f;
    int prod_priority = 0;
    int max_prod = 0;
    ifile >> prod_time >> prod_priority >> max_prod;
    m_prod_time.push_back(prod_time);
    m_prod_priority.push_back(prod_priority);
    m_max_prod_type.push_back(max_prod);
  }

  ifile >> m_max_alive_children;
  if (m_max_alive_children > 0) {
    ifile >> m_num_alive;
    if (m_num_alive < 0) m_num_alive = 0;
    if (m_num_alive > m_max_alive_children) m_num_alive = m_max_alive_children;

    m_alive_list.assign(m_max_alive_children, 0);
    for (int i = 0; i < m_num_alive; i++) {
      ifile >> m_alive_list[i];
    }
  } else {
    m_num_alive = 0;
    m_alive_list.clear();
  }

  ifile >> m_preprod_time >> m_postprod_time;

  // Convert these to names
  m_sounds.clear();
  for (int i = 0; i < max_matcen_sounds; i++) {
    const std::string sound_name = readLevelName(ifile);
    m_sounds.push_back(FindSoundName(sound_name));
  }

  ifile >> m_speed_multi;

  ifile >> reinterpret_cast<uint8_t &>(m_prod_mode);
  ifile >> m_prod_mode_time;

  ifile >> reinterpret_cast<int32_t &>(m_status);
  ifile >> m_num_prod;
  ifile >> m_last_prod_type_index;
  ifile >> m_last_prod_finish_time;

  ifile >> m_cached_prod_index;
  ifile >> m_cached_prod_time;

  ifile >> m_next_active_check_time;
  int8_t active = 0;
  ifile >> active;
  m_last_active_check_result = (active != 0);

  ifile >> m_last_prod_objref;

  m_num_prod_type.clear();
  for (int i = 0; i < max_prod_types; i++) {
    int num_prod_type = 0;
    ifile >> num_prod_type;
    m_num_prod_type.push_back(num_prod_type);
  }

  if (version >= 3) {
    ifile >> m_sound_active_handle;
  }
}
