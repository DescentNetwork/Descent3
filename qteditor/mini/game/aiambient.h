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
#include <posix_stream.h>
#include <optional>
#include "utils.h"


#define MAX_AL_TYPES 6
#define MAX_ALS_PER_TYPE 130

// ambient_life flags
struct [[gnu::packed]] ambient_life_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint8_t padding : 7;
  uint8_t inside : 1; // ALF_INSIDE (0x01)
#else
  uint8_t inside : 1; // ALF_INSIDE (0x01)
  uint8_t padding : 7;
#endif
};
static_assert(sizeof(ambient_life_flags_t) == sizeof(uint8_t));

// Field selectors for GetALValue/SetALValue (engine ALI_*).
enum class ambient_field : uint8_t
{
  type = 0,  // ALI_TYPE
  total = 1, // ALI_TOTAL
  max = 2,   // ALI_MAX
  min = 3,   // ALI_MIN
  flags = 4, // ALI_FLAGS
};

#define CHECK_INTERVAL_MIN 5.0f
#define CHECK_INTERVAL_MAX 10.0f

class ambient_life
{
  // Editor settable values
  struct setting_t
  {
    index_t type;
    std::string name;
    uint8_t total;
    uint8_t max;
    uint8_t min;
    ambient_life_flags_t flags;
    uint8_t next_size;
    float next_do_time;

    void reset(void);
    void compute_next_size(void);
  };
  friend byte_ostream& operator<<(byte_ostream& os, const setting_t& al);
  friend byte_istream& operator>>(byte_istream& is, setting_t& al);

  // Editor settable values
  std::array<setting_t, MAX_AL_TYPES> m_settings;

  // These are never set or gotten from outside of the class
  std::array<uint8_t, MAX_AL_TYPES> m_cur_num;
  std::array<std::array<int, MAX_ALS_PER_TYPE>, MAX_AL_TYPES> m_handle;
public:
  ambient_life() { reset(); }

  void GetALValue(uint8_t index, ambient_field field, void *ptr) const;
  void SetALValue(uint8_t index, ambient_field field, void *ptr);

  void DoFrame(void);
  void InitForLevel(void);
  void reset(void);

  friend byte_ostream& operator<<(byte_ostream& os, const ambient_life& al);
  friend byte_istream& operator>>(byte_istream& is, ambient_life& al);
};


extern ambient_life a_life;

#endif
