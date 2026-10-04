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

 * $Logfile: /DescentIII/main/viseffect_external.h $
 * $Revision: 6 $
 * $Date: 3/21/00 9:58a $
 * $Author: Matt $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/main/viseffect_external.h $
 *
 * 6     3/21/00 9:58a Matt
 * Changed to Mac-only the code that sets a variable number of vis effects
 * based on texture quality.
 *
 * 5     3/20/00 2:27p Matt
 * Merge of Duane's post-1.3 changes.
 * Moved max_vis_effects (the variable, not the constant) from viseffect.h
 * to viseffect_external.h.
 *
 * 4     10/21/99 9:30p Jeff
 * B.A. Macintosh code merge
 *
 * 3     5/02/99 1:37a Jason
 * added moving object lighting viseffects
 *
 * 2     4/30/99 7:37p Jeff
 * created viseffect_external.h
 *
 * $NoKeywords: $
 */

#ifndef __VISEFFECT_EXTERNAL_H_
#define __VISEFFECT_EXTERNAL_H_

#include <cstdint>
#include "vecmat.h"

#define MAX_VIS_EFFECTS 5000

// types
#define VIS_NONE 0
#define VIS_FIREBALL 1

// Flags
struct [[gnu::packed]] vis_effect_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint16_t padding : 7 = 0;
  uint16_t link_to_viewer : 1 = 0; // Always link into the room that the viewer is in
  uint16_t no_z_adjust : 1 = 0;
  uint16_t attached : 1 = 0;
  uint16_t expand : 1 = 0;
  uint16_t reverse : 1 = 0;
  uint16_t planar : 1 = 0;
  uint16_t dead : 1 = 0;
  uint16_t windshield_effect : 1 = 0;
  uint16_t uses_lifeleft : 1 = 0;
#else
  uint16_t uses_lifeleft : 1 = 0;
  uint16_t windshield_effect : 1 = 0;
  uint16_t dead : 1 = 0;
  uint16_t planar : 1 = 0;
  uint16_t reverse : 1 = 0;
  uint16_t expand : 1 = 0;
  uint16_t attached : 1 = 0;
  uint16_t no_z_adjust : 1 = 0;
  uint16_t link_to_viewer : 1 = 0; // Always link into the room that the viewer is in
  uint16_t padding : 7 = 0;
#endif
};
static_assert(sizeof(vis_effect_flags_t) == sizeof(uint16_t));

extern uint16_t max_vis_effects;

struct object;

struct vis_attach_info {
  int obj_handle;
  int dest_objhandle;

  uint16_t modelnum;
  uint16_t vertnum;
  uint16_t end_vertnum;

  uint8_t subnum, subnum2;
};

struct axis_billboard_info {
  uint8_t width;
  uint8_t height;
  uint8_t texture;
};

struct vis_effect {
  vector3 pos;
  vector3 end_pos;
  vector3 velocity;
  float mass;
  float drag;
  float size;
  float lifeleft;
  float lifetime;
  float creation_time;

  int roomnum;

  int phys_flags;

  int16_t custom_handle;
  uint16_t lighting_color;

  vis_effect_flags_t flags = {};

  int16_t next;
  int16_t prev;

  vis_attach_info attach_info;
  axis_billboard_info billboard_info;

  uint8_t movement_type;
  uint8_t type, id;
};

#endif
