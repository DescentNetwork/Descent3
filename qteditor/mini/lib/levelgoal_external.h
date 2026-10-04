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

#ifndef LEVELGOALEXTERNAL_H_
#define LEVELGOALEXTERNAL_H_

#include <cstdint>

// lgoal::m_flags.  Bits 8-13 are the six independent LGF_COMP_* completion
// triggers, so they stay separate bits rather than one field.
struct [[gnu::packed]] lgoal_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 18;           // Unused padding to complete 32 bits
  uint32_t comp_dallas : 1;        // LGF_COMP_DALLAS (0x2000)
  uint32_t comp_player : 1;        // LGF_COMP_PLAYER (0x1000)
  uint32_t comp_player_weapon : 1; // LGF_COMP_PLAYER_WEAPON (0x0800)
  uint32_t comp_destroy : 1;       // LGF_COMP_DESTROY (0x0400)
  uint32_t comp_enter : 1;         // LGF_COMP_ENTER (0x0200)
  uint32_t comp_activate : 1;      // LGF_COMP_ACTIVATE (0x0100)
  uint32_t failed : 1;             // LGF_FAILED (0x0080)
  uint32_t not_loc_based : 1;      // LGF_NOT_LOC_BASED (0x0040)
  uint32_t gb_doesnt_know_loc : 1; // LGF_GB_DOESNT_KNOW_LOC (0x0020)
  uint32_t telcom_lists : 1;       // LGF_TELCOM_LISTS (0x0010)
  uint32_t completed : 1;          // LGF_COMPLETED (0x0008)
  uint32_t enabled : 1;            // LGF_ENABLED (0x0004)
  uint32_t secondary_goal : 1;     // LGF_SECONDARY_GOAL (0x0002)
  uint32_t blank1 : 1;             // LGF_BLANK1 (0x0001)
#else
  uint32_t blank1 : 1;             // LGF_BLANK1 (0x0001)
  uint32_t secondary_goal : 1;     // LGF_SECONDARY_GOAL (0x0002)
  uint32_t enabled : 1;            // LGF_ENABLED (0x0004)
  uint32_t completed : 1;          // LGF_COMPLETED (0x0008)
  uint32_t telcom_lists : 1;       // LGF_TELCOM_LISTS (0x0010)
  uint32_t gb_doesnt_know_loc : 1; // LGF_GB_DOESNT_KNOW_LOC (0x0020)
  uint32_t not_loc_based : 1;      // LGF_NOT_LOC_BASED (0x0040)
  uint32_t failed : 1;             // LGF_FAILED (0x0080)
  uint32_t comp_activate : 1;      // LGF_COMP_ACTIVATE (0x0100)
  uint32_t comp_enter : 1;         // LGF_COMP_ENTER (0x0200)
  uint32_t comp_destroy : 1;       // LGF_COMP_DESTROY (0x0400)
  uint32_t comp_player_weapon : 1; // LGF_COMP_PLAYER_WEAPON (0x0800)
  uint32_t comp_player : 1;        // LGF_COMP_PLAYER (0x1000)
  uint32_t comp_dallas : 1;        // LGF_COMP_DALLAS (0x2000)
  uint32_t padding : 18;           // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(lgoal_flags_t) == sizeof(uint32_t));

// levelgoals::m_flags.
struct [[gnu::packed]] levelgoals_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 30;        // Unused padding to complete 32 bits
  uint32_t all_primaries_done : 1; // LF_ALL_PRIMARIES_DONE (0x02)
  uint32_t auto_end_level : 1;     // LF_AUTO_END_LEVEL (0x01)
#else
  uint32_t auto_end_level : 1;     // LF_AUTO_END_LEVEL (0x01)
  uint32_t all_primaries_done : 1; // LF_ALL_PRIMARIES_DONE (0x02)
  uint32_t padding : 30;        // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(levelgoals_flags_t) == sizeof(uint32_t));

// Level Item Types
#define LIT_TERRAIN_CELL 0
#define LIT_INTERNAL_ROOM 1
#define LIT_OBJECT 2
#define LIT_TRIGGER 3
#define LIT_ANY_MINE 4

// Level Item Operations
#define LO_SET_SPECIFIED 0
#define LO_GET_SPECIFIED 1
#define LO_CLEAR_SPECIFIED 2

#define MAX_GOAL_ITEMS 12
#define MAX_LEVEL_GOALS 32

#define MAX_GOAL_LISTS 4

#endif
