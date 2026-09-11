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
//   NLMP  - room/terrain lightmaps (textures + lightmap-info records)
//   TXNM  - skipped (no texture xlate table in the mini; raw indices survive)
//   ROOM  - room geometry (verts, faces, portals), Comp face normals after
//   RWND  - per-room wind vectors
//   TERR  - exterior terrain (heights, texmaps/flags, sky & lighting)
//   OBJS  - object placement data.  Each record is the engine's handle
//           (object number in the low bits) followed by the placement
//           prefix the mini keeps: type/id/name/flags/roomnum/pos/orient.
//   TRIG  - trigger table
//   INFO  - level name/designer/copyright/notes + level physics params
//   PSTR  - skipped (player starts are editor-only; ignored on load)
//
// All other chunks are skipped by seeking to chunk_start + chunk_size.
// SaveLevel writes those chunks in the same format so LoadLevel round-trips.

#include "level_loader.h"
#include "room.h"
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

#include <QtGlobal>

#include <cstring>
#include <cstdio>
#include <stdexcept>

#include <algorithm>
#include <bit>
#include <vector>

#define LL_TAG "D3LV"


static bool IsChunk(const char *chunk_name, const char *id) { return chunk_name[0] == id[0] && chunk_name[1] == id[1] && chunk_name[2] == id[2] && chunk_name[3] == id[3]; }

// A level's faces reference textures by an index into the level's own
// texture-name list (TXNM chunk).  texture_xlate[levelIdx] maps that index to
// the corresponding slot in the global GameTextures[] (matched by name).
// Reset to identity before each LoadLevel so the raw index survives when no
// TXNM list is present (e.g. our own saved files).
static int texture_xlate[MAX_TEXTURES];

static int LL_FindTextureName(const std::string& name) {
  for (int i = 0; i < Num_textures; i++) {
    if (match(GameTextures[i].name, name))
      return i;
  }
  return -1;
}


// Reads a TXNM chunk: an int32 count, then that many null-terminated texture
// names.  Builds texture_xlate[] so faces can map level-texture-index → global
// GameTextures index.
static void LL_ReadTextureList(posix_istream &ifile, int chunk_size) {
  int32_t n32 = 0;
  ifile >> n32;
  int n = n32;
  long end = ifile.tell() + (chunk_size - 4);
  for (int i = 0; i < n; i++) {
    std::string name;
    std::string s;
    ifile >> name;
    int g = LL_FindTextureName(name);
    if (g < 0)
      g = 0;
    texture_xlate[i] = g;
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
static int16_t generic_xlate[MAX_OBJECT_IDS];
static int16_t door_xlate[MAX_DOORS];

// Reads a GNNM/DRNM chunk body: an int32 count, then that many null-terminated
// page names.  Each name is resolved through lookup(); an empty name (an
// unused slot) is never looked up and maps to -1, exactly as in the engine's
// BuildXlateTable.  The trailing entries up to max_items are cleared to -1 so
// a partially filled table never leaks indices from a previous level.
static void LL_ReadNameXlateChunk(posix_istream &ifile, int chunk_size,
                                  int (*lookup)(const std::string &), int16_t *xlate, int max_items) {
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
      xlate[i] = lookup(name);
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
// slot for OBJ_DOOR).  Returns -1 when no game table provides one.
static int FindValidID(int type) {
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
    return -1;
  default:
    return -1;
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

  const int valid = FindValidID(type);
  return (valid != -1) ? valid : id;
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

static int LL_ReadRoom(posix_istream &ifile, room *rp, int /*version*/) {
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
static void LL_ReadGamePathsChunk(posix_istream &ifile, int version) {
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
  for (int i = 0; i < MAX_GAME_PATHS; i++)
    if (GamePaths[i].used)
      npaths++;

  int start = LL_StartChunk(ofile, "PATH");
  ofile << (int16_t)npaths;
  for (int i = 0; i < MAX_GAME_PATHS; i++) {
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

static void LL_ReadNewLightmapChunk(posix_istream &ifile, int version) {
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
    int lm_handle = lm_AllocLightmap(map_w, map_h);
    if (lm_handle == BAD_LM_INDEX)
      lm_handle = 0;
    lightmap_remap[i] = static_cast<uint16_t>(lm_handle);
    LL_ReadCompressedShortArray(ifile, lm_data(lm_handle), map_w * map_h);
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

    int lmi = AllocLightmapInfo(w, h, type, false);
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
        const int map_w = lm_w(lm_handle);
        const int map_h = lm_h(lm_handle);
        ofile << (int16_t)map_w;
        ofile << (int16_t)map_h;
        LL_CheckToWriteCompressShort(ofile, lm_data(lm_handle), map_w * map_h);
      }
    }
  }

  ofile << (int32_t)lightmap_info_count;
  for (int i = 0; i < MAXINFOS; i++) {
    if (LightmapInfo[i].used && LightmapInfo[i].type != LMI_DYNAMIC) {
      const lightmap_info &info = LightmapInfo[i];
      ofile << (int16_t)lightmap_remap[info.lm_handle];
      ofile << (int16_t)lmi_w(i);
      ofile << (int16_t)lmi_h(i);
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

  int32_t c = 0;
  ifile >> c;
  Terrain_sky.sky_color = static_cast<ddgr_color>(c);
  ifile >> c;
  Terrain_sky.horizon_color = static_cast<ddgr_color>(c);
  ifile >> c;
  Terrain_sky.fog_color = static_cast<ddgr_color>(c);

  uint32_t flags32 = 0;
  ifile >> flags32;
  Terrain_sky.flags = std::bit_cast<terrain_sky_flags_t>(flags32);

  ifile >> Terrain_sky.radius;
  SetupSky(Terrain_sky.radius, static_cast<int>(flags32), 1);

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
static void LL_ReadTerrainChunks(posix_istream &ifile, int version) {
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

  memset(TerrainSelected, 0, TERRAIN_WIDTH * TERRAIN_DEPTH);
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


bool LoadLevel(const std::filesystem::path& filename, void (*cb_fn)(const char *, int, int)) {
  posix_istream ifile;
  if (!ifile.open(filename, std::ios_base::in))
    return false;

  // Default texture mapping is identity so faces that precede a TXNM chunk
  // (or files without one) still index GameTextures[] directly.
  for (int i = 0; i < MAX_TEXTURES; i++)
    texture_xlate[i] = i;

  // Default object/door mapping is "no translation" so a level without GNNM /
  // DRNM chunks keeps the page indices it was saved with.
  for (int i = 0; i < MAX_OBJECT_IDS; i++)
    generic_xlate[i] = -1;
  for (int i = 0; i < MAX_DOORS; i++)
    door_xlate[i] = -1;

  FreeAllRooms();

  // Reset the object table (matches the original's ResetObjectList: handles,
  // OBJ_NONE markers, roomnums -1, the free-object list, big-object list and
  // position-history state).
  ResetObjectList();
  Num_triggers = 0;

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

  try {
    char tag[4];
    ifile.read(tag, 4);
    if (tag[0] != 'D' || tag[1] != '3' || tag[2] != 'L' || tag[3] != 'V') {
      ifile.close();
      return false;
    }
    int32_t version32 = 0;
    ifile >> version32;
    int version = version32;
    // The editor writes the current LEVEL_FILE_VERSION layout via the stream
    // operators, whose room/face/portal/object/trigger serializers match the
    // engine on-disk format for version >= 127.  Reject anything older (or
    // newer than we can write) instead of mis-parsing a legacy layout.
    if (version > LEVEL_FILE_VERSION || version < 127) {
      // Let the catch block below close the (still-open) stream once.
      throw std::runtime_error(
          std::string("Unsupported level file version ") + std::to_string(version) +
          " (expected between 127 and " + std::to_string(LEVEL_FILE_VERSION) + ")");
    }

    while (!ifile.eof()) {
      char chunk_name[4];
      ifile.read(chunk_name, 4);
      if (ifile.eof()) // Run off the end of the file on a chunk boundary
        break;
      long chunk_start = static_cast<long>(ifile.tell());
      int32_t chunk_size32 = 0;
      ifile >> chunk_size32;
      int chunk_size = chunk_size32;

      if (IsChunk(chunk_name, "PATH")) {
        LL_ReadGamePathsChunk(ifile, version);
      } else if (IsChunk(chunk_name, "NLMP")) {
        LL_ReadNewLightmapChunk(ifile, version);
      } else if (IsChunk(chunk_name, "ROOM")) {
        int32_t num = 0;
        ifile >> num;
        int num_rooms = num;
        int32_t t;
        ifile >> t; // nverts
        ifile >> t; // nfaces
        ifile >> t; // nfaceverts
        ifile >> t; // nportals
        int roomnum = 0;
        for (int i = 0; i < num_rooms; i++) {
          int16_t room = 0;
          ifile >> room;
          roomnum = room;
          LL_ReadRoom(ifile, &Rooms[roomnum], version);
        }
        Highest_room_index = roomnum;
        if (Highest_room_index < 0 || Highest_room_index >= MAX_ROOMS)
          Highest_room_index = MAX_ROOMS - 1;
      } else if (IsChunk(chunk_name, "TXNM")) {
        // Level-local texture name list.  Builds the level->global texture
        // index so faces (ReadFace's raw tmap index) resolve correctly.
        LL_ReadTextureList(ifile, chunk_size);
      } else if (IsChunk(chunk_name, CHUNK_GENERIC_NAMES)) {
        // Object page names; maps file object ids to the loaded game tables.
        LL_ReadNameXlateChunk(ifile, chunk_size, FindObjectIDName, generic_xlate, MAX_OBJECT_IDS);
      } else if (IsChunk(chunk_name, CHUNK_DOOR_NAMES)) {
        // Door page names; maps file door ids to the loaded game tables.
        LL_ReadNameXlateChunk(ifile, chunk_size, FindDoorName, door_xlate, MAX_DOORS);
      } else if (IsChunk(chunk_name, "RWND")) {
        int32_t num = 0;
        ifile >> num;
        int nrooms = num;
        for (int i = 0; i < nrooms; i++) {
          int16_t roomnum = 0;
          ifile >> roomnum;
          ifile >> Rooms[roomnum].wind;
        }
      } else if (IsChunk(chunk_name, "TERR")) {
        LL_ReadTerrainChunks(ifile, version);
      } else if (IsChunk(chunk_name, CHUNK_OBJECT_HANDLES)) {
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
      } else if (IsChunk(chunk_name, "OBJS")) {
        int32_t num = 0;
        ifile >> num;
        int n = num;
        for (int i = 0; i < n; i++) {
          // Each record begins with the object's 32-bit handle (not its
          // index); the object number lives in the low bits.  This mirrors
          // the engine's LoadLevel (version >= 45).
          int32_t handle32 = 0;
          ifile >> handle32;
int handle = handle32;
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
          if ((roomnum > Highest_room_index) && !ROOMNUM_OUTSIDE(roomnum))
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
      } else if (IsChunk(chunk_name, "TRIG")) {
        int32_t nt = 0;
        ifile >> nt;
        Num_triggers = nt;
        if (Num_triggers > 500)
          Num_triggers = 500;
        for (int i = 0; i < Num_triggers; i++) {
          trigger *tp = &Triggers[i];
          // Value-initialise (NOT memset: trigger contains a std::string name).
          *tp = trigger{};
          ifile >> *tp;
        }
      } else if (IsChunk(chunk_name, "INFO")) {
        LL_ReadInfo(ifile, version);
      } else {
        // unknown / skipped chunk (PSTR, lightmaps, ...)
      }

      // Seek past any leftover body bytes to the next chunk boundary.
      long body_end = chunk_start + chunk_size;
      if (ifile.tell() != body_end)
        ifile.seek(body_end, std::ios_base::beg);

      if (cb_fn)
        cb_fn(chunk_name, chunk_size, (int)filelen);
    }
  } catch (const std::runtime_error &e) {
    // A well-formed D3LV header with an unsupported version is a *distinct*
    // failure from a corrupt/unrecognized file: propagate it so callers can
    // tell the user the exact reason instead of the generic "false" path.
    (void)e;
    ifile.close();
    throw;
  } catch (std::exception &) {
    ifile.close();
    return false;
  }

  ifile.close();

  // Recompute face normals for any room still missing them and find first used
  // indices after the sparse room load.
  for (int i = 0; i <= Highest_room_index; i++) {
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

    // NLMP: room/terrain lightmaps (engine order: after terrain sounds, before
    // the texture list).
    LL_WriteLightmapChunk(out);

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
      for (int i = 0; i <= Highest_room_index; i++) {
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
      for (int i = 0; i <= Highest_room_index; i++) {
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
      for (int i = 0; i <= Highest_room_index; i++)
        if (Rooms[i].used && (Rooms[i].wind.x() != 0.0f || Rooms[i].wind.y() != 0.0f || Rooms[i].wind.z() != 0.0f))
          nwind++;
      if (nwind) {
        int start = LL_StartChunk(out, CHUNK_ROOM_WIND);
        out << nwind;
        for (int i = 0; i <= Highest_room_index; i++) {
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
      for (int i = 0; i < MAX_OBJECTS; i++)
        if (Objects[i].type == OBJ_NONE && (Objects[i].handle & HANDLE_COUNT_MASK) != 0)
          handleCount++;
      int start = LL_StartChunk(out, CHUNK_OBJECT_HANDLES);
      out << handleCount;
      for (int i = 0; i < MAX_OBJECTS; i++) {
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
      out << Num_triggers;
      for (int i = 0; i < Num_triggers; i++)
        out << Triggers[i];
      LL_EndChunk(out, start);
    }

    // INFO
    {
      int start = LL_StartChunk(out, CHUNK_LEVEL_INFO);
      LL_WriteInfo(out);
      LL_EndChunk(out, start);
    }
  } catch (std::exception &) {
    out.close();
    return false;
  }

  out.close();
  return true;
}
