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

 * $Logfile: /DescentIII/main/BOA.h $
 * $Revision: 34 $
 * $Date: 4/30/99 5:40p $
 * $Author: Jason $
 *
 * BOA Header
 *
 * $Log: /DescentIII/main/BOA.h $
 *
 * 34    4/30/99 5:40p Jason
 * fixed some issues with release builds
 *
 * 33    4/29/99 5:43p Chris
 * Added the check for bad center points
 *
 * 32    4/26/99 11:11a Chris
 * Updated Bnode system
 *
 * 31    4/20/99 8:55p Chris
 * Fixed problem with robots not being able to open locked doors that a
 * player has the key for.
 *
 * 30    4/18/99 5:39a Chris
 * Vastly improved the path node system
 *
 * 29    2/25/99 5:43p Chris
 * Massive improvement to BOA and AI (again)
 *
 * 28    2/24/99 12:27p Chris
 * Fixed problems with GB finding robots/room he couldn't get to.  Fixed
 * problems with forcefields(sound prop. and path finding).  Fixed
 * problems with small portals.
 *
 * 27    2/12/99 11:22a Chris
 * Updated what BOA uses for BOA_cost_array.  It is not just the distance
 * from the center of the room to the center of the portal.  It was this +
 * the dist to the next room's center.  The reason for this change was it
 * makes it easier to compute the exact BOA_DIST for sounds.  This is
 * because the two distances are separated (so from the start pos, we do a
 * vm_Vect dist from the start point to the portal)...  Hmmm....
 *
 * 26    2/09/99 12:41p Chris
 *
 * 25    2/09/99 12:40p Chris
 * More aipath stuff has been merged with the new BOA
 *
 * 24    2/09/99 12:17p Chris
 *
 * 23    2/09/99 9:58a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 22    1/11/99 2:14p Chris
 * Massive work on OSIRIS and AI
 *
 * 21    12/11/98 1:55p Chris
 * Reduced BOA's size and changed how it works with no path situations
 *
 * 20    12/09/98 10:41a Jason
 * fixed BOA problems with lighting
 *
 * 19    9/16/98 12:07p Chris
 * Improved BOA AABB computation with room checksums
 *
 * 18    8/04/98 2:32p Chris
 * Improved attach code added more fixes to the AABB partial computation
 * patch
 *
 * 17    8/03/98 5:47p Chris
 * Improved the partial AABB computation
 *
 * 16    7/21/98 2:14p Chris
 * Some FVI speedups - not done
 *
 * 15    7/20/98 5:39p Jason
 * fixed MACRO bug
 *
 * 14    7/20/98 4:45p Jason
 * added BOA vis table stuff
 *
 * 13    7/16/98 8:29p Chris
 * Intermediate checkin
 *
 * 12    7/16/98 8:29p Chris
 * Partial implementation of the new collide code
 *
 * 11    5/03/98 8:36p Chris
 * Additional debug info
 *
 * 10    4/28/98 12:01p Matt
 * Dropped MAX_PATH_PORTALS from 100 down to 40.
 *
 * 9     4/22/98 6:38p Matt
 * Added SourceSafe header
 *
 */

#ifndef _BOA_H__
#define _BOA_H__

// "Blockage/Occulsion Array" (where robots/sound/light are blocked)
// Axis-Aligned Bounding Box

#include "room.h"
#include "terrain.h"
#include "utils.h"

// One packed BOA_Array entry: the next room to step through on the path from
// this pair, plus the visibility/propagation/blockage bits recorded for it.
struct boa_array_entry_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint16_t too_small_for_robot : 1; // path has a portal too small for a robot
  uint16_t blockage : 1;            // something blocks sound/light/robots here
  uint16_t distance : 2;            // boa_distance bucket for this pair
  uint16_t sound_prop : 1;          // a sound could prop between these two areas
  uint16_t vis : 1;                 // the two areas can see each other
  uint16_t next_room : 10;
#else
  uint16_t next_room : 10;
  uint16_t vis : 1;                 // the two areas can see each other
  uint16_t sound_prop : 1;          // a sound could prop between these two areas
  uint16_t distance : 2;            // boa_distance bucket for this pair
  uint16_t blockage : 1;            // something blocks sound/light/robots here
  uint16_t too_small_for_robot : 1; // path has a portal too small for a robot
#endif
};
static_assert(sizeof(boa_array_entry_t) == sizeof(uint16_t));

// Distance bucket stored in boa_array_entry_t::distance
enum class boa_distance : uint8_t {
  vnear = 0,    // 0.0   to 100.0
  near_dist = 1,// 100.0 to 200.0
  far_dist = 2, // 200.0 to 300.0
  vfar_dist = 3,// 300.0+
};

#define MAX_PATH_PORTALS 40
#define MAX_BOA_TERRAIN_REGIONS 8
extern std::array<std::array<float, MAX_PATH_PORTALS>, MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS> BOA_cost_array;

extern std::array<std::array<boa_array_entry_t, MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS>, MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS>
    BOA_Array;
extern int BOA_mine_checksum;
extern int BOA_AABB_checksum;
extern int BOA_vis_checksum;
extern bool BOA_vis_valid;
extern std::array<int, MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS> BOA_AABB_ROOM_checksum;

//--  Priority Queue
class q_item {
public:
  q_item(int room_index, int par, float n_cost) {
    roomnum = room_index;
    parent = par;
    cost = n_cost;
    next = NULL;
  }

  int roomnum;
  int parent;
  float cost;

  q_item *next;
};

class pq {
  q_item *q_head;

public:
  pq() { q_head = NULL; }

  void push(q_item *new_node) {
    new_node->next = q_head;
    q_head = new_node;
  }

  q_item *pop() {
    q_item *cur_item = q_head;
    q_item *prev_item = NULL;
    q_item *best_item = q_head;
    q_item *best_prev_item = NULL;

    if (q_head == NULL)
      return NULL;

    while (cur_item != NULL) {
      if (cur_item->cost < best_item->cost) {

        best_prev_item = prev_item;
        best_item = cur_item;
      }

      prev_item = cur_item;
      cur_item = cur_item->next;
    }

    // Actually remove this item from the list
    if (best_item == q_head) {
      q_head = best_item->next;
    } else {
      best_prev_item->next = best_item->next;
    }

    return best_item;
  }
};

// Next Segment Info

inline int BOA_TERRAIN_INDEX() { return static_cast<int>(Rooms.size()); }
inline int BOA_NO_PATH() { return static_cast<int>(Rooms.size()) - 1 + 9; }
inline int BOA_INDEX(int roomnum) {
  return ROOMNUM_OUTSIDE(roomnum) ? static_cast<int>(Terrain_seg[roomnum].flags.region + Rooms.size()) : roomnum;
}

// Visibility Info

inline uint16_t BOA_NEXT_ROOM(int a, int b) { return static_cast<uint16_t>(BOA_Array[a][b].next_room); }

inline bool BOA_GET_VIS(int a, int b) { return BOA_vis_valid ? (BOA_Array[a][b].vis != 0) : true; }

inline bool BOA_GET_DIST(int a, int b) { return (BOA_Array[a][b].distance != 0); }

inline bool BOA_HAS_POSSIBLE_BLOCKAGE(int a, int b) { return (BOA_Array[a][b].blockage != 0); }

inline bool BOA_TOO_SMALL_FOR_ROBOT(int a, int b) { return (BOA_Array[a][b].too_small_for_robot != 0); }

// Distance from segment to segment Info

inline constexpr float BOA_VNEAR_DIST = 0.0f;
inline constexpr float BOA_NEAR_DIST = 100.0f;
inline constexpr float BOA_FAR_DIST = 200.0f;
inline constexpr float BOA_VFAR_DIST = 300.0f;

struct connect_data {
  index_t roomnum;
  index_t portal;
};

extern int BOA_num_mines;
extern int BOA_num_terrain_regions;
extern int BOA_num_connect[MAX_BOA_TERRAIN_REGIONS];
extern std::array<std::array<connect_data, MAX_PATH_PORTALS>, MAX_BOA_TERRAIN_REGIONS> BOA_connect;

void MakeBOA(void);

// Goes through all the rooms and determines their visibility in relation to one another
void MakeBOAVisTable(bool from_lighting = 0);

// Computes the AABBs for the level
void ComputeAABB(bool f_full);

bool BOA_ComputeMinDist(int start_room, int end_room, float max_check_dist, float &dist, optref<int> num_blockages = std::nullopt);
bool BOA_IsSoundAudible(int start_room, int end_room);
bool BOA_IsVisible(int start_room, int end_room);
bool BOA_HasPossibleBlockage(int start_room, int end_room);
index_t BOA_GetNextRoom(int start_room, int end_room);
index_t BOA_DetermineStartRoomPortal(int start_room, optref<vector3> start_pos, int end_room,
                                                     optref<vector3> end_pos, bool f_for_sound = false,
                                                     bool f_making_robot_path_invalid_list = false,
                                                     optref<int> blocked_portal = std::nullopt);
bool BOA_PassablePortal(int room, int portal_index, bool f_for_sound = false,
                        bool f_making_robot_path_invalid_list = false);
bool BOA_LockedDoor(object *obj, int roomnum);
void BOA_ComputePathPoints(char *message = NULL, int len = 0);
int BOAGetMineChecksum();

#endif
