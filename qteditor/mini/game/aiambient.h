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

#ifndef _AIAMBIENT_H_
#define _AIAMBIENT_H_

#include <array>
#include <cstdint>
#include "cfile.h"

#define MAX_AL_TYPES 6
#define MAX_ALS_PER_TYPE 130

#define ALF_INSIDE 0x01

#define ALI_TYPE 0
#define ALI_TOTAL 1
#define ALI_MAX 2
#define ALI_MIN 3
#define ALI_FLAGS 4

#define CHECK_INTERVAL_MIN 5.0f
#define CHECK_INTERVAL_MAX 10.0f

class ambient_life {
  // Editor settable values
  std::array<int, MAX_AL_TYPES> m_type;
  std::array<uint8_t, MAX_AL_TYPES> m_total;
  std::array<uint8_t, MAX_AL_TYPES> m_max;
  std::array<uint8_t, MAX_AL_TYPES> m_min;
  std::array<uint8_t, MAX_AL_TYPES> m_flags;

  // These are never set or gotten from outside of the class
  std::array<uint8_t, MAX_AL_TYPES> m_cur_num;
  std::array<std::array<int, MAX_ALS_PER_TYPE>, MAX_AL_TYPES> m_handle;

  // Don't save these...
  std::array<uint8_t, MAX_AL_TYPES> m_next_size;
  std::array<float, MAX_AL_TYPES> m_next_do_time;

  void ComputeNextSize(int8_t index);

public:
  ambient_life() { ALReset(); }

  void GetALValue(int8_t index, char field, void *ptr);
  void SetALValue(int8_t index, char field, void *ptr);
  void SaveData(struct CFILE* fptr);
  void LoadData(struct CFILE* fptr);

  void DoFrame();
  void InitForLevel(void);
  void ALReset();
};

extern ambient_life a_life;

#endif
