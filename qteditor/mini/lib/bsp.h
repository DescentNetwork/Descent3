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

 * $Logfile: /DescentIII/Main/bsp.h $
 * $Revision: 13 $
 * $Date: 4/19/00 5:30p $
 * $Author: Matt $
 *
 * Header for bsp.cpp
 *
 * $Log: /DescentIII/Main/bsp.h $
 *
 * 13    4/19/00 5:30p Matt
 * From Duane for 1.4
 * Added extern
 *
 * 12    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 11    9/22/98 12:01p Matt
 * Added SourceSafe headers
 *
 */

#ifndef BSP_H
#define BSP_H

#include "list.h"
#include "vecmat.h"

// Results of classifying a polygon against a plane
enum class bsp_side : uint8_t {
  in_front = 1,
  behind = 2,
  on_plane = 3,
  spanning = 4,
  coincident = 5,
};

inline constexpr float BSP_EPSILON = 0.00005f;

// Values for bspnode::type
enum class bsp_node_type : uint8_t {
  node = 0,
  empty_leaf = 1,
  solid_leaf = 2,
};

struct bspplane {
  float a, b, c, d;
  uint8_t used;
};

struct bsppolygon {
  std::vector<vector3>& verts;
  int nv;
  bspplane plane;

  int16_t roomnum;
  int16_t facenum;
  int8_t subnum;

  int color;

};

struct bspnode {
  bsp_node_type type;
  bspplane plane;
  uint16_t node_facenum;
  uint16_t node_roomnum;
  int8_t node_subnum;

  bspnode *front;
  bspnode *back;

  listnode *polylist;
  int num_polys;
};

struct bsptree {
  listnode *vertlist;
  listnode *polylist;
  bspnode *root;
};

// Builds a bsp tree for the indoor rooms
void BuildBSPTree();

// Runs a ray through the bsp tree
// Returns true if a ray is occludes
bool BSPRayOccluded(vector3& start, vector3& end, bspnode *node);

// Walks the BSP tree and frees up any nodes/polygons that we might be using
void DestroyBSPTree(bsptree *tree);

// Saves and BSP node to an open file and recurses with the nodes children
void SaveBSPNode(struct CFILE* outfile, bspnode *node);

// Loads a bsp node from an open file and recurses with its children
void LoadBSPNode(struct CFILE* infile, bspnode **node);

// Initializes some variables for the indoor bsp tree
void InitDefaultBSP();

// Destroy indoor bsp tree
void DestroyDefaultBSPTree();

// Builds a bsp tree for a single room
void BuildSingleBSPTree(int roomnum);

// Reports the current mine's checksum
int BSPGetMineChecksum();

extern bsptree MineBSP;
extern int BSPChecksum;
extern bool BSP_initted;
extern bool UseBSP;

#endif
