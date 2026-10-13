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

 * $Logfile: /DescentIII/main/gamepath.h $
 * $Revision: 12 $
 * $Date: 10/08/98 4:23p $
 * $Author: Kevin $
 *
 * Header for gamepath.cpp
 *
 * $Log: /DescentIII/main/gamepath.h $
 *
 * 12    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 11    2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 *
 */

#ifndef GAME_PATH_H
#define GAME_PATH_H

#include <cstdlib>
#include <cstdint>
#include <optional>
#include <vector>

#include "3d.h"
#include "manage.h"
#include "mem/mem.h"
#include "slotvec.h"
#include "vecmat.h"
#include "utils.h"

// chrishack -- this could be dynamically allocated at the beginning of a level
// MAX_NODES_PER_PATH is big and so is MAX_GAME_PATHS

#define MAX_GAME_PATHS 300
#define MAX_NODES_PER_PATH 100

// Game-path node flags.  No bits are currently defined; the field is reserved
// and only ever stores the value passed to InsertNodeIntoPath (callers pass 0),
// but it is serialized in the PATH chunk so its reserved bits are preserved.
struct [[gnu::packed]] node_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 32; // Unused padding to complete 32 bits
#else
  uint32_t padding : 32; // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(node_flags_t) == sizeof(uint32_t));

struct node {
  vector3 pos;  // where this node is in the world
  int roomnum; // what room?
  node_flags_t flags;   // if this point lives over the terrain, etc
  vector3 fvec;
  vector3 uvec;
};

// Game-path flags.  No bits are currently defined; the field is reserved and
// only ever set to 0 (AllocGamePath), but it is serialized in the PATH chunk so
// its reserved bits are preserved.
struct [[gnu::packed]] game_path_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint8_t padding : 8; // Unused padding to complete 8 bits
#else
  uint8_t padding : 8; // Unused padding to complete 8 bits
#endif
};
static_assert(sizeof(game_path_flags_t) == sizeof(uint8_t));

class game_path {
public:
  game_path() { num_nodes = 0; }

  std::vector<node> pathnodes; // how many nodes in this path? (count kept in num_nodes)
  int num_nodes;           // how many nodes in this path?
  std::string name; // the name of this path
  game_path_flags_t flags;  // special properties of this path
};

extern d3::slotvec_t<game_path> GamePaths;

void InitGamePaths();

// searches through GamePath index and returns index of path matching name
// returns -1 if not found
index_t FindGamePathName(const std::string &name);


extern bool Show_paths;

// Allocs a gamepath that a robot will follow.  Returns an index into the GamePaths
// array
index_t AllocGamePath(void);

// Given a path number, and a node number in that path, adds another node after the
// specified node
// Returns the index number of the new node
// If nodenum is -1, this node couldn't be added
// Flags are passed via the flags field
int InsertNodeIntoPath(int pathnum, int nodenum, int flags);

void FreeGamePath(uint32_t n);

// Given a pathnum and a node index, deletes that node and moves all the following nodes down
// by one
void DeleteNodeFromPath(int pathnum, int nodenum);

// Given a path number and a node, it moves the node by the change in position (if the new position is valid)
int MovePathNode(int pathnum, int nodenum, vector3 *delta_pos);

// Given a path number and a node, it moves the node to the position (if the new position is valid)
int MovePathNodeToPos(int pathnum, int nodenum, vector3 *pos);

// Gets next path from n that has actually been alloced
index_t GetNextPath(uint32_t n);
// Gets previous path from n that has actually been alloced
index_t GetPrevPath(uint32_t n);

// returns the index of the first path (from 0) alloced
// returns -1 if there are no paths
int GetFirstPath();

#endif
