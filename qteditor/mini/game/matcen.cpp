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

// Port of the engine's matcen.cpp: the serialization (SaveData/LoadData), the
// construction/reset state and the runtime production helpers.

#include "matcen.h"

#include <QtGlobal>

#include <algorithm>
#include <utility>

#include "chrono_timer.h"
#include "damage_external.h"
#include "fireball.h"
#include "fireball_external.h"
#include "findintersection.h"
#include "gametexture.h"
#include "grdefs.h"
#include "hlsoundlib.h"
#include "object.h"
#include "object_external_struct.h"
#include "objinfo.h"
#include "ObjScript.h"
#include "physics.h"
#include "polymodel.h"
#include "rand.h"
#include "room.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "string_helpers.h"
#include "terrain.h"
#include "viseffect.h"
#include "weapon.h"

#include <posix_stream.h>

namespace {

// matcen_status_flags_t is a packed bitfield struct; the engine manipulated the
// status word with C bit macros, so the port works on the underlying word.
uint32_t status_bits(const matcen_status_flags_t &status) { return reinterpret_cast<const uint32_t &>(status); }

matcen_status_flags_t status_from_bits(uint32_t bits) {
  matcen_status_flags_t status{};
  reinterpret_cast<uint32_t &>(status) = bits;
  return status;
}

matcen_status_flags_t status_done_never_mask() {
  matcen_status_flags_t mask{};
  mask.done_prod = true;
  mask.never_prod = true;
  return mask;
}

matcen_status_flags_t status_active_mask() {
  matcen_status_flags_t mask{};
  mask.active = true;
  return mask;
}

constexpr float MATCEN_DAMAGE_PER_SECOND = 20.0f;
constexpr float MATCEN_DAMAGE_DIST = 10.0f;
constexpr float MATCEN_FORCE = 60000.0f;

const vector3 g_zero_vector{};

} // namespace

bool Matcen_created = false;

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
  m_spawn_vis_effects = {};

  m_max_prod = 0;

  m_prod_type.assign(MAX_PROD_TYPES, index_t{});
  m_prod_time.assign(MAX_PROD_TYPES, 1.0f);
  m_prod_priority.assign(MAX_PROD_TYPES, 100);
  m_max_prod_type.assign(MAX_PROD_TYPES, 3);

  // The engine clears the whole per type production counter in Reset().
  m_num_prod_type.assign(MAX_PROD_TYPES, 0);

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
  m_create_room = index_t{};

  m_cached_prod_index = -1;
  m_cached_prod_time = 0.0f;

  m_sound_active_handle = 0;

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

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

void matcen::SetCreationTexture(int16_t texnum) { m_creation_texture = texnum; }

int16_t matcen::GetCreationTexture() { return m_creation_texture; }

matcen_type matcen::GetAttachType() { return m_type; }

bool matcen::SetAttachType(matcen_type type) {
  if (type == matcen_type::object || type == matcen_type::room || type == matcen_type::unassigned) {
    m_type = type;
    return true;
  }

  return false;
}

matcen_control_type matcen::GetControlType() { return m_control_type; }

bool matcen::SetControlType(matcen_control_type type) {
  if (type >= matcen_control_type::script && type < matcen_control_type::count) {
    m_control_type = type;
    return true;
  }

  return false;
}

int32_t matcen::GetAttach() { return m_roomnum; }

bool matcen::SetAttach(int32_t attach) {
  m_roomnum = attach;
  ComputeCreatePnt();

  return true;
}

bool matcen::GetCreatePnt(vector3 &pnt) {
  ComputeCreatePnt();
  pnt = m_create_pnt;
  return true;
}

bool matcen::SetCreatePnt(const vector3 &pnt) {
  if (m_status.manual_update_create_pnt) {
    m_create_pnt = pnt;
    return true;
  }

  return false;
}

index_t matcen::GetCreateRoom() {
  ComputeCreatePnt();

  return m_create_room;
}

bool matcen::SetCreateRoom(int32_t room) {
  if (m_status.manual_update_create_pnt) {
    m_create_room = to_roomnum(room);
    return true;
  }

  return false;
}

int32_t matcen::GetNumSpawnPnts() { return m_num_spawn_pnts; }

bool matcen::SetNumSpawnPnts(int32_t num_s) {
  if (num_s >= 0 && num_s <= MAX_SPAWN_PNTS) {
    m_num_spawn_pnts = num_s;
    ComputeCreatePnt();
    return true;
  }

  return false;
}

int32_t matcen::GetSpawnPnt(int8_t s_index) {
  if (s_index >= 0 && s_index < MAX_SPAWN_PNTS && s_index < static_cast<int8_t>(m_spawn_pnt.size()))
    return index_to_int(m_spawn_pnt[s_index]);

  return MATCEN_ERROR;
}

bool matcen::SetSpawnPnt(int8_t s_index, int32_t s_value) {
  if (s_index >= 0 && s_index < MAX_SPAWN_PNTS && s_index < static_cast<int8_t>(m_spawn_pnt.size())) {
    m_spawn_pnt[s_index] = (s_value < 0) ? index_t{} : index_t{static_cast<uint32_t>(s_value)};
    ComputeCreatePnt();
    return true;
  }

  return false;
}

int32_t matcen::GetMaxProd() { return m_max_prod; }

bool matcen::SetMaxProd(int32_t max_p) {
  if (max_p < -1) {
    return false;
  }

  m_max_prod = max_p;

  ComputeNextProdInfo();

  return true;
}

int32_t matcen::GetNumProdTypes() { return m_num_prod_types; }

bool matcen::SetNumProdTypes(int32_t num_prod_types) {
  if (num_prod_types >= 0 && num_prod_types <= MAX_PROD_TYPES) {
    if (m_num_prod_types < num_prod_types && m_status.done_prod) {
      ComputeNextProdInfo();
    }

    m_num_prod_types = static_cast<int8_t>(num_prod_types);
    return true;
  }

  return false;
}

bool matcen::GetProdInfo(int8_t index, optref<int32_t> type_id, optref<int32_t> priority, optref<float> time,
                         optref<int32_t> max_prod) {
  if (index >= 0 && index < MAX_PROD_TYPES && index < static_cast<int8_t>(m_prod_type.size())) {
    if (type_id)
      *type_id = index_to_int(m_prod_type[index]);

    if (priority)
      *priority = m_prod_priority[index];

    if (time)
      *time = m_prod_time[index];

    if (max_prod)
      *max_prod = m_max_prod_type[index];

    return true;
  }

  return false;
}

bool matcen::SetProdInfo(int8_t index, optref<int32_t> type_id, optref<int32_t> priority, optref<float> time,
                         optref<int32_t> max_prod) {
  if (index >= 0 && index < MAX_PROD_TYPES && index < static_cast<int8_t>(m_prod_type.size())) {
    if (type_id && (*type_id >= -1))
      m_prod_type[index] = (*type_id < 0) ? index_t{} : index_t{static_cast<uint32_t>(*type_id)};

    if (priority && (*priority >= 0))
      m_prod_priority[index] = *priority;

    if (time && (*time >= 0.0f))
      m_prod_time[index] = *time;

    if (max_prod && (*max_prod >= -1)) {
      if (*max_prod == -1 && m_max_prod_type[index] < *max_prod && m_status.done_prod) {
        ComputeNextProdInfo();
      }

      m_max_prod_type[index] = *max_prod;
    }

    return true;
  }

  return false;
}

float matcen::GetProdMultiplier() { return m_speed_multi; }

bool matcen::SetProdMultiplier(float multi) {
  if (multi >= 0.0f) {
    m_speed_multi = multi;
    return true;
  }

  return false;
}

matcen_status_flags_t matcen::GetStatus() { return m_status; }

bool matcen::SetStatus(matcen_status_flags_t status, bool f_enable) {
  const uint32_t bits = status_bits(status) & ~status_bits(status_done_never_mask());
  m_last_active_check_result = m_status.active;

  if (f_enable) {
    bool f_recompute = false;

    if ((m_prod_mode == matcen_prod_mode::notprod) && ((bits & status_bits(status_active_mask())) != 0) &&
        !m_status.active) {
      f_recompute = true;
    }

    m_status = status_from_bits(status_bits(m_status) | bits);

    if (f_recompute) {
      ComputeNextProdInfo();
    }
  } else {
    m_status = status_from_bits(status_bits(m_status) & ~bits);
  }

  UpdateActiveSound();

  return true;
}

matcen_effect matcen::GetCreationEffect() { return m_creation_effect; }

bool matcen::SetCreationEffect(matcen_effect effect) {
  if (effect < matcen_effect::line_lightning || effect >= matcen_effect::count) {
    return false;
  }

  m_creation_effect = effect;

  switch (m_creation_effect) {
  case matcen_effect::line_lightning:
    m_preprod_time = 1.5f;
    m_postprod_time = 1.0f;
    break;
  case matcen_effect::line_sine_wave:
    m_preprod_time = 0.25f;
    m_postprod_time = 0.0f;
    break;
  default:
    m_preprod_time = 1.5f;
    m_postprod_time = 1.0f;
    break;
  }

  return true;
}

int32_t matcen::GetMaxAliveChildren() { return m_max_alive_children; }

bool matcen::SetMaxAliveChildren(int32_t max_alive) {
  if (max_alive >= -1 && max_alive != 0 && max_alive <= MAX_MATCEN_ALIVE_CHILDREN) {
    std::vector<int32_t> temp;

    if (max_alive > 0) {
      temp.assign(static_cast<size_t>(max_alive), 0);
    }

    if (!m_alive_list.empty() && !temp.empty()) {
      m_num_alive = static_cast<int16_t>((max_alive < m_num_alive) ? max_alive : m_num_alive);

      for (int32_t i = 0; i < m_num_alive; i++) {
        temp[i] = m_alive_list[i];
      }
    } else {
      m_num_alive = 0;
    }

    m_alive_list = temp;
    m_max_alive_children = static_cast<int16_t>(max_alive);

    return true;
  }

  return false;
}

float matcen::GetPreProdTime() { return m_preprod_time; }

bool matcen::SetPreProdTime(float time) {
  const float upper_bound = 2.0f;

  if (time >= 0.0f && time <= upper_bound) {
    m_preprod_time = time;
    return true;
  }

  return false;
}

float matcen::GetPostProdTime() { return m_postprod_time; }

bool matcen::SetPostProdTime(float time) {
  const float upper_bound = 2.0f;

  if (time >= 0.0f && time <= upper_bound) {
    m_postprod_time = time;
    return true;
  }

  return false;
}

index_t matcen::GetSound(int8_t sound_type) {
  if (sound_type >= 0 && sound_type < static_cast<int8_t>(m_sounds.size()))
    return m_sounds[sound_type];

  return index_t{};
}

bool matcen::SetSound(int8_t sound_type, index_t sound_index) {
  if (sound_type >= 0 && sound_type < static_cast<int8_t>(m_sounds.size())) {
    m_sounds[sound_type] = sound_index;
  }

  return false;
}

// Reads one length-prefixed, NUL-terminated C-style name field: a short byte
// count followed by that many raw bytes.  Always consumes exactly the declared
// byte count so the enclosing record stays aligned.  The string is truncated
// at the first embedded NUL because writers terminate names with one.
static std::string readLevelName(byte_istream &ifile) {
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

void matcen::SaveData(byte_ostream &ofile) const {
  ofile << static_cast<int32_t>(MATCEN_LOADSAVE_VERSION);

  ofile << static_cast<int16_t>(MAX_SPAWN_PNTS);
  ofile << static_cast<int16_t>(MAX_PROD_TYPES);
  ofile << static_cast<int16_t>(std::to_underlying(matcen_sound::count));

  ofile << static_cast<int16_t>(m_name.size() + 1) << m_name;

  ofile << static_cast<int8_t>(m_num_prod_types);
  ofile << std::to_underlying(m_control_type);
  ofile << std::to_underlying(m_type);
  ofile << std::to_underlying(m_creation_effect);
  ofile << m_creation_texture; // int16_t

  ofile << static_cast<int32_t>(m_num_spawn_pnts);
  ofile << static_cast<int32_t>(m_roomnum);

  ofile << m_create_pnt;
  ofile << static_cast<int32_t>(from_roomnum(m_create_room));

  // The original engine writer duplicates x() for the z component of both
  // spawn vectors and normals (a long-standing bug); the corrected z() is
  // written here.  Load+Save is still an identity because LoadData stores the
  // stored z value and SaveData writes that same value back.
  for (size_t i = 0; i < MAX_SPAWN_PNTS; i++) {
    const index_t spawn_pnt = (i < m_spawn_pnt.size()) ? m_spawn_pnt[i] : index_t{};
    const vector3 &spawn_vec = (i < m_spawn_vec.size()) ? m_spawn_vec[i] : g_zero_vector;
    const vector3 &spawn_normal = (i < m_spawn_normal.size()) ? m_spawn_normal[i] : g_zero_vector;

    ofile << static_cast<int32_t>(index_to_int(spawn_pnt)) << spawn_vec << spawn_normal;
  }

  ofile << static_cast<int32_t>(m_max_prod);

  for (size_t i = 0; i < MAX_PROD_TYPES; i++) {
    const index_t type = (i < m_prod_type.size()) ? m_prod_type[i] : index_t{};

    if (type && *type < Object_info.size()) {
      ofile << static_cast<int16_t>(Object_info[*type].name.size() + 1) << Object_info[*type].name;
    } else {
      ofile << static_cast<int16_t>(1);
      ofile.put(0);
    }

    const float prod_time = (i < m_prod_time.size()) ? m_prod_time[i] : 1.0f;
    const int32_t prod_priority = (i < m_prod_priority.size()) ? m_prod_priority[i] : 100;
    const int32_t max_prod = (i < m_max_prod_type.size()) ? m_max_prod_type[i] : 3;

    ofile << prod_time;
    ofile << static_cast<int32_t>(prod_priority);
    ofile << static_cast<int32_t>(max_prod);
  }

  ofile << m_max_alive_children;
  if (m_max_alive_children > 0) {
    ofile << m_num_alive;
    for (int i = 0; i < m_num_alive && i < static_cast<int>(m_alive_list.size()); i++)
      ofile << static_cast<int32_t>(m_alive_list[i]);
  }

  ofile << m_preprod_time << m_postprod_time;

  // Convert these to names
  for (size_t i = 0; i < static_cast<size_t>(matcen_sound::count); i++) {
    if (i >= m_sounds.size() || !m_sounds[i]) {
      ofile << static_cast<int16_t>(0);
      continue;
    }
    ofile << static_cast<int16_t>(Sounds[*m_sounds[i]].name.size() + 1) << Sounds[*m_sounds[i]].name;
  }

  ofile << m_speed_multi;

  ofile << std::to_underlying(m_prod_mode);
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

  for (size_t i = 0; i < MAX_PROD_TYPES; i++)
    ofile << static_cast<int32_t>((i < m_num_prod_type.size()) ? m_num_prod_type[i] : 0);

  ofile << static_cast<int32_t>(m_sound_active_handle);
}

void matcen::LoadData(byte_istream &ifile, const int *texture_xlate) {
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
  if (max_matcen_sounds > static_cast<int32_t>(std::to_underlying(matcen_sound::count)))
    max_matcen_sounds = static_cast<int32_t>(std::to_underlying(matcen_sound::count));

  m_name = readLevelName(ifile);

  ifile >> reinterpret_cast<int8_t &>(m_num_prod_types);
  if (m_num_prod_types < 0) m_num_prod_types = 0;
  if (m_num_prod_types > MAX_PROD_TYPES) m_num_prod_types = MAX_PROD_TYPES;
  ifile >> reinterpret_cast<int8_t &>(m_control_type);
  ifile >> reinterpret_cast<int8_t &>(m_type);
  ifile >> reinterpret_cast<int8_t &>(m_creation_effect);

  if (version >= 2) {
    int16_t raw = 0;
    ifile >> raw;
    m_creation_texture = (raw >= 0 && raw < MAX_TEXTURES) ? static_cast<int16_t>(texture_xlate[raw]) : 0;
  }

  ifile >> m_num_spawn_pnts;

  // The engine stores the counts in narrow types; clamp them so a corrupt
  // file cannot overrun the fixed size tables.
  if (m_num_spawn_pnts < 0) m_num_spawn_pnts = 0;
  if (m_num_spawn_pnts > MAX_SPAWN_PNTS) m_num_spawn_pnts = MAX_SPAWN_PNTS;

  m_roomnum = 0;
  ifile >> m_roomnum;

  ifile >> m_create_pnt.x() >> m_create_pnt.y() >> m_create_pnt.z();

  int32_t create_room = MATCEN_ERROR;
  ifile >> create_room;
  m_create_room = to_roomnum(create_room);

  m_spawn_pnt.clear();
  m_spawn_vec.clear();
  m_spawn_normal.clear();
  for (int i = 0; i < max_spawn_pnts; i++) {
    int32_t roomnum = MATCEN_ERROR;
    ifile >> roomnum;
    m_spawn_pnt.push_back(to_roomnum(roomnum));

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
  if (m_max_alive_children < -1) m_max_alive_children = -1;
  if (m_max_alive_children > MAX_MATCEN_ALIVE_CHILDREN) m_max_alive_children = MAX_MATCEN_ALIVE_CHILDREN;
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

  // The engine always writes the fixed maximum counts; pad the tables back
  // out to those sizes so a re-save reproduces the engine's chunk layout.
  while (m_spawn_pnt.size() < MAX_SPAWN_PNTS) {
    m_spawn_pnt.push_back(index_t{});
    m_spawn_vec.push_back(vector3{});
    m_spawn_normal.push_back(vector3{});
  }

  while (m_prod_type.size() < MAX_PROD_TYPES) {
    m_prod_type.push_back(index_t{});
    m_prod_time.push_back(1.0f);
    m_prod_priority.push_back(100);
    m_max_prod_type.push_back(3);
  }

  while (m_num_prod_type.size() < MAX_PROD_TYPES) {
    m_num_prod_type.push_back(0);
  }

  while (m_sounds.size() < static_cast<size_t>(matcen_sound::count)) {
    m_sounds.push_back(index_t{});
  }
}

// ---------------------------------------------------------------------------
// Runtime helpers
// ---------------------------------------------------------------------------

index_t matcen::ResolveCreateRoom() {
  if (!roomnum_outside(m_create_room) && *m_create_room < Rooms.size() && Rooms[*m_create_room].flags.external) {
    return GetTerrainRoomFromPos(m_create_pnt);
  }

  return m_create_room;
}

index_t matcen::GetSoundHandle(matcen_sound sound) const {
  const size_t slot = static_cast<size_t>(sound);
  if (slot >= m_sounds.size())
    return index_t{};

  return m_sounds[slot];
}

void matcen::UpdateActiveSound() {
  const bool cur_active = m_status.active;

  if (cur_active && !m_last_active_check_result) {
    const index_t sound = GetSoundHandle(matcen_sound::active);
    if (!sound)
      return;

    if (m_type == matcen_type::object) {
      object *obj = ObjGet(m_objref);

      if (obj)
        m_sound_active_handle = Sound_system.Play3dSound(index_to_int(sound), sound_priority::normal, obj);
    } else {
      pos_state pos;
      matrix idmat = Identity_matrix;

      pos.position = &m_create_pnt;
      pos.orient = &idmat;
      pos.roomnum = from_roomnum(ResolveCreateRoom());

      m_sound_active_handle = Sound_system.Play3dSound(index_to_int(sound), sound_priority::normal, &pos);
    }
  } else if (!cur_active && m_last_active_check_result) {
    Sound_system.StopSoundLooping(m_sound_active_handle);
  }
}

bool matcen::ComputeNextProdInfo() {
  if (m_status.disabled)
    return false;

  m_status.done_prod = false;

  if (m_max_prod <= m_num_prod && m_max_prod != -1) {
    m_status.done_prod = true;
    return false;
  }

  Q_ASSERT(m_prod_mode == matcen_prod_mode::notprod);

  int32_t cur_index = -1;
  bool f_do = true;

  if (m_status.random_prod_order) {
    int32_t total_priorities = 0;

    for (int32_t i = 0; i < m_num_prod_types; i++) {
      if (m_max_prod_type[i] == -1 || m_max_prod_type[i] > m_num_prod_type[i]) {
        total_priorities += m_prod_priority[i];
      }
    }

    if (total_priorities <= 0) {
      f_do = false;
    } else {
      int32_t r_val = d3::rand() % total_priorities;
      bool f_looking = true;

      int32_t i = 0;

      do {
        Q_ASSERT(i < m_num_prod_types);

        if (m_max_prod_type[i] == -1 || m_max_prod_type[i] > m_num_prod_type[i]) {
          r_val -= m_prod_priority[i];

          if (r_val <= 0) {
            cur_index = i;
            f_looking = false;
          }
        }

        i++;
      } while (f_looking);
    }
  } else {
    int32_t best_priority = -1;

    for (int32_t i = 0; i < m_num_prod_types; i++) {
      if ((m_max_prod_type[i] == -1 || m_max_prod_type[i] > m_num_prod_type[i]) &&
          m_prod_priority[i] > best_priority) {
        cur_index = i;
        best_priority = m_prod_priority[i];
      }
    }

    if (cur_index == -1) {
      f_do = false;
    }
  }

  if (f_do) {
    m_cached_prod_index = cur_index;
    m_cached_prod_time = d3::chrono::last_update() + m_prod_time[cur_index];

    return true;
  }

  m_cached_prod_index = -1;
  m_status.done_prod = true;
  return false;
}

bool matcen::ComputeCreatePnt() {
  if (m_status.manual_update_create_pnt) {
    return true;
  }

  if (m_type == matcen_type::unassigned)
    return false;

  float spawn_dist = 8.0f;

  if (m_cached_prod_index >= 0 && m_cached_prod_index < m_num_prod_types &&
      m_cached_prod_index < static_cast<int32_t>(m_prod_type.size()) && m_prod_type[m_cached_prod_index] &&
      *m_prod_type[m_cached_prod_index] < Object_info.size() &&
      Object_info[*m_prod_type[m_cached_prod_index]].type != object_type_e::none) {
    spawn_dist = Object_info[*m_prod_type[m_cached_prod_index]].size + 1.0f;
  }

  if (m_type == matcen_type::room) {
    bool f_computed = false;

    if (m_roomnum < 0 || static_cast<size_t>(m_roomnum) >= Rooms.size() || !Rooms[m_roomnum].used)
      return false;

    if (m_num_spawn_pnts) {
      int32_t num_valid_faces = 0;
      room_t &rp = Rooms[m_roomnum];

      m_create_pnt = vector3{};

      for (int32_t i = 0; i < m_num_spawn_pnts; i++) {
        if (i >= static_cast<int32_t>(m_spawn_pnt.size()) ||
            num_valid_faces >= static_cast<int32_t>(m_spawn_vec.size()))
          break;

        const int32_t spawn_pnt = index_to_int(m_spawn_pnt[i]);
        if (spawn_pnt >= 0 && spawn_pnt < static_cast<int32_t>(rp.faces.size())) {
          face &fp = rp.faces[spawn_pnt];

          m_spawn_vec[num_valid_faces] = vector3{};

          for (uint8_t y = 0; y < fp.num_verts && y < fp.face_verts.size(); y++) {
            const int32_t vert = fp.face_verts[y];
            if (vert < 0 || static_cast<size_t>(vert) >= rp.verts.size())
              continue;

            m_spawn_vec[num_valid_faces] += rp.verts[vert];
          }

          m_spawn_vec[num_valid_faces] /= static_cast<float>(fp.num_verts);
          m_create_pnt += m_spawn_vec[num_valid_faces];

          m_spawn_normal[num_valid_faces] = fp.normal;

          num_valid_faces++;
        }
      }

      if (num_valid_faces) {
        m_create_pnt /= static_cast<float>(num_valid_faces);

        for (int32_t i = 0; i < num_valid_faces; i++) {
          const vector3 to_dist = m_create_pnt - m_spawn_vec[i];
          const scalar dot = vm_Dot3Product(m_spawn_normal[i], to_dist);

          if (dot < spawn_dist) {
            const scalar add_dist = spawn_dist - dot;
            m_create_pnt += (add_dist * m_spawn_normal[i]);
          }
        }

        f_computed = true;
      }
    }

    if (!f_computed) {
      m_create_pnt = Rooms[m_roomnum].path_pnt;
    }

    m_create_room = to_roomnum(m_roomnum);
  } else {
    object *master_obj = ObjGet(m_objref);
    if (!master_obj)
      return false;

    bool f_computed = false;

    if (m_num_spawn_pnts) {
      int32_t num_valid_gps = 0;

      m_create_pnt = vector3{};

      for (int32_t i = 0; i < m_num_spawn_pnts; i++) {
        if (i >= static_cast<int32_t>(m_spawn_pnt.size()) ||
            num_valid_gps >= static_cast<int32_t>(m_spawn_vec.size()))
          break;

        if (WeaponCalcGun(&m_spawn_vec[num_valid_gps], &m_spawn_normal[num_valid_gps], master_obj,
                          index_to_int(m_spawn_pnt[i]))) {
          m_create_pnt += m_spawn_vec[num_valid_gps];
          num_valid_gps++;
        }
      }

      if (num_valid_gps) {
        m_create_pnt /= static_cast<float>(num_valid_gps);

        for (int32_t i = 0; i < num_valid_gps; i++) {
          const vector3 to_dist = m_create_pnt - m_spawn_vec[i];
          const scalar dot = vm_Dot3Product(m_spawn_normal[i], to_dist);

          if (dot < spawn_dist) {
            const scalar add_dist = spawn_dist - dot;
            m_create_pnt += (add_dist * m_spawn_normal[i]);
          }
        }

        f_computed = true;
      }
    }

    if (!f_computed) {
      m_create_pnt = master_obj->pos;
    }

    fvi_query fq{};
    fvi_info hit_info{};

    fq.p0 = &master_obj->pos;
    fq.startroom = from_roomnum(master_obj->roomnum);
    fq.p1 = &m_create_pnt;
    fq.rad = 0.0f;
    fq.thisobjnum = -1;
    fq.ignore_obj_list = nullptr;
    fq.flags.ignore_walls = true;

    fvi_FindIntersection(fq, hit_info);
    m_create_room = to_roomnum(hit_info.hit_room);
  }

  return false;
}

void matcen::CheckActivateStatus() {
  m_next_active_check_time = d3::chrono::last_update() + MATCEN_ACTIVE_CHECK_RATE +
                             (MATCEN_ACTIVE_CHECK_VARIENCE *
                              ((static_cast<float>(d3::rand()) / static_cast<float>(d3::rand_max)) - 0.5f));
  m_last_active_check_result = m_status.active;

  if (m_status.prod_till_done) {
    m_status.active = true;
  } else {
    switch (m_control_type) {
    case matcen_control_type::script:
      break;

    case matcen_control_type::while_player_visible:
    case matcen_control_type::after_player_visible: {
      m_status.active = false;

      fvi_query fq{};
      fvi_info hit_info{};

      fq.p0 = &m_create_pnt;
      fq.startroom = from_roomnum(ResolveCreateRoom());
      fq.p1 = &Player_object->pos;
      fq.rad = 0.0f;
      fq.thisobjnum = -1;
      fq.ignore_obj_list = nullptr;
      fq.flags.no_relink = true;
      fq.flags.check_objs = true;
      fq.flags.only_door_obj = true;

      const fvi_hit_type fate = fvi_FindIntersection(fq, hit_info);

      if (fate == fvi_hit_type::none)
        m_status.active = true;
    } break;

    case matcen_control_type::while_player_near:
    case matcen_control_type::after_player_near: {
      m_status.active = false;

      if (!m_create_room) {
      } else if (roomnum_outside(m_create_room) || (*m_create_room < Rooms.size() && Rooms[*m_create_room].used &&
                                                   Rooms[*m_create_room].flags.external)) {
        if (vm_VectorDistance(Player_object->pos, m_create_pnt) <= MATCEN_OUTSIDE_NEAR_DIST) {
          m_status.active = true;
        }
      } else if (*m_create_room < Rooms.size() && Rooms[*m_create_room].used) {
        const room_t &rp = Rooms[*m_create_room];
        const int32_t player_room = from_roomnum(Player_object->roomnum);

        if (static_cast<int32_t>(*m_create_room) == player_room) {
          m_status.active = true;
        } else {
          for (size_t x = 0; x < rp.portals.size(); x++) {
            const int32_t c_room = index_to_int(rp.portals[x].connected_room);

            if (c_room < 0 && (vm_VectorDistance(Player_object->pos, m_create_pnt) <= MATCEN_OUTSIDE_NEAR_DIST)) {
              m_status.active = true;
            } else {
              if (c_room == player_room) {
                m_status.active = true;
              }
            }

            if (m_status.active) {
              break;
            }
          }
        }
      }
    } break;

    default:
      Q_ASSERT(false);
      break;
    }
  }

  UpdateActiveSound();

  if (m_status.active && (m_control_type == matcen_control_type::after_player_near ||
                          m_control_type == matcen_control_type::after_player_visible)) {
    m_status.prod_till_done = true;
  }
}

bool matcen::StartObjProd() {
  if (m_cached_prod_index < 0 || m_cached_prod_index >= static_cast<int32_t>(m_max_prod_type.size()) ||
      m_cached_prod_index >= static_cast<int32_t>(m_num_prod_type.size()) ||
      m_max_prod_type[m_cached_prod_index] >= m_num_prod_type[m_cached_prod_index])
    ComputeNextProdInfo();

  if (m_status.done_prod || m_status.active_pause || m_status.disabled)
    return false;

  if (m_prod_mode != matcen_prod_mode::notprod)
    return false;

  if (!m_status.active)
    return false;

  if (m_max_prod <= m_num_prod && m_max_prod != -1) {
    m_status.done_prod = true;
    return false;
  }

  if (m_cached_prod_index < 0)
    return false;

  pos_state pos;
  matrix idmat = Identity_matrix;

  pos.position = &m_create_pnt;
  pos.orient = &idmat;
  pos.roomnum = from_roomnum(ResolveCreateRoom());

  const index_t prod_sound = GetSoundHandle(matcen_sound::prod);
  if (prod_sound) {
    Sound_system.StopSoundLooping(Sound_system.Play3dSound(index_to_int(prod_sound), sound_priority::high, &pos));
  }

  m_status.never_prod = false;

  m_prod_mode_time = 0.0f;
  m_prod_mode = matcen_prod_mode::preprod;
  m_cur_saturation_count = 0;

  return true;
}

bool matcen::DoObjProd() {
  int32_t p_handle = OBJECT_HANDLE_NONE;
  bool script_initted = false;

  Q_ASSERT(m_prod_mode == matcen_prod_mode::preprod);
  Q_ASSERT(m_type == matcen_type::room || m_type == matcen_type::object);

  if (m_type == matcen_type::object) {
    object *parent = ObjGet(m_objref);
    Q_ASSERT(parent);

    if (parent->control_type == control_type_e::ai || parent->type == object_type_e::player) {
      p_handle = parent->handle;
    }
  }

  ComputeCreatePnt();

  matrix orient;
  vector3 fvec = Player_object->pos - m_create_pnt;
  vm_NormalizeVector(fvec);

  vm_VectorToMatrix(orient, fvec);

  index_t objnum = index_t{};

  if (m_cached_prod_index >= 0 && m_cached_prod_index < static_cast<int32_t>(m_prod_type.size()) &&
      m_prod_type[m_cached_prod_index]) {
    const uint16_t type = static_cast<uint16_t>(*m_prod_type[m_cached_prod_index]);

    Matcen_created = true;

    objnum = ObjCreate(Object_info[type].type, type, ResolveCreateRoom(), m_create_pnt, orient, p_handle);

    Matcen_created = false;

    if (objnum && Object_info[type].flags.control_ai && !Object_info[type].ai_info.empty() &&
        Object_info[type].ai_info[0].movement_type != static_cast<uint8_t>(movement_class::flying)) {
      vector3 gp;
      vector3 ground_normal;
      object &obj = Objects[*objnum];

      if (PhysCalcGround(gp, ground_normal, obj, 0)) {
        fvi_info hit_info{};
        fvi_query fq{};

        vector3 start = obj.pos;
        vector3 end = obj.pos - 2000.0f * obj.orient.uvec;

        fq.p0 = &start;
        fq.p1 = &end;
        fq.startroom = from_roomnum(obj.roomnum);

        fq.rad = 0.0f;
        fq.flags.check_objs = true;
        fq.flags.ignore_powerups = true;
        fq.flags.ignore_weapons = true;
        fq.flags.ignore_moving_objects = true;
        fq.thisobjnum = static_cast<int16_t>(obj.index());
        fq.ignore_obj_list = nullptr;

        const fvi_hit_type fate = fvi_FindIntersection(fq, hit_info);

        if (fate != fvi_hit_type::none) {
          const scalar ps = vm_Dot3Product((gp - obj.pos), obj.orient.uvec);
          const scalar pr = vm_Dot3Product((hit_info.hit_pnt - obj.pos), obj.orient.uvec);

          if (ps != pr) {
            const scalar diff = ps - pr;
            obj.pos -= diff * obj.orient.uvec;
            ObjSetPos(obj, obj.pos, obj.roomnum, std::nullopt, false);
            obj.flags.moved_this_frame = true;

            m_create_pnt = obj.pos;
          }
        }
      }
    }

    if (objnum) {
      object &obj = Objects[*objnum];

      if (obj.effect_info) {
        obj.effect_info->type_flags.fading_in = true;
        obj.effect_info->fade_time = 1.0;
        obj.effect_info->fade_max_time = 1.0;
      }

      InitObjectScripts(&obj);
      script_initted = true;

      const bool f_generic = (obj.type == object_type_e::clutter || obj.type == object_type_e::building ||
                              obj.type == object_type_e::robot || obj.type == object_type_e::powerup);

      if (f_generic) {
        const index_t ambient_sound = Object_info[obj.id].sounds[generic_sound::ambient];
        if (ambient_sound) {
          Sound_system.Play3dSound(index_to_int(ambient_sound), sound_priority::lowest, &obj);
        }
      }
    }
  }

  if (objnum) {
    if (!script_initted)
      InitObjectScripts(&Objects[*objnum]);

    AddToAliveList(Objects[*objnum].handle);

    m_last_prod_objref = Objects[*objnum].handle;
  } else {
    m_last_prod_objref = OBJECT_HANDLE_NONE;
  }

  m_status.create_obj_frame = true;

  m_last_prod_type_index = m_cached_prod_index;
  if (m_cached_prod_index >= 0 && m_cached_prod_index < static_cast<int32_t>(m_num_prod_type.size()))
    m_num_prod_type[m_cached_prod_index]++;
  m_num_prod++;

  m_prod_mode_time = 0.0f;
  m_prod_mode = matcen_prod_mode::postprod;
  m_cur_saturation_count = 0;

  return true;
}

bool matcen::DoAliveListFrame() {
  for (int32_t i = 0; i < m_num_alive; i++) {
    if (i >= static_cast<int32_t>(m_alive_list.size()))
      break;

    if (!ObjGet(m_alive_list[i])) {
      for (int32_t j = i; j < m_num_alive - 1; j++) {
        m_alive_list[j] = m_alive_list[j + 1];
      }

      m_num_alive--;
      i--;
    }
  }

  return true;
}

bool matcen::AddToAliveList(int32_t objref) {
  if (m_max_alive_children > 0 && m_max_alive_children > m_num_alive &&
      m_num_alive < static_cast<int32_t>(m_alive_list.size())) {
    m_alive_list[m_num_alive++] = objref;
  }

  return false;
}

bool matcen::FinishObjProd() {
  Q_ASSERT(m_prod_mode == matcen_prod_mode::postprod);

  m_prod_mode_time = 0.0f;
  m_prod_mode = matcen_prod_mode::notprod;
  m_cur_saturation_count = 0;

  ComputeNextProdInfo();

  return false;
}

void matcen::DoThinkFrame() {
  if (m_type == matcen_type::unassigned)
    return;

  if ((m_prod_mode == matcen_prod_mode::notprod) && m_status.disabled)
    return;

  if (m_type == matcen_type::object) {
    object *parent = ObjGet(m_objref);

    if (!parent) {
      m_prod_mode_time = 0.0f;
      m_prod_mode = matcen_prod_mode::notprod;
      m_cur_saturation_count = 0;

      m_type = matcen_type::unassigned;
      return;
    }

    if ((m_prod_mode != matcen_prod_mode::notprod &&
         (parent->movement_type == movement_type_e::physics || parent->movement_type == movement_type_e::walking)) ||
        m_status.compute_create_pnt_every_frame) {
      ComputeCreatePnt();
    }
  }

  m_prod_mode_time += d3::chrono::last_render();

  if (m_max_alive_children > 0) {
    DoAliveListFrame();
  }

  switch (m_prod_mode) {
  case matcen_prod_mode::notprod: {
    if (!(m_status.done_prod || m_status.active_pause) && (m_num_alive != m_max_alive_children)) {
      if (d3::chrono::last_update() >= m_next_active_check_time) {
        CheckActivateStatus();
      }

      if (m_status.active && (d3::chrono::last_update() >= m_cached_prod_time) && (m_cached_prod_index >= 0)) {
        StartObjProd();
      }
    } else if (m_status.done_prod && m_status.active) {
      Sound_system.StopSoundLooping(m_sound_active_handle);
      m_status.active = false;
    }
  } break;

  case matcen_prod_mode::preprod: {
    if (m_prod_mode_time >= m_preprod_time)
      DoObjProd();
  } break;

  case matcen_prod_mode::postprod: {
    m_status.create_obj_frame = false;

    if (m_prod_mode_time >= m_postprod_time)
      FinishObjProd();
  } break;

  default:
    Q_ASSERT(false);
    break;
  }

  if (!m_status.not_hurt_player &&
      (m_prod_mode == matcen_prod_mode::preprod || m_prod_mode == matcen_prod_mode::postprod)) {
    const float damage = MATCEN_DAMAGE_PER_SECOND * d3::chrono::last_render();
    object *p = Player_object;

    if (p->type != object_type_e::ghost) {
      const int32_t croom = from_roomnum(ResolveCreateRoom());

      if (croom == from_roomnum(p->roomnum)) {
        vector3 dir = p->pos - m_create_pnt;

        const scalar dist = vm_NormalizeVector(dir) - p->size;
        if (dist < MATCEN_DAMAGE_DIST) {
          ApplyDamageToPlayer(p, p, player_damage_type::energy_weapon, damage);

          dir *= MATCEN_FORCE;

          vector3 movement_pos, movement_vec, pos;
          PhysicsDoSimLinear(*p, pos, dir, p->mtype.phys_info.velocity, movement_vec, movement_pos,
                             d3::chrono::last_render(), 1);
        }
      }
    }
  }
}

void matcen::DoRenderFrame() {
  if (!(m_cached_prod_index >= 0 && m_cached_prod_index < m_num_prod_types &&
        m_cached_prod_index < static_cast<int32_t>(m_prod_type.size()) && m_prod_type[m_cached_prod_index] &&
        *m_prod_type[m_cached_prod_index] < Object_info.size() &&
        Object_info[*m_prod_type[m_cached_prod_index]].type != object_type_e::none))
    return;

  switch (m_prod_mode) {
  case matcen_prod_mode::notprod: {
    switch (m_creation_effect) {
    case matcen_effect::line_lightning:
    default:
      return;
    }
  } break;

  case matcen_prod_mode::preprod: {
    switch (m_creation_effect) {
    case matcen_effect::line_lightning: {
      const uint32_t oi_index = *m_prod_type[m_cached_prod_index];

      if (oi_index >= MAX_POLY_MODELS)
        break;

      PageInPolymodel(Object_info[oi_index].render_handle, Object_info[oi_index].type, Object_info[oi_index].size);
      float size = Object_info[oi_index].size * 1.5f;

      if (m_cur_saturation_count < 1) {
        for (int32_t i = 0; i < m_num_spawn_pnts; i++) {
          const int32_t croom = from_roomnum(ResolveCreateRoom());

          m_spawn_vis_effects[m_cur_saturation_count][i] = -1;

          const int visnum =
              VisEffectCreate(VIS_FIREBALL, static_cast<uint8_t>(fireball_texture::lightning_bolt_index), croom,
                              &m_create_pnt);
          if (visnum >= 0) {
            vis_effect &vis = VisEffects[visnum];
            vis.lifeleft = 0.8f;
            vis.lifetime = 0.8f;
            vis.end_pos = m_spawn_vec[i];
            vis.flags.uses_lifeleft = true;
            vis.flags.expand = true;
            vis.velocity.x() = 0.8f;
            vis.velocity.y() = 1.0f;
            m_spawn_vis_effects[m_cur_saturation_count][i] = static_cast<int16_t>(visnum);
          }
        }
        m_cur_saturation_count++;
      }

      for (uint8_t i = 0; i < m_cur_saturation_count; i++) {
        for (int32_t t = 0; t < m_num_spawn_pnts; t++) {
          const int16_t visnum = m_spawn_vis_effects[i][t];
          if (visnum == -1)
            continue;

          Q_ASSERT(visnum >= 0 && visnum < MAX_VIS_EFFECTS);

          vis_effect &vis = VisEffects[visnum];

          vis.pos = m_create_pnt;
          vis.end_pos = m_spawn_vec[t];
        }
      }

      vector3 center = m_create_pnt;
      size = (m_prod_mode_time / m_preprod_time) * size;

      if (size > 0.0f)
        DrawColoredDisk(&center, 0.3f, 0.5f, 1.0f, 0.7f, 0.0f, size, 1);
    } break;

    case matcen_effect::line_sine_wave: {
      const uint32_t oi_index = *m_prod_type[m_cached_prod_index];

      if (oi_index >= MAX_POLY_MODELS)
        break;

      PageInPolymodel(Object_info[oi_index].render_handle, Object_info[oi_index].type, Object_info[oi_index].size);
      float size = Object_info[oi_index].size * 1.5f;

      if (m_cur_saturation_count < 1) {
        for (int32_t i = 0; i < m_num_spawn_pnts; i++) {
          const int32_t croom = from_roomnum(ResolveCreateRoom());

          m_spawn_vis_effects[m_cur_saturation_count][i] = -1;

          const int visnum =
              VisEffectCreate(VIS_FIREBALL, static_cast<uint8_t>(fireball_texture::sine_wave_index), croom,
                              &m_create_pnt);
          if (visnum >= 0) {
            vis_effect &vis = VisEffects[visnum];
            vis.lifeleft = 1.0f;
            vis.lifetime = 1.0f;
            vis.end_pos = m_spawn_vec[i];
            vis.flags.uses_lifeleft = true;
            vis.flags.expand = true;
            vis.velocity.x() = 1.0f;
            vis.velocity.y() = 1.0f;
            m_spawn_vis_effects[m_cur_saturation_count][i] = static_cast<int16_t>(visnum);
          }
        }

        m_cur_saturation_count++;
      }

      for (uint8_t i = 0; i < m_cur_saturation_count; i++) {
        for (int32_t t = 0; t < m_num_spawn_pnts; t++) {
          const int16_t visnum = m_spawn_vis_effects[i][t];

          if (visnum == -1)
            continue;

          Q_ASSERT(visnum >= 0 && visnum < MAX_VIS_EFFECTS);

          vis_effect &vis = VisEffects[visnum];

          vis.pos = m_create_pnt;
          vis.end_pos = m_spawn_vec[t];
        }
      }

      vector3 center = m_create_pnt;
      size = (m_prod_mode_time / m_preprod_time) * size;

      if (size > 0.0f)
        DrawColoredDisk(&center, 0.6f, 0.4f, 1.0f, 0.7f, 0.0f, size, 1);
    } break;

    case matcen_effect::procedural_lightning: {
      const uint32_t oi_index = *m_prod_type[m_cached_prod_index];

      PageInPolymodel(Object_info[oi_index].render_handle, Object_info[oi_index].type, Object_info[oi_index].size);
      float size = Object_info[oi_index].size * 1.5f;

      if (m_cur_saturation_count < MAX_MATCEN_EFFECT_SATURATION) {
        for (int32_t i = 0; i < m_num_spawn_pnts; i++) {
          const int32_t croom = from_roomnum(ResolveCreateRoom());

          m_spawn_vis_effects[m_cur_saturation_count][i] = -1;

          const int visnum =
              VisEffectCreate(VIS_FIREBALL, static_cast<uint8_t>(fireball_texture::thick_lightning_index), croom,
                              &m_create_pnt);
          if (visnum >= 0) {
            vis_effect &vis = VisEffects[visnum];

            vis.end_pos = m_spawn_vec[i];
            vis.lifeleft = 1.0f;
            vis.lifetime = 1.0f;
            vis.flags.uses_lifeleft = true;
            vis.flags.expand = true;
            vis.flags.link_to_viewer = true;
            vis.billboard_info.width = static_cast<uint8_t>(std::max<scalar>(1.0f, size / 2.0f));
            vis.billboard_info.texture = 1;
            vis.velocity.x() = 1.0f;
            vis.velocity.y() = 1.0f;
            vis.velocity.z() = 0.5f;
            vis.lighting_color = GR_RGB16(255, 255, 255);

            vis.custom_handle = m_creation_texture;
            vis.size = vm_VectorDistanceQuick(vis.pos, vis.end_pos);
            m_spawn_vis_effects[m_cur_saturation_count][i] = static_cast<int16_t>(visnum);
          }
        }
        m_cur_saturation_count++;
      }

      for (uint8_t i = 0; i < m_cur_saturation_count; i++) {
        for (int32_t t = 0; t < m_num_spawn_pnts; t++) {
          const int16_t visnum = m_spawn_vis_effects[i][t];

          if (visnum == -1)
            continue;

          Q_ASSERT(visnum >= 0 && visnum < MAX_VIS_EFFECTS);

          vis_effect &vis = VisEffects[visnum];
          vis.pos = m_create_pnt;
          vis.end_pos = m_spawn_vec[t];
        }
      }

      vector3 center = m_create_pnt;
      size = (m_prod_mode_time / m_preprod_time) * size;

      if (size > 0.0f)
        DrawColoredDisk(&center, 0.3f, 0.5f, 1.0f, 0.7f, 0.0f, size, 1);
    } break;

    default:
      return;
    }
  } break;

  case matcen_prod_mode::postprod: {
    return;
  } break;

  default:
    Q_ASSERT(false);
  }
}
