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

 * $Logfile: /DescentIII/main/player_external.h $
 * $Revision: 7 $
 * $Date: 5/23/99 3:06a $
 * $Author: Jason $
 *
 * Includes the flags, defines, etc. for player struct and functions (for DLL export)
 *
 * $Log: /DescentIII/main/player_external.h $
 *
 * 7     5/23/99 3:06a Jason
 * fixed bug with player rankings not being updated correctly
 *
 * 6     5/10/99 12:23a Chris
 * Fixed another hearing/seeing case.  :)  Buddy bot now is in the player
 * ship at respawn
 *
 * 5     4/24/99 6:45p Jeff
 * added functions for theif so he can steal things other than weapons
 *
 * 4     2/25/99 8:55p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 3     2/12/99 3:39p Jason
 * added client side interpolation
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef __PLAYER_EXTERNAL_H_
#define __PLAYER_EXTERNAL_H_

#include <cstdint>

#define N_PLAYER_GUNS 8

// Initial player stat values
#define INITIAL_ENERGY 100  // 100% energy to start
#define INITIAL_SHIELDS 100 // 100% shields to start

#define MAX_ENERGY 200 // go up to 200
#define MAX_SHIELDS 200

// Observer modes
enum class observer_mode : uint8_t {
  roam = 0,
  piggyback = 1,
};

// Values for player::flags
struct [[gnu::packed]] player_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 9;
  uint32_t playsoundmsgforinvuln : 1; // A sound and hud msg when invulnerability wears off (1<<22)
  uint32_t send_movement : 1;         // We need to tell the server about our movement (1<<21)
  uint32_t rearview : 1;              // Play is using rearview (1<<20)
  uint32_t zoomed : 1;                // Player is zoomed in (1<<19)
  uint32_t bullseye : 1;              // Bullseye reticle should light up (1<<18)
  uint32_t thrusted : 1;              // Player has thrusted this frame (1<<17)
  uint32_t custom_texture : 1;        // Player has a custom texture (65536)
  uint32_t afterburn_on : 1;          // Player afterburner is engaged (32768)
  uint32_t headlight_stolen : 1;      // is the headlight stolen? (16384)
  uint32_t headlight : 1;             // Player has headlight boost (8192)
  uint32_t afterburner : 1;           // Player has an afterburner (4096)
  uint32_t : 8;                       // 2048..16 legacy/unused
  uint32_t dead : 1;                  // The player is just sitting there dead. (8)
  uint32_t dying : 1;                 // Is this player in the middle of dying? (4)
  uint32_t : 1;                       // 2 unused
  uint32_t invulnerable : 1;          // Player is invincible (1)
#else
  uint32_t invulnerable : 1; // Player is invincible (1)
  uint32_t : 1;              // 2 unused
  uint32_t dying : 1;        // Is this player in the middle of dying? (4)
  uint32_t dead : 1;         // The player is just sitting there dead. (8)
  uint32_t : 8;              // 16..2048 legacy/unused
  uint32_t afterburner : 1;  // Player has an afterburner (4096)
  uint32_t headlight : 1;    // Player has headlight boost (8192)
  uint32_t headlight_stolen : 1; // is the headlight stolen? (16384)
  uint32_t afterburn_on : 1; // Player afterburner is engaged (32768)
  uint32_t custom_texture : 1; // Player has a custom texture (65536)
  uint32_t thrusted : 1;     // Player has thrusted this frame (1<<17)
  uint32_t bullseye : 1;     // Bullseye reticle should light up (1<<18)
  uint32_t zoomed : 1;       // Player is zoomed in (1<<19)
  uint32_t rearview : 1;     // Play is using rearview (1<<20)
  uint32_t send_movement : 1; // We need to tell the server about our movement (1<<21)
  uint32_t playsoundmsgforinvuln : 1; // A sound and hud msg when invulnerability wears off (1<<22)
  uint32_t padding : 9;
#endif
};
static_assert(sizeof(player_flags_t) == sizeof(uint32_t));

// Player start position flags (used for teams)
struct [[gnu::packed]] player_start_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 28;
  uint32_t yellow : 1;
  uint32_t green : 1;
  uint32_t blue : 1;
  uint32_t red : 1;
#else
  uint32_t red : 1;
  uint32_t blue : 1;
  uint32_t green : 1;
  uint32_t yellow : 1;
  uint32_t padding : 28;
#endif
};
static_assert(sizeof(player_start_flags_t) == sizeof(uint32_t));

// Define the two player weapons
enum class player_weapon_slot : uint8_t {
  primary = 0,
  secondary = 1,
};

// These are used to mask out various controls, put in initially for the training system
struct [[gnu::packed]] player_control_block_flags_t
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint16_t : 1;
  uint16_t afterburner : 1;
  uint16_t secondary : 1;
  uint16_t primary : 1;
  uint16_t bankright : 1;
  uint16_t bankleft : 1;
  uint16_t headingright : 1;
  uint16_t headingleft : 1;
  uint16_t pitchdown : 1;
  uint16_t pitchup : 1;
  uint16_t down : 1;
  uint16_t up : 1;
  uint16_t right : 1;
  uint16_t left : 1;
  uint16_t reverse : 1;
  uint16_t forward : 1;
#else
  uint16_t forward : 1;
  uint16_t reverse : 1;
  uint16_t left : 1;
  uint16_t right : 1;
  uint16_t up : 1;
  uint16_t down : 1;
  uint16_t pitchup : 1;
  uint16_t pitchdown : 1;
  uint16_t headingleft : 1;
  uint16_t headingright : 1;
  uint16_t bankleft : 1;
  uint16_t bankright : 1;
  uint16_t primary : 1;
  uint16_t secondary : 1;
  uint16_t afterburner : 1;
  uint16_t : 1;
#endif
};
static_assert(sizeof(player_control_block_flags_t) == sizeof(uint16_t));
// The DYNAMIC weapon battery flags (WFF_*) live on the object as
// weapon_fire_flags_t (object_external_struct.h).

#endif
