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

#ifndef MATCEN_EXTERNAL_H_
#define MATCEN_EXTERNAL_H_

#include <cstdint>

#define MAX_PROD_TYPES 8
#define MAX_SPAWN_PNTS 4

#define MATCEN_ERROR -1

struct matcen_status_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 19;                 // Unused padding to complete 32 bits
  uint32_t not_hurt_player : 1;
  uint32_t compute_create_pnt_every_frame : 1;
  uint32_t manual_update_create_pnt : 1;
  uint32_t prod_one_disable : 1;
  uint32_t prod_one_pause : 1;
  uint32_t prod_till_done : 1;
  uint32_t random_prod_order : 1;
  uint32_t done_prod : 1;
  uint32_t never_prod : 1;
  uint32_t create_obj_frame : 1;
  uint32_t active_pause : 1;
  uint32_t active : 1;
  uint32_t disabled : 1;
#else
  uint32_t disabled : 1;
  uint32_t active : 1;
  uint32_t active_pause : 1;
  uint32_t create_obj_frame : 1;
  uint32_t never_prod : 1;
  uint32_t done_prod : 1;
  uint32_t random_prod_order : 1;
  uint32_t prod_till_done : 1;
  uint32_t prod_one_pause : 1;
  uint32_t prod_one_disable : 1;
  uint32_t manual_update_create_pnt : 1;
  uint32_t compute_create_pnt_every_frame : 1;
  uint32_t not_hurt_player : 1;
  uint32_t padding : 19;                 // Unused padding to complete 32 bits
#endif
};

enum class matcen_prod_mode : uint8_t {
  notprod = 0,
  preprod = 1,
  postprod = 2,
};

// MATCEN NOTE:  Make sure to add the name of the effect to the list
// in matcen.cpp
enum class matcen_effect : uint8_t {
  line_lightning = 0,
  line_sine_wave = 1,
  procedural_lightning = 2,
  none = 3,
  count = 4,
};

enum class matcen_control_type : uint8_t {
  script = 0,
  while_player_near = 1,
  after_player_near = 2,
  while_player_visible = 3,
  after_player_visible = 4,
  count = 5,
};

enum class matcen_sound : uint8_t {
  active = 0,
  disable = 1,
  prod = 2,
  count = 3,
};

enum class matcen_type : uint8_t {
  object = 0,
  room = 1,
  unassigned = 2,
};

#endif
