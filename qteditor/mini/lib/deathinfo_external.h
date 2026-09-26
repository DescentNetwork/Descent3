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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/deathinfo_external.h $
 * $Revision: 10 $
 * $Date: 4/26/99 11:39p $
 * $Author: Matt $
 *
 * I don't know why we need this file, but here it is.
 *
 * $Log: /DescentIII/main/deathinfo_external.h $
 *
 * 10    4/26/99 11:39p Matt
 * Added a flag to make fly-in-air deaths no tumble
 *
 * 9     4/25/99 10:19p Matt
 * Fixed multiplayer and demo problems will killing an object from script,
 * and cleaned up the death code a bit in the process.
 *
 * 8     4/23/99 12:32a Matt
 * Added a death info option to play the explosion sound at the start of
 * the delay, instead of at the end when the object actually dies.
 *
 * 7     4/21/99 3:01p Matt
 * Added a new type for dying objects that have AI, instead of keeping a
 * flag in the dying info.
 *
 * 6     4/02/99 2:46p Matt
 * Moved flags from deathinfo to deathinfo_external, because the arhive
 * builder only copies the latter into the archive.
 *
 * 5     4/02/99 11:23a Matt
 * Made KillObject not take a death_info struct, but rather the death info
 * as individual parameters.  Moved death_info into objinfo.h, since it's
 * only used for generic objects.  Took out fade-away death hack, now that
 * fade-away deaths can be explicitely set.
 *
 */

#pragma once
//
//  Object death info flags
//

struct [[gnu::packed]] death_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 3;
  uint32_t delay_sound : 1;              // play sound at start of fade (0x1000000)
  uint32_t delay_no_tumble_fly : 1;      // don't tumble while flying up in the air (0x2000000)
  uint32_t fade_away : 1;                // fades away (0x0800000)
  uint32_t delay_fade_away : 1;          // fade away (0x0400000)
  uint32_t delay_fireball : 1;           // delay with fireballs (0x0200000)
  uint32_t delay_flying : 1;             // delay with object flying up into the air (0x0100000)
  uint32_t debris_remains : 1;           // stays around (0x0080000)
  uint32_t debris_blast_ring : 1;        // blast ring (0x0040000)
  uint32_t debris_fireball : 1;          // creates fireballs (0x0020000)
  uint32_t debris_ smokes : 1;           // the debris that's created smokes (0x0010000)
  uint32_t contact_remains : 1;          // stays around (0x0008000)
  uint32_t contact_blast_ring : 1;       // blast ring (0x0004000)
  uint32_t contact_breaks_apart : 1;     // break apart (0x0002000)
  uint32_t contact_fireball : 1;         // creates fireballs (0x0001000)
  uint32_t expl_size_mask_high : 1;      // part of DF_EXPL_SIZE_MASK (0x0000800 - large)
  uint32_t expl_size_mask_low : 1;       // part of DF_EXPL_SIZE_MASK (0x0000400 - medium)
  uint32_t loses_antigrav : 1;           // object gets gravity on death (0x0000200)
  uint32_t remains : 1;                  // the object does not go away when it does (0x0000100)
  uint32_t blast_ring : 1;               // a blast ring is created when the object dies (0x0000080)
  uint32_t breaks_apart : 1;             // the object breaks into pieces when it dies (0x0000040)
  uint32_t fireball : 1;                 // there are fireballs when the object dies (0x0000020)
  uint32_t delay_smokes : 1;             // delay with smoke (0x0000010)
  uint32_t delay_loses_antigrav : 1;     // object gets gravity during delay (0x0000008)
  uint32_t delay_sparks : 1;             // delay with sparks (0x0000004)
  uint32_t delay_from_anim : 1;          // delay time from death animation (0x0000002)
  uint32_t unused : 1;                   // Unused flag (0x0000001)
#else
  uint32_t unused : 1;                   // Unused flag (0x0000001)
  uint32_t delay_from_anim : 1;          // delay time from death animation (0x0000002)
  uint32_t delay_sparks : 1;             // delay with sparks (0x0000004)
  uint32_t delay_loses_antigrav : 1;     // object gets gravity during delay (0x0000008)
  uint32_t delay_smokes : 1;             // delay with smoke (0x0000010)
  uint32_t fireball : 1;                 // there are fireballs when the object dies (0x0000020)
  uint32_t breaks_apart : 1;             // the object breaks into pieces when it dies (0x0000040)
  uint32_t blast_ring : 1;               // a blast ring is created when the object dies (0x0000080)
  uint32_t remains : 1;                  // the object does not go away when it does (0x0000100)
  uint32_t loses_antigrav : 1;           // object gets gravity on death (0x0000200)
  uint32_t expl_size_mask_low : 1;       // part of DF_EXPL_SIZE_MASK (0x0000400 - medium)
  uint32_t expl_size_mask_high : 1;      // part of DF_EXPL_SIZE_MASK (0x0000800 - large)
  uint32_t contact_fireball : 1;         // creates fireballs (0x0001000)
  uint32_t contact_breaks_apart : 1;     // break apart (0x0002000)
  uint32_t contact_blast_ring : 1;       // blast ring (0x0004000)
  uint32_t contact_remains : 1;          // stays around (0x0008000)
  uint32_t debris_smokes : 1;            // the debris that's created smokes (0x0010000)
  uint32_t debris_fireball : 1;          // creates fireballs (0x0020000)
  uint32_t debris_blast_ring : 1;        // blast ring (0x0040000)
  uint32_t debris_remains : 1;           // stays around (0x0080000)
  uint32_t delay_flying : 1;             // delay with object flying up into the air (0x0100000)
  uint32_t delay_fireball : 1;           // delay with fireballs (0x0200000)
  uint32_t delay_fade_away : 1;          // fade away (0x0400000)
  uint32_t fade_away : 1;                // fades away (0x0800000)
  uint32_t delay_no_tumble_fly : 1;      // don't tumble while flying up in the air (0x2000000)
  uint32_t delay_sound : 1;              // play sound at start of fade (0x1000000)
  uint32_t padding : 3;
#endif
};
static_assert(sizeof(death_flags_t) == sizeof(uint32_t));

// Death info for an object type
struct death_info {
  death_flags_t flags;      // death flags // UPDATE_TYPE: `death_flags_t`
  float delay_min; // if delay, min amount
  float delay_max; // if delay, max amount
};
