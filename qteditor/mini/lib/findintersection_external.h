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

 * $Logfile: /DescentIII/main/lib/findintersection_external.h $
 * $Revision: 6 $
 * $Date: 4/18/99 5:42a $
 * $Author: Chris $
 *
 * Description goes here
 *
 * $Log: /DescentIII/main/lib/findintersection_external.h $
 *
 * 6     4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 *
 * 5     4/05/99 3:58p Chris
 * Made the FQ_ flags easier to read
 *
 * 4     3/27/99 3:25p Chris
 * Added comment headers
 *
 * $NoKeywords: $
 */

#ifndef FINDINTERSECTION_EXTERNAL_H_
#define FINDINTERSECTION_EXTERNAL_H_

// return values for find_vector_intersection()
#define HIT_NONE 0                  // we hit nothing
#define HIT_WALL 1                  // we hit a wall
#define HIT_OBJECT 2                // we hit an object
#define HIT_TERRAIN 3               // we hit the terrain
#define HIT_BAD_P0 4                // start point not is specified segment
#define HIT_OUT_OF_TERRAIN_BOUNDS 5 // End point is outside of the terrain
#define HIT_BACKFACE 6              // We hit the backface of a wall...
#define HIT_SPHERE_2_POLY_OBJECT 7  // Hit a sphere to a real polygon
#define HIT_CEILING 8               // Object hit the ceiling
#define HIT_CORNER_WALL 9
#define HIT_EDGE_WALL 10
#define HIT_FACE_WALL 11

// fvi_query::flags
struct [[gnu::packed]] fvi_query_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 6;               // Unused padding to complete 32 bits
  uint32_t ignore_render_through_portals : 1;  // FQ_IGNORE_RENDER_THROUGH_PORTALS (1 << 25)
  uint32_t ignore_clutter_collisions : 1;  // FQ_IGNORE_CLUTTER_COLLISIONS (1 << 24)
  uint32_t robots_as_sphere : 1;      // FQ_ROBOTS_AS_SPHERE (1 << 23)
  uint32_t players_as_sphere : 1;     // FQ_PLAYERS_AS_SPHERE (1 << 22)
  uint32_t ignore_terrain : 1;        // FQ_IGNORE_TERRAIN (1 << 21)
  uint32_t ignore_weapons : 1;        // FQ_IGNORE_WEAPONS (1 << 20)
  uint32_t ignore_external_rooms : 1;  // FQ_IGNORE_EXTERNAL_ROOMS (1 << 19)
  uint32_t compute_movement_time : 1;  // FQ_COMPUTE_MOVEMENT_TIME (1 << 18)
  uint32_t lighting : 1;              // FQ_LIGHTING (1 << 17): Lighting only optimizations
  uint32_t multi_point : 1;           // FQ_MULTI_POINT (1 << 16): Enable Multi-point collision
  uint32_t external_rooms_as_sphere : 1;  // FQ_EXTERNAL_ROOMS_AS_SPHERE (1 << 15): Hmmm....
  uint32_t no_relink : 1;             // FQ_NO_RELINK (1 << 14): Does not determine the hitseg
  uint32_t only_door_obj : 1;         // FQ_ONLY_DOOR_OBJ (1 << 13): Ignores all objects, except doors
  uint32_t check_ceiling : 1;         // FQ_CHECK_CEILING (1 << 12): Checks if object hits the imaginary ceiling
  uint32_t ignore_walls : 1;          // FQ_IGNORE_WALLS (1 << 11): Ignores all walls (it will still hit OBJ_ROOMS)
  uint32_t only_player_obj : 1;       // FQ_ONLY_PLAYER_OBJ (1 << 10): Ignores all objects besides the player
  uint32_t ignore_non_lightmap_objects : 1;  // FQ_IGNORE_NON_LIGHTMAP_OBJECTS (1 << 9): Ignores all objects that are not associated with lightmaps
  uint32_t ignore_moving_objects : 1;  // FQ_IGNORE_MOVING_OBJECTS (1 << 8): Ignores all objects that move
  uint32_t new_record_list : 1;       // FQ_NEW_RECORD_LIST (1 << 7): Records faces that should be recorded
  uint32_t record : 1;                // FQ_RECORD (1 << 6): Records faces that should be recorded
  uint32_t solid_portals : 1;         // FQ_SOLID_PORTALS (1 << 5): Makes connectivity disappear for FVI
  uint32_t backface : 1;              // FQ_BACKFACE (1 << 4): Check for collisions with backfaces, usually they are ignored
  uint32_t ignore_powerups : 1;       // FQ_IGNORE_POWERUPS (1 << 3): ignore powerups
  uint32_t transpoint : 1;            // FQ_TRANSPOINT (1 << 2): go through trans wall if hit point is transparent
  uint32_t obj_backface : 1;          // FQ_OBJ_BACKFACE (1 << 1): Hit the backfaces of polyobjs
  uint32_t check_objs : 1;            // FQ_CHECK_OBJS (1 << 0): check against objects?
#else
  uint32_t check_objs : 1;            // FQ_CHECK_OBJS (1 << 0): check against objects?
  uint32_t obj_backface : 1;          // FQ_OBJ_BACKFACE (1 << 1): Hit the backfaces of polyobjs
  uint32_t transpoint : 1;            // FQ_TRANSPOINT (1 << 2): go through trans wall if hit point is transparent
  uint32_t ignore_powerups : 1;       // FQ_IGNORE_POWERUPS (1 << 3): ignore powerups
  uint32_t backface : 1;              // FQ_BACKFACE (1 << 4): Check for collisions with backfaces, usually they are ignored
  uint32_t solid_portals : 1;         // FQ_SOLID_PORTALS (1 << 5): Makes connectivity disappear for FVI
  uint32_t record : 1;                // FQ_RECORD (1 << 6): Records faces that should be recorded
  uint32_t new_record_list : 1;       // FQ_NEW_RECORD_LIST (1 << 7): Records faces that should be recorded
  uint32_t ignore_moving_objects : 1;  // FQ_IGNORE_MOVING_OBJECTS (1 << 8): Ignores all objects that move
  uint32_t ignore_non_lightmap_objects : 1;  // FQ_IGNORE_NON_LIGHTMAP_OBJECTS (1 << 9): Ignores all objects that are not associated with lightmaps
  uint32_t only_player_obj : 1;       // FQ_ONLY_PLAYER_OBJ (1 << 10): Ignores all objects besides the player
  uint32_t ignore_walls : 1;          // FQ_IGNORE_WALLS (1 << 11): Ignores all walls (it will still hit OBJ_ROOMS)
  uint32_t check_ceiling : 1;         // FQ_CHECK_CEILING (1 << 12): Checks if object hits the imaginary ceiling
  uint32_t only_door_obj : 1;         // FQ_ONLY_DOOR_OBJ (1 << 13): Ignores all objects, except doors
  uint32_t no_relink : 1;             // FQ_NO_RELINK (1 << 14): Does not determine the hitseg
  uint32_t external_rooms_as_sphere : 1;  // FQ_EXTERNAL_ROOMS_AS_SPHERE (1 << 15): Hmmm....
  uint32_t multi_point : 1;           // FQ_MULTI_POINT (1 << 16): Enable Multi-point collision
  uint32_t lighting : 1;              // FQ_LIGHTING (1 << 17): Lighting only optimizations
  uint32_t compute_movement_time : 1;  // FQ_COMPUTE_MOVEMENT_TIME (1 << 18)
  uint32_t ignore_external_rooms : 1;  // FQ_IGNORE_EXTERNAL_ROOMS (1 << 19)
  uint32_t ignore_weapons : 1;        // FQ_IGNORE_WEAPONS (1 << 20)
  uint32_t ignore_terrain : 1;        // FQ_IGNORE_TERRAIN (1 << 21)
  uint32_t players_as_sphere : 1;     // FQ_PLAYERS_AS_SPHERE (1 << 22)
  uint32_t robots_as_sphere : 1;      // FQ_ROBOTS_AS_SPHERE (1 << 23)
  uint32_t ignore_clutter_collisions : 1;  // FQ_IGNORE_CLUTTER_COLLISIONS (1 << 24)
  uint32_t ignore_render_through_portals : 1;  // FQ_IGNORE_RENDER_THROUGH_PORTALS (1 << 25)
  uint32_t padding : 6;               // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(fvi_query_flags_t) == sizeof(uint32_t));


#endif
