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

namespace {
// On-disk version of the LIFE chunk (engine aiambient.cpp:29).
constexpr int32_t AL_VERSION = 1;
} // namespace

// Resets every ambient-life slot to a clean state (engine aiambient.cpp:115).
void ambient_life::ALReset() {
  for (int i = 0; i < (int)m_type.size(); i++) {
    m_type[i] = -1;

    for (int j = 0; j < (int)m_handle[i].size(); j++)
      m_handle[i][j] = 0;

    m_total[i] = 0;
    m_cur_num[i] = 0;
    m_flags[i] = 0;

    m_min[i] = 0;
    m_max[i] = 0;

    m_next_do_time[i] = 0.0f;
    m_next_size[i] = 0;
  }
}

void ambient_life::DoFrame(void) {}

// Picks a random resident count in [m_min, m_max] for slot i.
void ambient_life::ComputeNextSize(int8_t i) {
  const int diff = static_cast<int>(m_max[i]) - static_cast<int>(m_min[i]);
  if (diff > 0) {
    const int offset = d3::rand() % diff;
    m_next_size[i] = static_cast<uint8_t>(static_cast<int>(m_min[i]) + offset);
  } else {
    m_next_size[i] = m_max[i];
  }
}

// Called at level start.  Nowhere near gameplay (the mini is an editor), but
// ported for completeness: the engine initializes the per-type next-values.
void ambient_life::InitForLevel() {
  for (int i = 0; i < (int)m_type.size(); i++) {
    ComputeNextSize(static_cast<int8_t>(i));
    m_cur_num[i] = 0;
    m_next_do_time[i] = d3::chrono::last_update();
  }

  DoFrame();
}

// LIFE chunk writer (engine aiambient.cpp:138).  Per type: the object name
// (short length + name bytes including the NUL), then five byte fields and a
// float; afterwards the per-type live-resident counts and their object handles.
void ambient_life::SaveData(posix_ostream &ofile) const {
  ofile << AL_VERSION;

  for (int i = 0; i < (int)m_type.size(); i++) {
    const int type = m_type[i];

    if (type >= 0) {
      const std::string &name = Object_info[type].name;
      ofile << static_cast<int16_t>(static_cast<int>(name.size()) + 1); // + NUL
      if (!name.empty())
        ofile.write(name.data(), name.size());
      ofile.put('\0');
    } else {
      ofile << static_cast<int16_t>(1);
      ofile.put('\0');
    }

    ofile << m_total[i];
    ofile << m_flags[i];
    ofile << m_min[i];
    ofile << m_max[i];
    ofile << m_next_size[i];
    ofile << m_next_do_time[i];
  }

  for (int i = 0; i < (int)m_type.size(); i++) {
    ofile << m_cur_num[i];

    for (int j = 0; j < m_cur_num[i]; j++)
      ofile << m_handle[i][j];
  }
}

// LIFE chunk reader (engine aiambient.cpp:175).  The table is rebuilt fresh
// (like the other chunk readers) so a re-saved level keeps only the entries
// the file declared; the object name is resolved through the loaded objinfo
// tables to a global object id.
void ambient_life::LoadData(posix_istream &ifile) {
  ALReset();

  int32_t version = 0;
  ifile >> version;
  if (version < 1)
    return;

  for (int i = 0; i < (int)m_type.size(); i++) {
    int16_t len = 0;
    ifile >> len;
    if (len < 0)
      len = 0;
    if (len > 1024) // the engine reads into a 256-byte temp buffer
      len = 1024;

    std::string raw(static_cast<size_t>(len), '\0');
    if (len > 0)
      ifile.read(&raw[0], len);

    // The name field is NUL-terminated; anything after the first NUL is
    // ignored, exactly like the engine's C-string FindObjectIDName().
    m_type[i] = FindObjectIDName(std::string(raw.c_str()));

    ifile >> m_total[i];
    ifile >> m_flags[i];
    ifile >> m_min[i];
    ifile >> m_max[i];
    ifile >> m_next_size[i];
    ifile >> m_next_do_time[i];
  }

  for (int i = 0; i < (int)m_type.size(); i++) {
    ifile >> m_cur_num[i];

    for (int j = 0; j < m_cur_num[i]; j++)
      ifile >> m_handle[i][j];
  }
}