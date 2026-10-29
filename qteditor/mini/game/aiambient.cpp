/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <cstdint>
#include "aiambient.h"
#include "objinfo.h"
#include "chrono_timer.h"
#include "rand.h"
#include <posix_stream.h>
#include "string_helpers.h"

namespace {
// On-disk version of the LIFE chunk (engine aiambient.cpp:29).
constexpr int32_t AL_VERSION = 1;
} // namespace

void ambient_life::setting_t::reset(void)
{
  type.reset();
  name.clear();
  total = 0;
  min = 0;
  max = 0;
  flags = {};
  next_size = 0;
  next_do_time = 0.0f;
}

// Picks a random resident count in [m_min, m_max] for slot i.
void ambient_life::setting_t::compute_next_size(void)
{
  if (max > min)
    next_size = min + static_cast<uint8_t>(d3::rand() % (max - min));
  else
    next_size = max;
}

// Resets every ambient-life slot to a clean state (engine aiambient.cpp:115).
void ambient_life::reset()
{
  for (size_t i = 0; i < m_settings.size(); i++)
  {
    m_settings[i].reset();
    m_cur_num[i] = 0;

    for (size_t j = 0; j < m_handle[i].size(); j++)
      m_handle[i][j] = 0;
  }
}

void ambient_life::DoFrame(void) {}



// Called at level start.  Nowhere near gameplay (the mini is an editor), but
// ported for completeness: the engine initializes the per-type next-values.
void ambient_life::InitForLevel()
{
  for (size_t i = 0; i < m_settings.size(); i++)
  {
    m_settings[i].compute_next_size();
    m_settings[i].next_do_time = d3::chrono::last_update();
    m_cur_num[i] = 0;
  }

  DoFrame();
}

// Reads one editor-settable field of slot `index` into `*ptr` (engine
// aiambient.cpp:45).  type is an int (legacy object id, -1 for none); every
// other field is a single byte.
void ambient_life::GetALValue(uint8_t index, ambient_field field, void *ptr) const
{
  const setting_t& s = m_settings[index];
  switch (field)
  {
  case ambient_field::type:
    *static_cast<int *>(ptr) = index_to_int(s.type);
    break;
  case ambient_field::total:
    *static_cast<uint8_t *>(ptr) = s.total;
    break;
  case ambient_field::max:
    *static_cast<uint8_t *>(ptr) = s.max;
    break;
  case ambient_field::min:
    *static_cast<uint8_t *>(ptr) = s.min;
    break;
  case ambient_field::flags:
    *static_cast<uint8_t *>(ptr) = reinterpret_cast<const uint8_t &>(s.flags);
    break;
  }
}

// Writes one editor-settable field of slot `index` from `*ptr` (engine
// aiambient.cpp:75), then clamps the group counts and rerolls the next size.
void ambient_life::SetALValue(uint8_t index, ambient_field field, void *ptr)
{
  setting_t& s = m_settings[index];
  switch (field)
  {
  case ambient_field::type: {
    const int value = *static_cast<int *>(ptr);
    s.type = value >= 0 ? index_t(static_cast<uint32_t>(value)) : index_t(std::nullopt);
    break;
  }
  case ambient_field::total:
    s.total = *static_cast<uint8_t *>(ptr);
    break;
  case ambient_field::max:
    s.max = *static_cast<uint8_t *>(ptr);
    break;
  case ambient_field::min:
    s.min = *static_cast<uint8_t *>(ptr);
    break;
  case ambient_field::flags:
    reinterpret_cast<uint8_t &>(s.flags) = *static_cast<uint8_t *>(ptr);
    break;
  }

  // total/max/min are unsigned, so only the upper bounds can trip.
  if (s.total > MAX_ALS_PER_TYPE)
    s.total = MAX_ALS_PER_TYPE;

  if (s.max > s.total)
    s.max = s.total;

  if (s.min > s.max)
    s.min = s.max;

  s.compute_next_size();
}

byte_ostream& operator<<(byte_ostream& output, const ambient_life::setting_t& s)
{
  return output
         << pascal_string(s.type ? optref<std::string>(Object_info[*s.type].name) : std::nullopt)
         << s.total
         << reinterpret_cast<const uint8_t&>(s.flags)
         << s.min
         << s.max
         << s.next_size
         << s.next_do_time;
}

byte_istream& operator>>(byte_istream& input, ambient_life::setting_t& s)
{
  input >> pascal_string(s.name)
        >> s.total
        >> reinterpret_cast<uint8_t&>(s.flags)
        >> s.min
        >> s.max
        >> s.next_size
        >> s.next_do_time;
  s.type = FindObjectIDName(s.name);
  return input;
}

// LIFE chunk writer (engine aiambient.cpp:138).  Per type: the object name
// (short length + name bytes including the NUL), then five byte fields and a
// float; afterwards the per-type live-resident counts and their object handles.
byte_ostream& operator<<(byte_ostream& os, const ambient_life& al) {
  os << AL_VERSION;
  os << al.m_settings;

  for (size_t i = 0; i < al.m_settings.size(); i++) {
    os << al.m_cur_num[i];
    for (int j = 0; j < al.m_cur_num[i]; j++)
      os << al.m_handle[i][j];
  }
  return os;
}

// LIFE chunk reader (engine aiambient.cpp:175).  The table is rebuilt fresh
// (like the other chunk readers) so a re-saved level keeps only the entries
// the file declared; the object name is resolved through the loaded objinfo
// tables to a global object id.
byte_istream& operator>>(byte_istream& is, ambient_life& al) {
  al.reset();

  int32_t version = 0;
  is >> version;
  if (version < 1)
    return is;

  is >> al.m_settings;

  for (size_t i = 0; i < al.m_settings.size(); i++)
  {
    is >> al.m_cur_num[i];

    for (int j = 0; j < al.m_cur_num[i]; j++)
      is >> al.m_handle[i][j];
  }
  return is;
}
