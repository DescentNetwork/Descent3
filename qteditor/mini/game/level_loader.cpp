/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

// Decoupled level (.d3l) loader/saver for the Qt editor (qteditor/mini).
//
// This build links ONLY against the mini headers (cfile + its own lib/game
// headers); it does not link the full Descent3 engine.  LoadLevel/SaveLevel
// here are faithful, chunk-based ports of the engine's LoadLevel.cpp for the
// subset of chunks the editor needs to RENDER and round-trip:
//
//   PATH  - game paths (named navigation-path table)
//   TSND  - terrain sound bands (used bands: sound + altitude/volume range)
//   NLMP  - room/terrain lightmaps (textures + lightmap-info records)
//   PSTR  - player start position flags (passed through untouched)
//   TXNM  - skipped (no texture xlate table in the mini; raw indices survive)
//   ROOM  - room geometry (verts, faces, portals), Comp face normals after
//   RWND  - per-room wind vectors
//   TERR  - exterior terrain (heights, texmaps/flags, sky & lighting)
//   OBJS  - object placement data.  Each record is the engine's handle
//           (object number in the low bits) followed by the placement
//           prefix the mini keeps: type/id/name/flags/roomnum/pos/orient.
//   TRIG  - trigger table
//   OSND  - override (force-field/glass-breaking) sound names
//   FFTM  - force-field bounce texture multipliers (sets TF_FORCEFIELD)
//   INFO  - level name/designer/copyright/notes + level physics params
//   EDIT  - editor state (current/marked selection, wireframe view, lighting
//           globals); always the last chunk in a file
//
// All other chunks are skipped by seeking to chunk_start + chunk_size.
// SaveLevel writes those chunks in the same format so LoadLevel round-trips.

#include "level_loader.h"
#include "doorway.h"
#include "room.h"
#include "BOA.h"
#include "bsp.h"
#include "trigger.h"
#include "object.h"
#include "objinit.h"
#include "Mission.h"
#include "mem/mem.h"
#include "cfile.h"
#include "game.h"
#include "physics.h"
#include "terrain.h"
#include "findintersection.h"
#include "gametexture.h"
#include "gamepath.h"
#include "lightmap.h"
#include "lightmap_info.h"
#include "string_helpers.h"
#include "log.h"
#include "d3x_op.h"
#include "object_external_struct.h"
#include "objinfo.h"
#include "door.h"
#include "matcen.h"
#include "levelgoal.h"
#include "aiambient.h"
#include "player.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "d3edit.h"
#include "moveworld.h"

#include <QtGlobal>
#include <posix_stream.h>
#include <cstring>
#include <cstdio>
#include <stdexcept>

#include <algorithm>
#include <bit>
#include <vector>

constexpr uint32_t operator "" _ID(const char* const str, std::size_t len) {
  if (len != 4)
    throw "ID strings requires exactly 4 characters";
  return (static_cast<uint32_t>(str[3]) << 24) |
         (static_cast<uint32_t>(str[2]) << 16) |
         (static_cast<uint32_t>(str[1]) << 8)  |
         static_cast<uint32_t>(str[0]);
}

#define LL_TAG "D3LV"

// Editor-lighting globals defined in the Qt app layer (editor_lighting.cpp /
// rad_init.cpp); their headers can't be pulled into this TU (editor_lighting.h
// re-declares SaveLevel with a default argument, radiosity.h needs a <vector>
// include this file orders after its own headers).  Declared here so the EDIT
// chunk can round-trip them.
extern std::array<float, MAX_ROOMS + MAX_PALETTE_ROOMS> Room_multiplier;
extern std::array<float, MAX_ROOMS + MAX_PALETTE_ROOMS> Room_ambience_r, Room_ambience_g, Room_ambience_b;
extern int LightSpacing;
extern float GlobalMultiplier;
extern float Ambient_red, Ambient_green, Ambient_blue;
extern int rad_MaxStep;


static bool IsChunk(const char *chunk_name, const char *id) { return chunk_name[0] == id[0] && chunk_name[1] == id[1] && chunk_name[2] == id[2] && chunk_name[3] == id[3]; }

// A level's faces reference textures by an index into the level's own
// texture-name list (TXNM chunk).  texture_xlate[levelIdx] maps that index to
// the corresponding slot in the global GameTextures[] (matched by name).
// Reset to identity before each LoadLevel so the raw index survives when no
// TXNM list is present (e.g. our own saved files).
static std::array<int, MAX_TEXTURES> texture_xlate;

static std::optional<uint32_t> LL_FindTextureName(const std::string& name)
{
  for (uint32_t i = 0; i < static_cast<int>(GameTextures.size()); i++)
    if (match(GameTextures[i].name, name))
      return i;
  return std::nullopt;
}

static int LL_StartChunk(posix_ostream &ofile, const char *chunk_name);
static void LL_EndChunk(posix_ostream &ofile, int chunk_start_pos);

static void LL_ReadBOAChunk(posix_istream &ifile, uint32_t version)
{
  int i, j;
  int max_rooms;
  int max_path_portals;

  // Get the number of paths
  int32_t checksum_seed = 0;
  ifile >> checksum_seed;
  BOA_AABB_checksum = BOA_mine_checksum = static_cast<int>(checksum_seed);

  if (version >= 76)
    ifile >> BOA_vis_checksum;
  else
    BOA_vis_checksum = 0;

  ifile >> max_rooms;

  if (version < 62) {
    ifile.seek(sizeof(int16_t) * max_rooms * max_rooms, std::ios_base::cur);

    LOG_DEBUG("We will need to remake boa.  New cost structure added");
    BOA_AABB_checksum = BOA_mine_checksum = 0;
  } else {
    ifile >> max_path_portals;

    Q_ASSERT(max_rooms - 1 <= MAX_ROOMS + 8);

    if (version < 110 || (max_path_portals != MAX_PATH_PORTALS)) {
      ifile.seek(sizeof(int16_t) * max_rooms * max_rooms + max_rooms * max_path_portals * sizeof(float),
                 std::ios_base::cur);

      if (version >= 107) {
        // Read BOA terrain info (temporary, just so vis data works with multiplay)
        ifile.seek(max_rooms * sizeof(float), std::ios_base::cur);
      }

      LOG_DEBUG("We will need to remake boa.  Data size changed");
      BOA_AABB_checksum = BOA_mine_checksum = 0;
    } else {
      for (i = 0; i <= max_rooms; i++) {
        for (j = 0; j <= max_rooms; j++) {
          ifile >> BOA_Array[i][j];
        }
      }

      for (i = 0; i <= max_rooms; i++) {
        for (j = 0; j < max_path_portals; j++) {
          ifile >> BOA_cost_array[i][j];
        }
      }

      ifile >> BOA_num_mines;
      ifile >> BOA_num_terrain_regions;

      if (version < 112) {
        LOG_DEBUG("We will need to remake boa.");
        BOA_AABB_checksum = BOA_mine_checksum = 0;
      } else {
        for (i = 0; i < BOA_num_terrain_regions; i++) {
          ifile >> BOA_num_connect[i];

          for (j = 0; j < BOA_num_connect[i]; j++) {
            ifile >> BOA_connect[i][j].roomnum;
            ifile >> BOA_connect[i][j].portal;
          }
        }
      }
    }
  }
}


// Writes the CBOA (BOA) chunk.  Engine counterpart WriteBOAChunk
// (LoadLevel.cpp:4675).  mine/vis checksums, room count (incl. +8 for the path
// portals) and MAX_PATH_PORTALS, then the full BOA_Array grid of cost int16s,
// the BOA_cost_array of floats, then mine/terrain-region counts and the
// terrain-region connectivity list.  Written unconditionally in the current
// format (no version gates, like the engine writer).
static void LL_WriteBOAChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_BOA);
  int i, j;

  ofile << BOA_mine_checksum;
  ofile << BOA_vis_checksum;
  ofile << (((int)Rooms.size() - 1) + 8);
  ofile << static_cast<int>(MAX_PATH_PORTALS);

  for (i = 0; i < Rooms.size() + 8; i++) {
    for (j = 0; j < Rooms.size() + 8; j++) {
      ofile << static_cast<int16_t>(BOA_Array[i][j]);
    }
  }

  for (i = 0; i < Rooms.size() + 8; i++) {
    for (j = 0; j < MAX_PATH_PORTALS; j++) {
      ofile << BOA_cost_array[i][j];
    }
  }

  ofile << BOA_num_mines;
  ofile << BOA_num_terrain_regions;

  for (i = 0; i < BOA_num_terrain_regions; i++) {
    ofile << BOA_num_connect[i];

    for (j = 0; j < BOA_num_connect[i]; j++) {
      ofile << BOA_connect[i][j].roomnum;
      ofile << BOA_connect[i][j].portal;
    }
  }

  LL_EndChunk(ofile, start);
}

// Reads a TXNM chunk: an int32 count, then that many null-terminated texture
// names.  Builds texture_xlate[] so faces can map level-texture-index → global
// GameTextures index.
// Reads a NODE ("NODE") chunk: B-node / automatic path-point graph.
// Engine counterpart ReadBNodeChunk (LoadLevel.cpp:2991).
//   int16 hr_index (== ((int)Rooms.size() - 1) + 8)
//   per room slot i in [0 .. hr_index]:
//     byte    f_good_room
//     if f_good_room (BNode_GetBNListPtr(i, true) to materialize):
//       int16 num_nodes
//       if num_nodes, per node j:
//         vector3 pos  (3 floats)
//         int16  num_edges
//         if num_edges, per edge k:
//           int16  end_room
//           byte   end_index
//           if (version < 125) byte throwaway
//           int16  flags
//           int16  cost   (clamp: cost < 1 -> cost = 1)
//           float  max_rad
//   tail (version <= 123): BNode_verified = false
//   else: byte b; BNode_verified = (b != 0)
//   BNode_allocated = true
static void LL_ReadBNodeChunk(posix_istream &ifile, uint32_t version) {
  int16_t hr_index = 0;
  ifile >> hr_index;
  Q_ASSERT(hr_index == ((int)Rooms.size() - 1) + 8);

  for (int32_t i = 0; i <= hr_index; i++) {
    uint8_t f_good_room = 0;
    ifile >> f_good_room;
    if (f_good_room) {
      bn_list *bnlist = BNode_GetBNListPtr(i, true);

      int16_t num_nodes = 0;
      ifile >> num_nodes;
      if (num_nodes) {
        bnlist->nodes.resize(num_nodes);
        for (int32_t j = 0; j < num_nodes; j++) {
          bn_node &node = bnlist->nodes[j];
          ifile >> node.pos;

          int16_t num_edges = 0;
          ifile >> num_edges;
          if (num_edges) {
            node.edges.resize(num_edges);
            for (int32_t k = 0; k < num_edges; k++) {
              bn_edge &edge = node.edges[k];
              ifile >> edge.end_room;
              uint8_t eidx = 0;
              ifile >> eidx;
              edge.end_index = static_cast<char>(eidx);

              if (version < 125) {
                uint8_t bv = 0;
                ifile >> bv;
              }

              ifile >> edge.flags;
              ifile >> edge.cost;
              if (edge.cost < 1)
                edge.cost = 1;

              ifile >> edge.max_rad;
            }
          }
        }
      }
    }
  }

  if (version <= 123) {
    BNode_verified = false;
  } else {
    uint8_t bval = 0;
    ifile >> bval;
    BNode_verified = (bval != 0);
  }

  BNode_allocated = true;
}


// Writes a NODE (\"NODE\") chunk: automatic path-point (B-node) graph.
// Engine counterpart WriteBNodeChunk (Descent3/LoadLevel.cpp:4630).
//
// Layout (little-endian, posix stream) — mirror of LL_ReadBNodeChunk:
//   int16  hr_index == ((int)Rooms.size() - 1) + 8
//   per i in [0 .. hr_index]:
//     byte    f_good_room  (Rooms[i].used if i < Rooms.size(), else 1)
//     if f_good_room (uses BNode_GetBNListPtr(i, true)):
//       int16  num_nodes   (bnlist->nodes.size())
//       if num_nodes, per node j:
//         bn_node.pos   (whole vector3)
//         int16 num_edges  (node.edges.size())
//         if num_edges, per edge k:
//           int16 end_room
//           byte  end_index
//           int16 flags
//           int16 cost
//           float max_rad
//   byte BNode_verified ? 1 : 0
static void LL_WriteBNodeChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, "NODE");

  ofile << static_cast<int16_t>(((int)Rooms.size() - 1) + 8);

  for (int32_t i = 0; i < Rooms.size() + 8; i++) {
    if (i < Rooms.size() && !Rooms[i].used) {
      ofile << static_cast<uint8_t>(0);
    } else {
      ofile << static_cast<uint8_t>(1);
      bn_list *bnlist = BNode_GetBNListPtr(i, true);

      ofile << static_cast<int16_t>(bnlist->nodes.size());
      if (bnlist->nodes.size()) {
        for (int32_t j = 0; j < static_cast<int32_t>(bnlist->nodes.size()); j++) {
          bn_node &node = bnlist->nodes[j];
          ofile << node.pos;

          ofile << static_cast<int16_t>(node.edges.size());
          if (node.edges.size()) {
            for (int32_t k = 0; k < static_cast<int32_t>(node.edges.size()); k++) {
              bn_edge &edge = node.edges[k];
              ofile << edge.end_room;
              ofile << static_cast<uint8_t>(edge.end_index);
              ofile << edge.flags;
              ofile << edge.cost;
              ofile << edge.max_rad;
            }
          }
        }
      }
    }
  }

  ofile << static_cast<uint8_t>(BNode_verified ? 1 : 0);
  LL_EndChunk(ofile, start);
}

// Reads/writes the CNBS chunk: the mine's indoor BSP (collision/visibility)
// tree.  Engine counterpart InitDefaultBSP + LoadBSPNode / SaveBSPNode
// (Descent3/LoadLevel.cpp:3952-3956, bsp.cpp:189-242).  The chunk body is an
// int32 BSPChecksum followed by the root node in recursive pre-order.
//
//   uint8 type             BSP_NODE (0) / BSP_EMPTY_LEAF (1) / BSP_SOLID_LEAF (2)
//   leaves:                (no further data)
//   node:                  plane.a/b/c/d (float), node_roomnum (int16),
//                          node_facenum (int16), node_subnum (int8),
//                          front subtree, back subtree
//
// Loaded trees only carry node structure (never polygon lists), so the engine
// mem-style NewBSPNode/DestroyBSPNode reduce to new/delete here.

// BSP tree state for the CNBS chunk (extern-declared in mini/lib/bsp.h).  The
// tree object itself (MineBSP) is a game-table global defined in stubs.cpp;
// the serialization state lives with the loader that owns it.
int BSPChecksum = -1;
bool BSP_initted = false;

static bspnode *LL_NewBSPNode() {
  // Value-initialise: type -> BSP_NODE (0), plane/indices -> 0, front/back ->
  // nullptr, polylist -> nullptr (matches the engine's NewBSPNode zeroing).
  return new bspnode{};
}

static void LL_DestroyBSPNode(bspnode *node) {
  if (!node)
    return;
  if (node->type == BSP_NODE) {
    LL_DestroyBSPNode(node->front);
    LL_DestroyBSPNode(node->back);
  }
  delete node;
}

// Resets the mine BSP table to a fresh, empty state before reading a new
// tree (the engine's InitDefaultBSP without the atexit hook).
static void LL_DefaultBSPTree() {
  LL_DestroyBSPNode(MineBSP.root);
  MineBSP.root = nullptr;
  MineBSP.polylist = nullptr;
  MineBSP.vertlist = nullptr;
}

static void LL_ReadBSPNode(posix_istream &ifile, bspnode *&node_out) {
  uint8_t type = 0;
  ifile >> type;
  bspnode *node = LL_NewBSPNode();
  node->type = type;
  node_out = node;

  if (type == BSP_EMPTY_LEAF || type == BSP_SOLID_LEAF)
    return;

  ifile >> node->plane.a;
  ifile >> node->plane.b;
  ifile >> node->plane.c;
  ifile >> node->plane.d;

  int16_t roomnum = 0;
  ifile >> roomnum;
  node->node_roomnum = static_cast<uint16_t>(roomnum);

  int16_t facenum = 0;
  ifile >> facenum;
  node->node_facenum = static_cast<uint16_t>(facenum);

  ifile >> node->node_subnum;

  LL_ReadBSPNode(ifile, node->front);
  LL_ReadBSPNode(ifile, node->back);
}

static void LL_WriteBSPNode(posix_ostream &ofile, const bspnode *node) {
  ofile << node->type;

  if (node->type == BSP_EMPTY_LEAF || node->type == BSP_SOLID_LEAF)
    return;

  ofile << node->plane.a;
  ofile << node->plane.b;
  ofile << node->plane.c;
  ofile << node->plane.d;
  ofile << static_cast<int16_t>(node->node_roomnum);
  ofile << static_cast<int16_t>(node->node_facenum);
  ofile << node->node_subnum;

  LL_WriteBSPNode(ofile, node->front);
  LL_WriteBSPNode(ofile, node->back);
}

// Reads a CNBS chunk: clears any prior tree, stores the checksum, then loads
// the tree into MineBSP.root.  BSP_initted ends up true (tree present).
static void LL_ReadBSPChunk(posix_istream &ifile) {
  LL_DefaultBSPTree();

  ifile >> BSPChecksum;
  LL_ReadBSPNode(ifile, MineBSP.root);

  BSP_initted = true;
}

// Writes a CNBS chunk: checksum then the whole tree.  Caller decides whether
// the chunk is emitted (engine writes it only when BSP_initted).
static void LL_WriteBSPChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_NEW_BSP);

  ofile << BSPChecksum;
  LL_WriteBSPNode(ofile, MineBSP.root);

  LL_EndChunk(ofile, start);
}

static void LL_ReadTextureList(posix_istream &ifile, int chunk_size) {
  int32_t n32 = 0;
  ifile >> n32;
  int n = n32;
  long end = ifile.tell() + (chunk_size - 4);
  for (int i = 0; i < n; i++) {
    std::string name;
    ifile >> name;
    texture_xlate[i] = LL_FindTextureName(name).value_or(0);
    if (ifile.tell() >= end)
      break;
  }
}

// ---------------------------------------------------------------------------
// GNNM/DRNM name tables.
//
// A level names the object pages (GNNM) and door pages (DRNM) it references;
// those names let the loader map the file's page index to the index of the
// matching page in the currently loaded game tables.  This is the engine's
// BuildXlateTable + ReadObject translation: OBJ_ROBOT/POWERUP/BUILDING/CLUTTER
// ids go through generic_xlate[MAX_OBJECT_IDS], OBJ_DOOR through
// door_xlate[MAX_DOORS].  Reset to -1 (no mapping) before every LoadLevel so
// a level without the name chunks keeps the raw page indices it was saved
// with.
static std::array<int16_t, MAX_OBJECT_IDS> generic_xlate;

static std::array<int16_t, MAX_DOORS> door_xlate;
// Reads a GNNM/DRNM chunk body: an int32 count, then that many null-terminated
// page names.  Each name is resolved through lookup(); an empty name (an
// unused slot) is never looked up and maps to -1, exactly as in the engine's
// BuildXlateTable.  The trailing entries up to max_items are cleared to -1 so
// a partially filled table never leaks indices from a previous level.
static void LL_ReadNameXlateChunk(posix_istream &ifile, int chunk_size,
                                  std::optional<uint32_t> (*lookup)(const std::string &), int16_t *xlate, int max_items) {
  int32_t n32 = 0;
  ifile >> n32;
  int n = n32;
  if (n < 0 || n > max_items)
    n = max_items;
  long end = ifile.tell() + (chunk_size - 4);
  for (int i = 0; i < n; i++) {
    std::string name;
    ifile >> name;
    if (!name.empty())
      xlate[i] = lookup(name).value_or(-1);
    else
      xlate[i] = -1;
    if (ifile.tell() >= end)
      break;
  }
  for (int i = n; i < max_items; i++)
    xlate[i] = -1;
}

// First used page of the given type, the engine's FindValidID() fallback for
// a name-mapping miss (GetObjectID for the generic types, the first used door
// slot for OBJ_DOOR).  Returns std::nullopt when no game table provides one.
static std::optional<uint32_t> FindValidID(int type) {
  switch (type) {
  case OBJ_ROBOT:
  case OBJ_POWERUP:
  case OBJ_BUILDING:
  case OBJ_CLUTTER:
    return GetObjectID(type);
  case OBJ_DOOR:
    for (int i = 0; i < MAX_DOORS; i++)
      if (Doors[i].used)
        return i;
    return std::nullopt;
  default:
    return std::nullopt;
  }
}

// Applies the GNNM/DRNM id translation the engine performs in ReadObject() for
// the generic object types and doors.  On a lookup miss the engine picks the
// first valid page of the object's type; when no game table is loaded the raw
// file id is kept so the level still renders.
static int TranslateObjectId(int type, int id) {
  int xid = -1;
  if (type == OBJ_ROBOT || type == OBJ_POWERUP || type == OBJ_BUILDING || type == OBJ_CLUTTER)
    xid = (id < MAX_OBJECT_IDS) ? static_cast<int>(generic_xlate[id]) : -1;
  else if (type == OBJ_DOOR)
    xid = (id < MAX_DOORS) ? static_cast<int>(door_xlate[id]) : -1;
  else
    return id;

  if (xid != -1)
    return xid;

  const std::optional<uint32_t> valid = FindValidID(type);
  return valid.value_or(id)
}

// Writes a chunk header (4-char name + size placeholder), returns the position
// of the size field so EndChunk can seek back and patch it.
static int LL_StartChunk(posix_ostream &ofile, const char *chunk_name) {
  ofile.write(chunk_name, 4);
  int chunk_start_pos = static_cast<int>(ofile.tell());
  int32_t placeholder = 0;
  ofile << placeholder;
  return chunk_start_pos;
}

static void LL_EndChunk(posix_ostream &ofile, int chunk_start_pos) {
  int save_pos = static_cast<int>(ofile.tell());
  int len = save_pos - chunk_start_pos;
  while (len & 3) { // pad to multiple of four
    ofile.put(0);
    len++;
    save_pos++;
  }
  ofile.seek(chunk_start_pos, std::ios_base::beg);
  int32_t l = len;
  ofile << l;
  ofile.seek(save_pos, std::ios_base::beg);
}

// RLE byte compression used by the engine inside ROOM for volume lights.

static int LL_ReadRoom(posix_istream &ifile, room *rp, uint32_t /*version*/) {
  // Current canonical layout: the room stream operator reads every field
  // (verts, faces, portals, lights, ...) exactly as the engine writes it.
  ifile >> *rp;

  // Map raw level-local texture indices into global GameTextures[] slots.
  for (int f = 0; f < rp->num_faces; f++) {
    const int raw = rp->faces[f].tmap;
    rp->faces[f].tmap = (raw >= 0 && raw < MAX_TEXTURES) ? texture_xlate[raw] : 0;
  }

  return 1;
}

// AABB (room bounding boxes).  Per-face min/max extents (FVI), per-room
// bbf_min/max_xyz, and the BSP back-face (BBF) region lists used by BOA /
// visibility queries.  The engine's ReadRoomAABBChunk / WriteRoomAABBChunk
// store these per room; the mini keeps the arrays as vectors instead of the
// engine's malloc'd pointers.
static void LL_ReadRoomAABBChunk(posix_istream &ifile) {
  int32_t save_hri = 0;
  ifile >> save_hri;
  if (save_hri < 0 || save_hri >= MAX_ROOMS)
    save_hri = ((int)Rooms.size() - 1);

  for (int i = 0; i <= save_hri; i++)
    ifile >> BOA_AABB_ROOM_checksum[i];

  for (int i = 0; i < Rooms.size(); i++) {
    int32_t used = 0;
    ifile >> used;
    Q_ASSERT(Rooms[i].used == used);
    if (!used) {
      BOA_AABB_ROOM_checksum[i] = 0; // Not used
      continue;
    }

    int32_t n_faces = 0;
    ifile >> n_faces;
    Q_ASSERT(Rooms[i].num_faces == n_faces);
    for (int j = 0; j < Rooms[i].num_faces; j++) {
      ifile >> Rooms[i].faces[j].min_xyz;
      ifile >> Rooms[i].faces[j].max_xyz;
    }

    ifile >> BOA_AABB_ROOM_checksum[i];
    ifile >> Rooms[i].bbf_min_xyz;
    ifile >> Rooms[i].bbf_max_xyz;

    ifile >> Rooms[i].num_bbf_regions;
    int nregions = Rooms[i].num_bbf_regions;
    if (nregions < 0 || nregions > 200)
      nregions = 0; // sanity cap, mirrors the engine's MAX_REGIONS_PER_ROOM
    Rooms[i].num_bbf_regions = static_cast<int16_t>(nregions);

    Rooms[i].num_bbf.assign(nregions, 0);
    Rooms[i].bbf_list.assign(nregions, std::vector<int16_t>());
    Rooms[i].bbf_list_min_xyz.resize(nregions);
    Rooms[i].bbf_list_max_xyz.resize(nregions);
    Rooms[i].bbf_list_sector.resize(nregions);

    for (int j = 0; j < nregions; j++)
      ifile >> Rooms[i].num_bbf[j];

    for (int j = 0; j < nregions; j++) {
      const int nfaces = Rooms[i].num_bbf[j];
      if (nfaces < 0 || nfaces > Rooms[i].num_faces)
        continue;
      Rooms[i].bbf_list[j].resize(nfaces);
      for (int k = 0; k < nfaces; k++)
        ifile >> Rooms[i].bbf_list[j][k];
      ifile >> Rooms[i].bbf_list_min_xyz[j];
      ifile >> Rooms[i].bbf_list_max_xyz[j];
      ifile >> Rooms[i].bbf_list_sector[j];
    }
  }
}

static void LL_WriteRoomAABBChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_ROOM_AABB);

  ofile << (int32_t)((int)Rooms.size() - 1);
  for (int i = 0; i < Rooms.size(); i++)
    ofile << BOA_AABB_ROOM_checksum[i];

  for (int i = 0; i < Rooms.size(); i++) {
    if (!Rooms[i].used) {
      ofile << (int32_t)0; // Not used
      continue;
    }

    ofile << (int32_t)1; // used
    ofile << (int32_t)Rooms[i].num_faces;
    for (int j = 0; j < Rooms[i].num_faces; j++) {
      ofile << Rooms[i].faces[j].min_xyz;
      ofile << Rooms[i].faces[j].max_xyz;
    }

    ofile << BOA_AABB_ROOM_checksum[i];
    ofile << Rooms[i].bbf_min_xyz;
    ofile << Rooms[i].bbf_max_xyz;
    ofile << Rooms[i].num_bbf_regions;

    const int nregions = Rooms[i].num_bbf_regions;
    for (int j = 0; j < nregions; j++)
      ofile << (int16_t)Rooms[i].bbf_list[j].size();

    for (int j = 0; j < nregions; j++) {
      for (int k = 0; k < (int)Rooms[i].bbf_list[j].size(); k++)
        ofile << Rooms[i].bbf_list[j][k];
      ofile << Rooms[i].bbf_list_min_xyz[j];
      ofile << Rooms[i].bbf_list_max_xyz[j];
      ofile << Rooms[i].bbf_list_sector[j];
    }
  }

  LL_EndChunk(ofile, start);
}

// ---------------------------------------------------------------------------
// MTCN (matcen data).  matcen::LoadData/SaveData (mini/editor/matcen.cpp) do
// the per-record I/O; these helpers frame the table.  The engine reads the
// count then instantiates each entry; the in-memory table is rebuilt fresh to
// avoid leaking entries from a previously loaded level.
static void LL_ReadMatcenChunk(posix_istream &ifile) {
  int32_t count = 0;
  ifile >> count;

  DestroyAllMatcens();

  Num_matcens = (count < 0) ? 0 : count;
  if (Num_matcens > MAX_MATCENS) {
    // Corrupt count: only load what the fixed-size table can hold; the chunk
    // framer skips the remaining body bytes.
    Num_matcens = MAX_MATCENS;
  }

  for (int i = 0; i < Num_matcens; i++) {
    matcen *mp = new matcen;
    mp->LoadData(ifile, texture_xlate.data());
    Matcen[i] = mp;
  }
}

static void LL_WriteMatcenChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_MATCEN_DATA);

  ofile << static_cast<int32_t>(Num_matcens);

  for (int i = 0; i < Num_matcens; i++) {
    Q_ASSERT(Matcen[i]);
    Matcen[i]->SaveData(ofile);
  }

  LL_EndChunk(ofile, start);
}

// TSND (terrain sound bands).  Reader/writer (LoadLevel.cpp:3957 across,
// WriteLevel :5125).  Each band stores a sound (translated through the loaded
// sound table), a low/high altitude pair and a low/high volume pair.  The
// writer emits only the used bands (sound_index != -1), so the reader clears
// the table first and the reload re-emits the same used set byte-stably.
static void LL_ReadTerrainSoundChunk(posix_istream &ifile, uint32_t version) {
  int32_t n_bands = 0;
  ifile >> n_bands;
  if (n_bands < 0)
    n_bands = 0;
  if (n_bands > NUM_TERRAIN_SOUND_BANDS)
    n_bands = NUM_TERRAIN_SOUND_BANDS;

  ClearTerrainSound();

  for (int b = 0; b < n_bands; b++) {
    terrain_sound_band &band = Terrain_sound_bands[b];

    if (version < 119) {
      ifile >> band.sound_index;
    } else {
      std::string soundname;
      ifile >> soundname; // NUL-terminated, consuming the whole field
      band.sound_index = FindSoundName(soundname).value_or(-1);
    }

    int8_t low_alt = 0, high_alt = 0;
    ifile >> low_alt;
    ifile >> high_alt;
    band.low_alt = static_cast<uint8_t>(low_alt);
    band.high_alt = static_cast<uint8_t>(high_alt);

    ifile >> band.low_volume;
    ifile >> band.high_volume;
  }
}

static void LL_WriteTerrainSoundChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_TERRAIN_SOUND);

  int32_t n_bands = 0;
  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++)
    if (Terrain_sound_bands[b].sound_index != -1)
      n_bands++;
  ofile << n_bands;

  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++) {
    if (Terrain_sound_bands[b].sound_index == -1)
      continue;

    const terrain_sound_band &band = Terrain_sound_bands[b];
    ofile << Sounds[band.sound_index].name; // NUL-terminated, like cf_WriteString
    ofile << static_cast<int8_t>(band.low_alt);
    ofile << static_cast<int8_t>(band.high_alt);
    ofile << band.low_volume;
    ofile << band.high_volume;
  }

  LL_EndChunk(ofile, start);
}

// Clears all terrain sound bands (engine GameLoop.cpp:2839).
void ClearTerrainSound() {
  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++) {
    Terrain_sound_bands[b].sound_index = -1;
    Terrain_sound_bands[b].low_alt = 0;
    Terrain_sound_bands[b].high_alt = 0;
    Terrain_sound_bands[b].low_volume = 0.0f;
    Terrain_sound_bands[b].high_volume = 0.0f;
  }
}

// PSTR (player starts).  Reader/writer (LoadLevel.cpp:3499 / :5062).  A short
// player count (>= 120; legacy files used a fixed 32) then that many start
// position flag words.  The editor passes these through untouched.
static void LL_ReadPlayerStartsChunk(posix_istream &ifile, uint32_t version) {
  int n = MAX_PLAYERS;
  if (version >= 120) {
    int16_t n16 = 0;
    ifile >> n16;
    n = n16;
  }
  if (n < 0)
    n = 0;
  if (n > MAX_PLAYERS)
    n = MAX_PLAYERS;

  for (int i = 0; i < n; i++)
    ifile >> Players[i].startpos_flags;
}

static void LL_WritePlayerStartsChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_PLAYER_STARTS);

  ofile << static_cast<int16_t>(MAX_PLAYERS);

  for (int i = 0; i < (int)Players.size(); i++)
    ofile << Players[i].startpos_flags;

  LL_EndChunk(ofile, start);
}

// OSND (override sounds) / FFTM (force-field bounce texture multipliers).
// Reader/writer pairs (LoadLevel.cpp:2956/:2970, writers :4593/:4612).  The
// engine's NEWEDITOR build only *reads* these (for v1.2 compatibility it does
// not re-write them); the mini writes them so an engine-made level with such
// chunks round-trips losslessly.
static void LL_ReadOverrideSoundChunk(posix_istream &ifile) {
  sound_override_force_field.reset();
  sound_override_glass_breaking.reset();

  std::string soundname;
  ifile >> soundname;
  if (!soundname.empty())
    sound_override_force_field = FindSoundName(soundname);

  ifile >> soundname;
  if (!soundname.empty())
    sound_override_glass_breaking = FindSoundName(soundname);
}

static void LL_WriteOverrideSoundChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_OVERRIDE_SOUNDS);

  if(sound_override_force_field)
    ofile << Sounds[*sound_override_force_field].name;
  else
    ofile << std::string();

  if(sound_override_glass_breaking)
    ofile << Sounds[*sound_override_glass_breaking].name;
  else
    ofile << std::string();

  LL_EndChunk(ofile, start);
}

static void LL_ReadFFTMChunk(posix_istream &ifile, uint32_t version) {  
  std::ranges::fill(force_field_bounce, std::nullopt);

  size_t num_items = MAX_FORCE_FIELD_BOUNCE_TEXTURES;
  if (version < 132)
    num_items = 2;

  for (size_t i = 0; i < num_items; i++)
  {
    auto& bounce = force_field_bounce[i];
    std::string texturename;
    ifile >> texturename;
    if(!texturename.empty())
      ifile >> bounce->multiplier;

    if(auto idx = FindTextureName(texturename); idx)
    {
      bounce->texture = *idx;
      if(*idx < MAX_TEXTURES)
        GameTextures[*idx].flags.forcefield = true;
    }
  }
}

static void LL_WriteFFTMChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_FFT_MOD);

  for(auto& bounce : force_field_bounce)
  {
    if (bounce)
    {
      ofile << GameTextures[bounce->texture].name;
      ofile << bounce->multiplier;
    }
    else
    {
      ofile << std::string();
    }
  }

  LL_EndChunk(ofile, start);
}

// EDIT (editor state: current/marked room & selection, wireframe view,
// per-room multipliers/ambience and the lighting globals).  Engine reader
// inline (:4026-4089, #ifdef EDITOR), writer (:5313, always the LAST chunk).
static void LL_ReadEditorInfoChunk(posix_istream &ifile, uint32_t version) {
  auto lookup_room = [](int16_t idx) -> room * {
    if (idx >= 0 && idx < Rooms.size() && Rooms[idx].used)
      return &Rooms[idx];
    return nullptr;
  };

  int16_t room_idx = 0;
  ifile >> room_idx;
  Curroomp = lookup_room(room_idx);
  int16_t sel = 0;
  ifile >> sel;
  Curface = sel;
  if (version >= 81) {
    ifile >> sel;
    Curedge = sel;
    ifile >> sel;
    Curvert = sel;
  }

  ifile >> room_idx;
  Markedroomp = lookup_room(room_idx);
  ifile >> sel;
  Markedface = sel;
  if (version >= 81) {
    ifile >> sel;
    Markededge = sel;
    ifile >> sel;
    Markedvert = sel;
  }

  int32_t nsr = 0;
  ifile >> nsr;
  N_selected_rooms = std::min<int32_t>(nsr, MAX_ROOMS);
  for (int i = 0; i < N_selected_rooms; i++) {
    ifile >> sel;
    Selected_rooms[i] = sel;
  }

  if (version >= 14) {
    ifile >> Cur_object_index;
    ifile >> Current_trigger;
    int32_t tmp = 0;
    if (version < 106)
      ifile >> tmp; // was Current_doorway
    ifile >> tmp;
    if (tmp >= static_cast<int>(state::viewer::mine) && tmp <= static_cast<int>(state::viewer::room))
      app.view_mode = static_cast<state::viewer>(tmp);
    ifile >> Editor_viewer_id;
    if (version < 47)
      ifile >> tmp; // was Editor_viewer_id[VM_TERRAIN]
  }

  if (version >= 55) {
    ifile >> Wireframe_view_mine.target;
    ifile >> Wireframe_view_mine.orient;
    ifile >> Wireframe_view_mine.dist;
  }

  if (version >= 113) {
    for (int i = 0; i < MAX_ROOMS; i++) {
      ifile >> Room_multiplier[i];
      if (version >= 118) {
        ifile >> Room_ambience_r[i];
        ifile >> Room_ambience_g[i];
        ifile >> Room_ambience_b[i];
      }
    }
  }
  if (version >= 126)
    ifile >> LightSpacing;

  if (version >= 128) {
    ifile >> GlobalMultiplier;
    ifile >> Ambient_red;
    ifile >> Ambient_green;
    ifile >> Ambient_blue;
    ifile >> rad_MaxStep;
  }
}

static void LL_WriteEditorInfoChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, CHUNK_EDITOR_INFO);

  ofile << static_cast<int16_t>(Curroomp ? ROOMNUM(Curroomp) : -1);
  ofile << static_cast<int16_t>(Curface);
  ofile << static_cast<int16_t>(Curedge);
  ofile << static_cast<int16_t>(Curvert);
  ofile << static_cast<int16_t>(Markedroomp ? ROOMNUM(Markedroomp) : -1);
  ofile << static_cast<int16_t>(Markedface);
  ofile << static_cast<int16_t>(Markededge);
  ofile << static_cast<int16_t>(Markedvert);

  ofile << static_cast<int32_t>(N_selected_rooms);
  for (int i = 0; i < N_selected_rooms; i++)
    ofile << static_cast<int16_t>(Selected_rooms[i]);

  ofile << static_cast<int32_t>(Cur_object_index);
  ofile << static_cast<int32_t>(Current_trigger);
  ofile << static_cast<int32_t>(app.view_mode);
  ofile << static_cast<int32_t>(Editor_viewer_id);

  ofile << Wireframe_view_mine.target;
  ofile << Wireframe_view_mine.orient;
  ofile << Wireframe_view_mine.dist;

  for (int i = 0; i < MAX_ROOMS; i++) {
    ofile << Room_multiplier[i];
    ofile << Room_ambience_r[i];
    ofile << Room_ambience_g[i];
    ofile << Room_ambience_b[i];
  }

  ofile << static_cast<int32_t>(LightSpacing);
  ofile << GlobalMultiplier;
  ofile << Ambient_red;
  ofile << Ambient_green;
  ofile << Ambient_blue;
  ofile << static_cast<int32_t>(rad_MaxStep);

  LL_EndChunk(ofile, start);
}

static void LL_ReadInfo(posix_istream &ifile, int) {
  Level_info = level_info{};
  ifile >> Level_info;

  ifile >> Gravity_strength;

  int32_t v = 0;
  ifile >> v;
  Level_powerups_ignore_wind = (v != 0);

  ifile >> v;
  FVI_always_check_ceiling = (v != 0);

  ifile >> Ceiling_height;
}

static void LL_WriteInfo(posix_ostream &ofile) {
  ofile << Level_info;

  ofile << Gravity_strength;
  int32_t v = (int)Level_powerups_ignore_wind;
  ofile << v;
  v = (int)FVI_always_check_ceiling;
  ofile << v;
  ofile << Ceiling_height;
}

// ---------------------------------------------------------------------------
// Game paths (PATH) chunk: the level's named navigation-path table.
static void LL_ReadGamePathsChunk(posix_istream &ifile, uint32_t version) {
  int16_t np = 0;
  ifile >> np;
  Num_game_paths = np;
  if (Num_game_paths < 0 || Num_game_paths > MAX_GAME_PATHS) {
    // Corrupt count: bail out before indexing GamePaths[] out of range; the
    // chunk framer then skips the remaining body bytes.
    Num_game_paths = 0;
    return;
  }

  for (int i = 0; i < Num_game_paths; i++) {
    game_path &p = GamePaths[i];
    // Value-initialise (NOT memset: game_path contains std::string members).
    p = game_path{};
    p.used = true;

    // Null-terminated path name, then the node count and path flags.
    ifile >> p.name;
    int32_t nnodes = 0;
    ifile >> nnodes;
    p.num_nodes = std::max(0, std::min<int>((int)nnodes, MAX_NODES_PER_PATH));
    ifile >> p.flags;

    p.pathnodes.resize(p.num_nodes);
    for (int j = 0; j < p.num_nodes; j++) {
      node &nd = p.pathnodes[j];
      ifile >> nd.pos;
      ifile >> nd.roomnum;
      ifile >> nd.flags;
      if (version >= 51) {
        ifile >> nd.fvec;
        ifile >> nd.uvec;
      } else {
        nd.fvec = vector3::id(2);
        nd.uvec = vector3::id(1);
      }
    }
  }
}

static void LL_WriteGamePathsChunk(posix_ostream &ofile) {
  int npaths = 0;
  for (int i = 0; i < (int)GamePaths.size(); i++)
    if (GamePaths[i].used)
      npaths++;

  int start = LL_StartChunk(ofile, "PATH");
  ofile << (int16_t)npaths;
  for (int i = 0; i < (int)GamePaths.size(); i++) {
    const game_path &p = GamePaths[i];
    if (!p.used)
      continue;

    ofile << p.name;
    ofile << (int32_t)p.num_nodes;
    ofile << p.flags;

    for (int j = 0; j < p.num_nodes; j++) {
      const node &nd = p.pathnodes[j];
      ofile << nd.pos;
      ofile << nd.roomnum;
      ofile << nd.flags;
      ofile << nd.fvec;
      ofile << nd.uvec;
    }
  }
  LL_EndChunk(ofile, start);
}

// ---------------------------------------------------------------------------
// Lightmaps (NLMP) chunk: the room/terrain lightmap table.  Two sections: a
// list of unique raw lightmap textures (each RLE-compressed as uint16_t), then
// the lightmap-info records that reference a texture by ordinal and carry the
// mapping geometry (spacing, upper-left corner, normal).  Faces in the ROOM
// and OBJS chunks reference the info *ordinals*; because the mini allocates
// infos sequentially in file order, ordinals and store slots coincide, so
// faces round-trip unchanged.

// The engine emits whichever of raw/RLE is smaller: command bytes (0 = single
// raw value, 2..250 = run of copies) each cost one command byte + a uint16_t.
static int LL_CountCompressShort(const uint16_t *val, int total) {
  int curptr = 0;
  int written = 0;
  while (curptr < total) {
    const int count = [&] {
      uint16_t curval = val[curptr];
      int c = 1;
      while ((curptr + c) < total && val[curptr + c] == curval && c < 250)
        c++;
      return c;
    }();
    written += 3;
    curptr += count;
  }
  return written;
}

static void LL_WriteCompressShort(posix_ostream &ofile, const uint16_t *val, int total, bool compressed) {
  if (!compressed) {
    ofile.put(0); // NO_COMPRESS
    for (int i = 0; i < total; i++)
      ofile << val[i];
    return;
  }

  ofile.put(1); // COMPRESS
  int curptr = 0;
  while (curptr < total) {
    const uint16_t curval = val[curptr];
    int count = 1;
    while ((curptr + count) < total && val[curptr + count] == curval && count < 250)
      count++;
    if (count == 1) {
      ofile.put(0);
      ofile << curval;
    } else {
      ofile.put(static_cast<uint8_t>(count));
      ofile << curval;
    }
    curptr += count;
  }
}

static void LL_CheckToWriteCompressShort(posix_ostream &ofile, const uint16_t *vals, int total) {
  if (total <= 0)
    return;
  if (LL_CountCompressShort(vals, total) >= total)
    LL_WriteCompressShort(ofile, vals, total, false);
  else
    LL_WriteCompressShort(ofile, vals, total, true);
}

static void LL_ReadCompressedShortArray(posix_istream &ifile, uint16_t *vals, int total) {
  uint8_t compressed = 0;
  ifile >> compressed;
  if (compressed == 0) {
    for (int i = 0; i < total; i++)
      ifile >> vals[i];
    return;
  }
  int count = 0;
  while (count != total) {
    uint8_t command = 0;
    ifile >> command;
    if (command == 0) {
      ifile >> vals[count];
      count++;
    } else if (command >= 2 && command <= 250) {
      uint16_t value = 0;
      ifile >> value;
      for (int k = 0; k < command && count < total; k++) {
        vals[count] = value;
        count++;
      }
    } else {
      break; // corrupt RLE stream
    }
  }
}

static void LL_ReadNewLightmapChunk(posix_istream &ifile, uint32_t version) {
  Num_of_lightmap_info = 0;
  int32_t nummaps = 0;
  ifile >> nummaps;
  if (nummaps < 0 || nummaps > static_cast<int32_t>(MAX_LIGHTMAPS))
    nummaps = 0;
  const int num_raw = nummaps;

  // ordinal -> GameLightmaps handle for the raw-texture section.
  std::vector<uint16_t> lightmap_remap(static_cast<size_t>(std::max(0, num_raw)));
  for (int i = 0; i < num_raw; i++) {
    int16_t map_w = 0, map_h = 0;
    ifile >> map_w;
    ifile >> map_h;
    if (map_w < 2 || map_h < 2)
      map_w = map_h = 2;
    int lm_handle = static_cast<int>(lm_AllocLightmap(map_w, map_h).value_or(BAD_LM_INDEX));
    if (lm_handle == BAD_LM_INDEX)
      lm_handle = 0;
    lightmap_remap[i] = static_cast<uint16_t>(lm_handle);
    std::vector<uint16_t> flat(static_cast<size_t>(map_w) * map_h);
    LL_ReadCompressedShortArray(ifile, flat.data(), static_cast<int>(flat.size()));
    std::vector<std::vector<uint16_t>> &data = lm_data(lm_handle);
    for (int y = 0; y < map_h; y++)
      std::copy_n(flat.begin() + y * map_w, map_w, data[y].begin());
  }

  int32_t ninfos = 0;
  ifile >> ninfos;
  if (ninfos < 0 || ninfos > static_cast<int32_t>(MAX_LIGHTMAP_INFOS))
    ninfos = 0;
  Num_lightmap_infos_read = ninfos;

  for (int i = 0; i < ninfos; i++) {
    int16_t remap_handle = 0, w = 0, h = 0;
    ifile >> remap_handle;
    ifile >> w;
    ifile >> h;
    uint8_t type = 0;
    ifile >> type;

    int lmi = static_cast<int>(AllocLightmapInfo(w, h, type, false).value_or(BAD_LMI_INDEX));
    if (lmi == BAD_LMI_INDEX)
      continue;
    const size_t remap_idx = (remap_handle >= 0 && remap_handle < (int32_t)num_raw)
                                 ? static_cast<size_t>(remap_handle)
                                 : 0;
    LightmapInfo[lmi].lm_handle = lightmap_remap[remap_idx];

    if (version >= 91) {
      int16_t x1 = 0, y1 = 0;
      ifile >> x1;
      ifile >> y1;
      LightmapInfo[lmi].x1 = static_cast<uint8_t>(x1);
      LightmapInfo[lmi].y1 = static_cast<uint8_t>(y1);
    }

    uint8_t xspacing = 0, yspacing = 0;
    ifile >> xspacing;
    ifile >> yspacing;
    LightmapInfo[lmi].xspacing = xspacing;
    LightmapInfo[lmi].yspacing = yspacing;
    ifile >> LightmapInfo[lmi].upper_left;
    ifile >> LightmapInfo[lmi].normal;
  }
}

static void LL_WriteLightmapChunk(posix_ostream &ofile) {
  // Build the lm_handle -> ordinal remap and count infos, exactly as the
  // engine's WriteLightmapChunk does (dynamic infos are excluded).
  const int MAXLMS = MAX_LIGHTMAPS;
  const int MAXINFOS = MAX_LIGHTMAP_INFOS;
  std::vector<uint16_t> lightmap_remap(MAXLMS, 0);
  std::vector<uint8_t> lightmap_spoken_for(MAXLMS, 0);
  int lightmap_count = 0;
  int lightmap_info_count = 0;

  for (int i = 0; i < MAXINFOS; i++) {
    if (LightmapInfo[i].used && LightmapInfo[i].type != LMI_DYNAMIC) {
      const uint16_t lm_handle = LightmapInfo[i].lm_handle;
      if (lm_handle < MAXLMS && !lightmap_spoken_for[lm_handle]) {
        lightmap_spoken_for[lm_handle] = 1;
        lightmap_remap[lm_handle] = static_cast<uint16_t>(lightmap_count);
        lightmap_count++;
      }
      lightmap_info_count++;
    }
  }
  std::fill(lightmap_spoken_for.begin(), lightmap_spoken_for.end(), 0);

  int start = LL_StartChunk(ofile, "NLMP");

  ofile << (int32_t)lightmap_count;
  for (int i = 0; i < MAXINFOS; i++) {
    if (LightmapInfo[i].used && LightmapInfo[i].type != LMI_DYNAMIC) {
      const uint16_t lm_handle = LightmapInfo[i].lm_handle;
      if (lm_handle < MAXLMS && !lightmap_spoken_for[lm_handle]) {
        lightmap_spoken_for[lm_handle] = 1;
        const int map_w = static_cast<int>(lm_w(lm_handle).value_or(255));
        const int map_h = static_cast<int>(lm_h(lm_handle).value_or(255));
        ofile << (int16_t)map_w;
        ofile << (int16_t)map_h;
        const std::vector<std::vector<uint16_t>> &data = lm_data(lm_handle);
        std::vector<uint16_t> flat(static_cast<size_t>(map_w) * map_h);
        for (int y = 0; y < map_h; y++)
          std::copy(data[y].begin(), data[y].end(), flat.begin() + y * map_w);
        LL_CheckToWriteCompressShort(ofile, flat.data(), map_w * map_h);
      }
    }
  }

  ofile << (int32_t)lightmap_info_count;
  for (int i = 0; i < MAXINFOS; i++) {
    if (LightmapInfo[i].used && LightmapInfo[i].type != LMI_DYNAMIC) {
      const lightmap_info &info = LightmapInfo[i];
      ofile << (int16_t)lightmap_remap[info.lm_handle];
      ofile << (int16_t)lmi_w(i).value_or(0);
      ofile << (int16_t)lmi_h(i).value_or(0);
      ofile << info.type;
      ofile << (int16_t)info.x1;
      ofile << (int16_t)info.y1;
      ofile << info.xspacing;
      ofile << info.yspacing;
      ofile << info.upper_left;
      ofile << info.normal;
    }
  }

  LL_EndChunk(ofile, start);
}

// ---------------------------------------------------------------------------
// Terrain (TERR) chunk: heights, texmaps/flags and sky/lighting.  The engine
// stores these as sub-chunks (TERH/TETM/TSKY) inside one TERR container, which
// ends with a TEND terminator.  Only the version >= 127 layouts are live:
// LoadLevel() rejects anything older, so the engine's ancient version gates
// (31/41/56/69/72/74/75/87/88/102/104/114/116) all collapse onto this shape.

// RLE byte array format shared with the engine (cf. room.cpp volume lights): a
// leading flag byte (0 = raw, 1 = run-length encoded) followed by either
// `total` raw values or command bytes (0 = single raw value follows, 2..250 =
// run of that many copies of the next byte).
static void LL_ReadCompressedByte(posix_istream &ifile, uint8_t *vals, int total) {
  uint8_t compressed = 0;
  ifile >> compressed;
  int count = 0;
  if (compressed == 0) {
    for (int i = 0; i < total; i++)
      ifile >> vals[i];
    return;
  }
  while (count != total) {
    uint8_t command = 0;
    ifile >> command;
    if (command == 0) {
      ifile >> vals[count];
      count++;
    } else if (command >= 2 && command <= 250) {
      uint8_t value = 0;
      ifile >> value;
      for (int k = 0; k < command && count < total; k++) {
        vals[count] = value;
        count++;
      }
    } else {
      break; // corrupt RLE stream
    }
  }
}

// Same scheme for uint16_t values (terrain texmap indices).
static void LL_ReadCompressedShort(posix_istream &ifile, uint16_t *vals, int total) {
  uint8_t compressed = 0;
  ifile >> compressed;
  int count = 0;
  if (compressed == 0) {
    for (int i = 0; i < total; i++)
      ifile >> vals[i];
    return;
  }
  while (count != total) {
    uint8_t command = 0;
    ifile >> command;
    if (command == 0) {
      ifile >> vals[count];
      count++;
    } else if (command >= 2 && command <= 250) {
      uint16_t value = 0;
      ifile >> value;
      for (int k = 0; k < command && count < total; k++) {
        vals[count] = value;
        count++;
      }
    } else {
      break; // corrupt RLE stream
    }
  }
}

// The engine emits whichever of raw/RLE is smaller; the mini (like the room
// volume lights) always writes raw (flag byte 0), which every reader -- ours
// and the engine's -- accepts.
static void LL_WriteCompressedByte(posix_ostream &ofile, const uint8_t *vals, int total) {
  ofile.put(0);
  for (int i = 0; i < total; i++)
    ofile.put(vals[i]);
}

static void LL_WriteCompressedShort(posix_ostream &ofile, const uint16_t *vals, int total) {
  ofile.put(0);
  for (int i = 0; i < total; i++)
    ofile << vals[i];
}

// Maps a level-local texture index read from the file to a global
// GameTextures[] slot.  texture_xlate is built from the TXNM chunk; a missing
// or unmapped index (raw -1 / out of range) becomes 0, as in the engine.
static int16_t LL_TranslateTerrainTexture(int raw) {
  int g = (raw >= 0 && raw < MAX_TEXTURES) ? texture_xlate[raw] : -1;
  return (g >= 0) ? static_cast<int16_t>(g) : 0;
}

static void LL_ReadTerrainHeightChunk(posix_istream &ifile, int) {
  // The engine also folds the heights into a level checksum; the mini keeps no
  // level checksum, so the values are the only thing we retain.
  std::vector<uint8_t> byte_vals(TERRAIN_DEPTH * TERRAIN_WIDTH);
  LL_ReadCompressedByte(ifile, byte_vals.data(), static_cast<int>(byte_vals.size()));
  for (size_t i = 0; i < byte_vals.size(); i++)
    Terrain_seg[i].ypos = byte_vals[i];
}

static void LL_ReadTerrainSkyAndLightChunk(posix_istream &ifile, int) {
  ifile >> Terrain_sky.fog_scalar;
  ifile >> Terrain_sky.damage_per_second;

  uint8_t textured = 0;
  ifile >> textured;
  Terrain_sky.textured = textured;

  int16_t dome = 0;
  ifile >> dome;
  Terrain_sky.dome_texture = LL_TranslateTerrainTexture(dome);

  ifile >> Terrain_sky.sky_color;
  ifile >> Terrain_sky.horizon_color;
  ifile >> Terrain_sky.fog_color;
  ifile >> reinterpret_cast<uint32_t&>(Terrain_sky.flags);


  ifile >> Terrain_sky.radius;
  SetupSky(Terrain_sky.radius, Terrain_sky.flags, 1);

  ifile >> Terrain_sky.rotate_rate;

  int32_t num_sats = 0;
  ifile >> num_sats;
  Terrain_sky.num_satellites = static_cast<uint8_t>(std::min(num_sats, static_cast<int>(MAX_SATELLITES)));

  for (int i = 0; i < num_sats; i++) {
    int16_t tex = 0;
    ifile >> tex;
    if (i < MAX_SATELLITES)
      Terrain_sky.satellite_texture[i] = LL_TranslateTerrainTexture(tex);

    vector3 satvec{};
    ifile >> satvec;
    if (i < MAX_SATELLITES)
      Terrain_sky.satellite_vectors[i] = satvec;

    uint8_t sf = 0;
    ifile >> sf;
    if (i < MAX_SATELLITES)
      Terrain_sky.satellite_flags[i] = std::bit_cast<terrain_satellite_flags_t>(sf);

    float sz = 0.0f;
    ifile >> sz;
    if (i < MAX_SATELLITES)
      Terrain_sky.satellite_size[i] = sz;

    float r = 0.0f, g = 0.0f, b = 0.0f;
    ifile >> r >> g >> b;
    if (i < MAX_SATELLITES) {
      Terrain_sky.satellite_r[i] = r;
      Terrain_sky.satellite_g[i] = g;
      Terrain_sky.satellite_b[i] = b;
    }
  }

  const int total = TERRAIN_DEPTH * TERRAIN_WIDTH;
  std::vector<uint8_t> byte_vals(total);
  LL_ReadCompressedByte(ifile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_seg[i].l = byte_vals[i];
  LL_ReadCompressedByte(ifile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_seg[i].r = byte_vals[i];
  LL_ReadCompressedByte(ifile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_seg[i].g = byte_vals[i];
  LL_ReadCompressedByte(ifile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_seg[i].b = byte_vals[i];
  LL_ReadCompressedByte(ifile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_dynamic_table[i] = byte_vals[i];

  // Terrain occlusion cutaway table.  OCCLUSION_SIZE is 16; the on-disk block
  // is OCCLUSION_SIZE*OCCLUSION_SIZE*32 bytes packed into a [row][col] grid
  // with 32 columns.
  ifile >> Terrain_occlusion_checksum;
  constexpr int occ_total = OCCLUSION_SIZE * OCCLUSION_SIZE * 32;
  std::vector<uint8_t> occlusion(occ_total);
  LL_ReadCompressedByte(ifile, occlusion.data(), occ_total);
  for (int i = 0; i < occ_total; i++)
    Terrain_occlusion_map[i / 32][i % 32] = occlusion[i];
}

static void LL_ReadTerrainTmapFlagChunk(posix_istream &ifile, int) {
  constexpr int tex_total = TERRAIN_TEX_DEPTH * TERRAIN_TEX_WIDTH;
  std::vector<uint16_t> short_vals(tex_total);
  LL_ReadCompressedShort(ifile, short_vals.data(), tex_total);
  for (int i = 0; i < tex_total; i++)
    Terrain_tex_seg[i].tex_index = LL_TranslateTerrainTexture(short_vals[i]);

  std::vector<uint8_t> byte_vals(tex_total);
  LL_ReadCompressedByte(ifile, byte_vals.data(), tex_total);
  for (int i = 0; i < tex_total; i++) {
    Terrain_tex_seg[i].rotation = byte_vals[i];
    if ((Terrain_tex_seg[i].rotation >> 4) == 0)
      Terrain_tex_seg[i].rotation |= (1 << 4);
  }

  const int total = TERRAIN_DEPTH * TERRAIN_WIDTH;
  std::vector<uint8_t> flags(total);
  LL_ReadCompressedByte(ifile, flags.data(), total);
  for (int i = 0; i < total; i++)
    Terrain_seg[i].flags = std::bit_cast<terrain_segment_flags_t>(flags[i]);
}

// Reads the TERR container: sub-chunks until the TEND terminator, then
// regenerates derived data (AABB, normals, lightmaps).
static void LL_ReadTerrainChunks(posix_istream &ifile, uint32_t version) {
  // Force-reset so the min/max quadtree + LOD delta arrays are sized: the
  // mini never runs InitTerrain(), and the height/light sub-chunk readers and
  // BuildMinMaxTerrain() below write through them.  The height chunk re-fills
  // the ypos values that the force path zeroes.
  ResetTerrain(1);

  while (true) {
    char chunk_name[4];
    ifile.read(chunk_name, 4);
    if (ifile.eof())
      break;
    const long chunk_start = static_cast<long>(ifile.tell());
    int32_t size32 = 0;
    ifile >> size32;
    const long body_end = chunk_start + size32;

    if (IsChunk(chunk_name, "TERH"))
      LL_ReadTerrainHeightChunk(ifile, version);
    else if (IsChunk(chunk_name, "TETM"))
      LL_ReadTerrainTmapFlagChunk(ifile, version);
    else if (IsChunk(chunk_name, "TSKY"))
      LL_ReadTerrainSkyAndLightChunk(ifile, version);
    else if (IsChunk(chunk_name, "TEND"))
      break;

    if (ifile.tell() != body_end)
      ifile.seek(body_end, std::ios_base::beg);
  }

  BuildMinMaxTerrain();
  BuildTerrainNormals();
  UpdateTerrainLightmaps();

  std::ranges::fill(TerrainSelected, 0);
  Num_terrain_selected = 0;
}

static void LL_WriteTerrainHeightChunk(posix_ostream &ofile) {
  const int total = TERRAIN_DEPTH * TERRAIN_WIDTH;
  int start = LL_StartChunk(ofile, "TERH");

  std::vector<uint8_t> heightvals(total);
  for (int i = 0; i < total; i++)
    heightvals[i] = Terrain_seg[i].ypos;
  LL_WriteCompressedByte(ofile, heightvals.data(), total);

  LL_EndChunk(ofile, start);
}

static void LL_WriteTerrainTmapChunk(posix_ostream &ofile) {
  constexpr int tex_total = TERRAIN_TEX_DEPTH * TERRAIN_TEX_WIDTH;
  const int total = TERRAIN_DEPTH * TERRAIN_WIDTH;
  int start = LL_StartChunk(ofile, "TETM");

  std::vector<uint16_t> short_vals(tex_total);
  for (int i = 0; i < tex_total; i++)
    short_vals[i] = static_cast<uint16_t>(Terrain_tex_seg[i].tex_index);
  LL_WriteCompressedShort(ofile, short_vals.data(), tex_total);

  std::vector<uint8_t> byte_vals(tex_total);
  for (int i = 0; i < tex_total; i++)
    byte_vals[i] = Terrain_tex_seg[i].rotation;
  LL_WriteCompressedByte(ofile, byte_vals.data(), tex_total);

  byte_vals.assign(total, 0);
  for (int i = 0; i < total; i++)
    byte_vals[i] = std::bit_cast<uint8_t>(Terrain_seg[i].flags);
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);

  LL_EndChunk(ofile, start);
}

static void LL_WriteTerrainSkyAndLightChunk(posix_ostream &ofile) {
  int start = LL_StartChunk(ofile, "TSKY");

  ofile << Terrain_sky.fog_scalar;
  ofile << Terrain_sky.damage_per_second;
  ofile.put(static_cast<uint8_t>(Terrain_sky.textured));
  ofile << Terrain_sky.dome_texture;

  ofile << static_cast<int32_t>(Terrain_sky.sky_color);
  ofile << static_cast<int32_t>(Terrain_sky.horizon_color);
  ofile << static_cast<int32_t>(Terrain_sky.fog_color);

  ofile << std::bit_cast<int32_t>(Terrain_sky.flags);
  ofile << Terrain_sky.radius;
  ofile << Terrain_sky.rotate_rate;

  ofile << static_cast<int32_t>(Terrain_sky.num_satellites);
  for (int i = 0; i < Terrain_sky.num_satellites; i++) {
    ofile << Terrain_sky.satellite_texture[i];
    ofile << Terrain_sky.satellite_vectors[i];
    ofile.put(std::bit_cast<uint8_t>(Terrain_sky.satellite_flags[i]));
    ofile << Terrain_sky.satellite_size[i];
    ofile << Terrain_sky.satellite_r[i];
    ofile << Terrain_sky.satellite_g[i];
    ofile << Terrain_sky.satellite_b[i];
  }

  const int total = TERRAIN_DEPTH * TERRAIN_WIDTH;
  std::vector<uint8_t> byte_vals(total);
  for (int i = 0; i < total; i++)
    byte_vals[i] = Terrain_seg[i].l;
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    byte_vals[i] = Terrain_seg[i].r;
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    byte_vals[i] = Terrain_seg[i].g;
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    byte_vals[i] = Terrain_seg[i].b;
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);
  for (int i = 0; i < total; i++)
    byte_vals[i] = Terrain_dynamic_table[i];
  LL_WriteCompressedByte(ofile, byte_vals.data(), total);

  ofile << Terrain_occlusion_checksum;
  constexpr int occ_total = OCCLUSION_SIZE * OCCLUSION_SIZE * 32;
  std::vector<uint8_t> occlusion(occ_total);
  for (int i = 0; i < occ_total; i++)
    occlusion[i] = Terrain_occlusion_map[i / 32][i % 32];
  LL_WriteCompressedByte(ofile, occlusion.data(), occ_total);

  LL_EndChunk(ofile, start);
}

// TERR container.  Must be saved before OBJS: loading it runs ResetTerrain(),
// which clears the per-segment object links just before OBJS re-links them.
static void LL_WriteTerrainChunks(posix_ostream &ofile) {
  LL_WriteTerrainHeightChunk(ofile);
  LL_WriteTerrainTmapChunk(ofile);
  LL_WriteTerrainSkyAndLightChunk(ofile);

  int start = LL_StartChunk(ofile, "TEND");
  LL_EndChunk(ofile, start);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------


bool LoadLevel(const std::filesystem::path& filename, void (*cb_fn)(uint32_t, uint32_t, uint32_t))
{
  posix_istream ifile; // automatically closed by destructor
  if (!ifile.open(filename, std::ios_base::in))
    return false;

  // Default texture mapping is identity so faces that precede a TXNM chunk
  // (or files without one) still index GameTextures[] directly.
  for (int i = 0; i < (int)texture_xlate.size(); i++)
    texture_xlate[i] = i;

  // Default object/door mapping is "no translation" so a level without GNNM /
  // DRNM chunks keeps the page indices it was saved with.
  for (int i = 0; i < (int)generic_xlate.size(); i++)
    generic_xlate[i] = -1;
  for (int i = 0; i < (int)door_xlate.size(); i++)
    door_xlate[i] = -1;

  FreeAllRooms();

  // Reset the object table (matches the original's ResetObjectList: handles,
  // OBJ_NONE markers, roomnums -1, the free-object list, big-object list and
  // position-history state).
  ResetObjectList();
  Triggers.clear();

  // Reset the game-path table (engine LoadLevel calls InitGamePaths() before
  // the chunk loop, so paths from a previous level can't leak into this one).
  InitGamePaths();

  // Reset the lightmap state: the NLMP chunk (and any app-side allocations)
  // must start from an empty table.  lm_InitLightmaps() also (re)builds the
  // free list if this is the first use.
  lm_ShutdownLightmaps();
  lm_InitLightmaps();
  InitLightmapInfo();
  Num_lightmap_infos_read = 0;

  const size_t filelen = ifile.size();

  uint32_t id;
  ifile >> id;
  if(id != "D3LV"_ID){
    LOG_DEBUG("BAD MAGIC: %c%c%c%c",
              (char)((id>>0) & 0xFF),
              (char)((id>>8) & 0xFF),
              (char)((id>>16) & 0xFF),
              (char)((id>>24) & 0xFF));
    ifile.close();
    return false;
  }
  uint32_t version = 0;
  ifile >> version;
  // The editor writes the current LEVEL_FILE_VERSION layout via the stream
  // operators, whose room/face/portal/object/trigger serializers match the
  // engine on-disk format for version >= 127.  Reject anything older (or
  // newer than we can write) instead of mis-parsing a legacy layout.
  if (version > LEVEL_FILE_VERSION || version < 127) {
    throw std::runtime_error(
        std::string("Unsupported level file version ") + std::to_string(version) +
        " (expected between 127 and " + std::to_string(LEVEL_FILE_VERSION) + ")");
  }

  while (!(ifile >> id).eof())
  {
    LOG_DEBUG("Processing chunk: %c%c%c%c",
              (char)((id>>0) & 0xFF),
              (char)((id>>8) & 0xFF),
              (char)((id>>16) & 0xFF),
              (char)((id>>24) & 0xFF));

    long chunk_start = static_cast<long>(ifile.tell());
    uint32_t chunk_size = 0;
    ifile >> chunk_size;

    switch(id)
    {
      case "PATH"_ID: LL_ReadGamePathsChunk(ifile, version); break;
      case "TSND"_ID: LL_ReadTerrainSoundChunk(ifile, version); break;
      case "PSTR"_ID: LL_ReadPlayerStartsChunk(ifile, version); break;
      case "NLMP"_ID: LL_ReadNewLightmapChunk(ifile, version); break;
      case "ROOM"_ID:
        {
          uint32_t num_rooms = 0;
          ifile >> num_rooms;
          uint32_t t;
          ifile >> t; // nverts
          ifile >> t; // nfaces
          ifile >> t; // nfaceverts
          ifile >> t; // nportals
          uint16_t roomnum = 0;
          for (int i = 0; i < num_rooms; i++) {
            ifile >> roomnum;
            if (!RoomsEnsureIndex(roomnum)) {
              ifile.close();
              throw std::runtime_error("Level room index exceeds the room capacity");
            }
            LL_ReadRoom(ifile, &Rooms[roomnum], version);
          }
          // Smallest index after the rooms read is the high-water mark + 1.
          // Win32 clamped the watermark to MAX_ROOMS - 1; rooms above that
          // were palette-region scratch that never made it into the saved
          // mine, so drop them the same way.
          if (Rooms.size() > MAX_ROOMS) {
            for (int i = (int)Rooms.size() - 1; i >= MAX_ROOMS; --i)
              if (Rooms[i].used)
                FreeRoom(&Rooms[i]);
            Rooms.resize(MAX_ROOMS);
          }
          break;
        }
      case "TXNM"_ID:
        // Level-local texture name list.  Builds the level->global texture
        // index so faces (ReadFace's raw tmap index) resolve correctly.
        LL_ReadTextureList(ifile, chunk_size);
        break;
      case "GNNM"_ID:
        // Object page names; maps file object ids to the loaded game tables.
        LL_ReadNameXlateChunk(ifile, chunk_size, FindObjectIDName, generic_xlate.data(), MAX_OBJECT_IDS);
        break;
      case "DRNM"_ID:
        // Door page names; maps file door ids to the loaded game tables.
        LL_ReadNameXlateChunk(ifile, chunk_size, FindDoorName, door_xlate.data(), MAX_DOORS);
        break;
      case "RWND"_ID:
      {
        uint32_t num = 0;
        ifile >> num;
        for (uint32_t i = 0; i < num; i++) {
          uint16_t roomnum = 0;
          ifile >> roomnum;
          ifile >> Rooms[roomnum].wind;
        }
        break;
      }
      case "TERR"_ID: LL_ReadTerrainChunks(ifile, version); break;
      case "OHND"_ID:
      {
        // Object handles for deleted (OBJ_NONE) slots whose handle count part
        // is non-zero, so a freed slot's identity survives a save/load cycle.
        // Matches the engine's inline OHND reader.
        int32_t nh = 0;
        ifile >> nh;
        uint8_t already_loaded[MAX_OBJECTS] = {};
        for (int i = 0; i < nh; i++) {
          int32_t handle32 = 0;
          ifile >> handle32;
          int objnum = handle32 & HANDLE_OBJNUM_MASK;
          if (objnum < 0 || objnum >= MAX_OBJECTS)
            continue;
          Q_ASSERT(already_loaded[objnum] == 0);
          already_loaded[objnum] = 1;
          Objects[objnum].handle = handle32;
        }
        break;
      }
      case "OBJS"_ID:
      {
        uint32_t num = 0;
        ifile >> num;
        for (uint32_t i = 0; i < num; i++) {
          // Each record begins with the object's 32-bit handle (not its
          // index); the object number lives in the low bits.  This mirrors
          // the engine's LoadLevel (version >= 45).
          uint32_t handle = 0;
          ifile >> handle;

           int objnum = handle & HANDLE_OBJNUM_MASK;
          if (objnum < 0 || objnum >= MAX_OBJECTS)
            continue;
          object *obj = &Objects[objnum];
          // Value-initialise (NOT memset: object contains std::string members).
          *obj = object{};
          ifile >> *obj;

          // GNNM/DRNM name tables map the file's page index to the page index
          // in the loaded game tables, exactly as the engine's ReadObject does.
          obj->id = static_cast<uint16_t>(TranslateObjectId(obj->type, obj->id));

          int roomnum = obj->roomnum;
          LOG_DEBUG("OBJS[%d]: type=%d id=%d name='%s' flags=%u room=%d pos=(%f,%f,%f)",
                    objnum, (int)obj->type, (int)obj->id, obj->name.c_str(), std::bit_cast<uint32_t>(obj->flags),
                    roomnum, (double)obj->pos.x(), (double)obj->pos.y(), (double)obj->pos.z());
          // Give the object a usable handle and link it into the mine, exactly
          // as the original LL_ReadObjects does (object.cpp / LoadLevel.cpp).
          obj->handle = (version >= 45) ? handle : (objnum + HANDLE_COUNT_INCREMENT);
          obj->roomnum = -1; // ObjLink() expects the roomnum to be -1
          if ((roomnum > ((int)Rooms.size() - 1)) && !ROOMNUM_OUTSIDE(roomnum))
            obj->type = OBJ_NONE; // loading object with invalid room number
          else
            ObjLink(objnum, roomnum);
          if (objnum > Highest_object_index)
            Highest_object_index = objnum;
        }
        // Rebuild the free object list, as the original does after the OBJS
        // chunk (this syncs Num_objects and free_obj_list with the loaded set).
        ResetFreeObjects();
        // Copy type-specific defaults (size, shields, render type, ...) from
        // the object_info page for each loaded object, matching the engine's
        // ReadObject which calls ObjInit() (and thus ObjInitTypeSpecific).
        ObjReInitAll();
        break;
      }
      case "TRIG"_ID:
      {
        int32_t nt = 0;
        ifile >> nt;
        Triggers.resize(nt);
        for (int i = 0; i < static_cast<int>(Triggers.size()); i++) {
          trigger *tp = &Triggers[i];
          // Value-initialise (NOT memset: trigger contains a std::string name).
          *tp = trigger{};
          ifile >> *tp;
        }
        break;
      }
      case "AABB"_ID: LL_ReadRoomAABBChunk(ifile); break;
      case "CNBS"_ID: LL_ReadBSPChunk(ifile); break;
      case "CBOA"_ID: LL_ReadBOAChunk(ifile, version); break;
      case "NODE"_ID: LL_ReadBNodeChunk(ifile, version); break;
      case "MTCN"_ID: LL_ReadMatcenChunk(ifile); break;
      case "LVLG"_ID: Level_goals.LoadLevelGoalInfo(ifile); break;
      case "LIFE"_ID: a_life.LoadData(ifile); break;
      case "OSND"_ID: LL_ReadOverrideSoundChunk(ifile); break;
      case "FFTM"_ID: LL_ReadFFTMChunk(ifile, version); break;
      case "INFO"_ID: LL_ReadInfo(ifile, version); break;
      case "EDIT"_ID: LL_ReadEditorInfoChunk(ifile, version); break;
      default: // unknown / skipped chunk (PSTR, lightmaps, ...)
        LOG_DEBUG("UNHANDLED CHUNK: %c%c%c%c",
                  (char)((id>>0) & 0xFF),
                  (char)((id>>8) & 0xFF),
                  (char)((id>>16) & 0xFF),
                  (char)((id>>24) & 0xFF));
        break;
    }

    // Seek past any leftover body bytes to the next chunk boundary.
    long body_end = chunk_start + chunk_size;
    if (ifile.tell() != body_end)
      ifile.seek(body_end, std::ios_base::beg);

    if (cb_fn)
      cb_fn(id, chunk_size, static_cast<uint32_t>(filelen));
  }

  ifile.close();

  // Recompute face normals for any room still missing them and find first used
  // indices after the sparse room load.
  for (int i = 0; i < Rooms.size(); i++) {
    if (!Rooms[i].used)
      continue;
    for (int f = 0; f < Rooms[i].num_faces; f++)
      ComputeFaceNormal(&Rooms[i], f);
  }

  return true;
}

bool SaveLevel(const std::filesystem::path& filename, bool f_save_room_AABB) {
  (void)f_save_room_AABB;
  posix_ostream out;
  if (!out.open(filename, std::ios_base::out | std::ios_base::trunc))
    return false;

  try {
    out.write("D3LV", 4);
    int32_t ver = LEVEL_FILE_VERSION;
    out << ver;

    // PATH: the engine writes the game-path table first, before any other
    // geometry chunk.
    LL_WriteGamePathsChunk(out);

    // TSND: terrain sound bands (engine order: right after PATH).
    LL_WriteTerrainSoundChunk(out);

    // NLMP: room/terrain lightmaps (engine order: after terrain sounds, before
    // the texture list).
    LL_WriteLightmapChunk(out);

    // PSTR: player start flags (engine order: right after the lightmaps).
    LL_WritePlayerStartsChunk(out);

    // TXNM: no texture names; write an empty list.
    {
      int start = LL_StartChunk(out, "TXNM");
      int32_t zero = 0;
      out << zero;
      LL_EndChunk(out, start);
    }

    // GNNM: object page names for id mapping on load; only the used slots up
    // to the highest used page are written (WRITE_DATA_NAMES_GENERIC).
    {
      int start = LL_StartChunk(out, CHUNK_GENERIC_NAMES);
      int highest = -1;
      for (int i = 0; i < MAX_OBJECT_IDS; i++)
        if (Object_info[i].type != OBJ_NONE)
          highest = i;
      out << (int32_t)(highest + 1);
      for (int i = 0; i <= highest; i++)
        out << (Object_info[i].type != OBJ_NONE ? Object_info[i].name : std::string());
      LL_EndChunk(out, start);
    }

    // DRNM: door page names for id mapping on load (WRITE_DATA_NAMES).
    {
      int start = LL_StartChunk(out, CHUNK_DOOR_NAMES);
      int highest = -1;
      for (int i = 0; i < MAX_DOORS; i++)
        if (Doors[i].used)
          highest = i;
      out << (int32_t)(highest + 1);
      for (int i = 0; i <= highest; i++)
        out << (Doors[i].used ? Doors[i].name : std::string());
      LL_EndChunk(out, start);
    }

    // ROOM
    {
      int start = LL_StartChunk(out, CHUNK_ROOMS);
      int nrooms = 0, nverts = 0, nfaces = 0, nfaceverts = 0, nportals = 0;
      for (int i = 0; i < Rooms.size(); i++) {
        if (!Rooms[i].used)
          continue;
        nrooms++;
        nverts += Rooms[i].num_verts;
        nfaces += Rooms[i].num_faces;
        nportals += Rooms[i].num_portals;
        for (int f = 0; f < Rooms[i].num_faces; f++)
          nfaceverts += Rooms[i].faces[f].num_verts;
      }
      out << nrooms;
      out << nverts;
      out << nfaces;
      out << nfaceverts;
      out << nportals;
      for (int i = 0; i < Rooms.size(); i++) {
        if (!Rooms[i].used)
          continue;
        int16_t room = (int16_t)i;
        out << room;
        out << Rooms[i];
      }
      LL_EndChunk(out, start);
    }

    // RWND (room wind)
    {
      int nwind = 0;
      for (int i = 0; i < Rooms.size(); i++)
        if (Rooms[i].used && (Rooms[i].wind.x() != 0.0f || Rooms[i].wind.y() != 0.0f || Rooms[i].wind.z() != 0.0f))
          nwind++;
      if (nwind) {
        int start = LL_StartChunk(out, CHUNK_ROOM_WIND);
        out << nwind;
        for (int i = 0; i < Rooms.size(); i++) {
          if (Rooms[i].used && (Rooms[i].wind.x() != 0.0f || Rooms[i].wind.y() != 0.0f || Rooms[i].wind.z() != 0.0f)) {
            int16_t room = (int16_t)i;
            out << room;
            out << Rooms[i].wind;
          }
        }
        LL_EndChunk(out, start);
      }
    }

    // TERR (exterior terrain: heights, texmaps, sky/lighting)
    {
      int start = LL_StartChunk(out, "TERR");
      LL_WriteTerrainChunks(out);
      LL_EndChunk(out, start);
    }

    // OHND (object handles): persist the handles of deleted object slots so
    // their count part survives a save/load cycle.  Written unconditionally
    // (may be an empty list), like the engine.
    {
      int handleCount = 0;
      for (int i = 0; i < (int)Objects.size(); i++)
        if (Objects[i].type == OBJ_NONE && (Objects[i].handle & HANDLE_COUNT_MASK) != 0)
          handleCount++;
      int start = LL_StartChunk(out, CHUNK_OBJECT_HANDLES);
      out << handleCount;
      for (int i = 0; i < (int)Objects.size(); i++) {
        if (Objects[i].type == OBJ_NONE && (Objects[i].handle & HANDLE_COUNT_MASK) != 0)
          out << (int32_t)Objects[i].handle;
      }
      LL_EndChunk(out, start);
    }

    // OBJS
    {
      int start = LL_StartChunk(out, CHUNK_OBJECTS);
      int count = 0;
      for (int i = 0; i <= Highest_object_index; i++)
        if (Objects[i].type != OBJ_NONE)
          count++;
      out << count;
      for (int i = 0; i <= Highest_object_index; i++) {
        if (Objects[i].type == OBJ_NONE)
          continue;
        // Engine-compatible record: handle first (low bits = object index),
        // then the full placement record via the object stream operator.
        out << (int32_t)(i + HANDLE_COUNT_INCREMENT);
        out << Objects[i];
      }
      LL_EndChunk(out, start);
    }

    // TRIG
    {
      int start = LL_StartChunk(out, CHUNK_TRIGGERS);
      out << static_cast<int>(Triggers.size());
      for (int i = 0; i < static_cast<int>(Triggers.size()); i++)
        out << Triggers[i];
      LL_EndChunk(out, start);
    }

    // CNBS (new BSP tree): written right after TRIG, only when a tree is
    // present — mirrors the engine's `if (BSP_initted)` gate and keeps the
    // chunk order (CNBS CBOA NODE) identical to the engine's WriteLevel.
    if (BSP_initted)
      LL_WriteBSPChunk(out);

    // CBOA (automatic path point / BOA data).  Written after the TRIG block,
    // before AABB — mirrors engine SaveLevel relative order and keeps BOA
    // fields alive across a read->write round trip.
    LL_WriteBOAChunk(out);

    if (BNode_allocated)
      LL_WriteBNodeChunk(out);

    // AABB (room bounding boxes / BBF region lists)
    LL_WriteRoomAABBChunk(out);

    // OSND / FFTM (override sounds + force-field bounce textures).  Engine
    // (game build) order: after the BOA/BNode chunks, before AABB.
    LL_WriteOverrideSoundChunk(out);
    LL_WriteFFTMChunk(out);

    // MTCN (matcen data).  Written unconditionally; per-record SaveData does
    // not Reset() (see mini/editor/matcen.cpp) so a re-saved level keeps the
    // stored field values byte-stable.
    LL_WriteMatcenChunk(out);

    // LVLG (level goals).  Written between MTCN and INFO, matching the
    // engine's relative order.
    {
      int start = LL_StartChunk(out, CHUNK_LEVEL_GOALS);
      Level_goals.SaveLevelGoalInfo(out);
      LL_EndChunk(out, start);
    }

    // LIFE (ambient life data).  Engine order: after matcen/goals, before INFO.
    {
      int start = LL_StartChunk(out, CHUNK_ALIFE_DATA);
      a_life.SaveData(out);
      LL_EndChunk(out, start);
    }

    // INFO
    {
      int start = LL_StartChunk(out, CHUNK_LEVEL_INFO);
      LL_WriteInfo(out);
      LL_EndChunk(out, start);
    }

    // EDIT — editor state; the engine always writes this as the last chunk.
    LL_WriteEditorInfoChunk(out);
  } catch (std::exception &) {
    out.close();
    return false;
  }

  out.close();
  return true;
}
