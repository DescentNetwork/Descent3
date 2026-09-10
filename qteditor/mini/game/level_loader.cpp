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
//   TXNM  - skipped (no texture xlate table in the mini; raw indices survive)
//   ROOM  - room geometry (verts, faces, portals), Comp face normals after
//   RWND  - per-room wind vectors
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
#include "string_helpers.h"
#include "log.h"
#include "d3x_op.h"
#include "object_external_struct.h"

#include <QtGlobal>

#include <cstring>
#include <cstdio>
#include <stdexcept>

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

  FreeAllRooms();

  // Reset the object table (matches the original's ResetObjectList: handles,
  // OBJ_NONE markers, roomnums -1, the free-object list, big-object list and
  // position-history state).
  ResetObjectList();
  Num_triggers = 0;

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
      ifile.close();
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

      if (IsChunk(chunk_name, "ROOM")) {
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
      } else if (IsChunk(chunk_name, "RWND")) {
        int32_t num = 0;
        ifile >> num;
        int nrooms = num;
        for (int i = 0; i < nrooms; i++) {
          int16_t roomnum = 0;
          ifile >> roomnum;
          ifile >> Rooms[roomnum].wind;
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

          int roomnum = obj->roomnum;
          LOG_DEBUG("OBJS[%d]: type=%d id=%d name='%s' flags=%u room=%d pos=(%f,%f,%f)",
                    objnum, (int)obj->type, (int)obj->id, obj->name.c_str(), (unsigned)obj->flags, roomnum,
                    (double)obj->pos.x(), (double)obj->pos.y(), (double)obj->pos.z());
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

    // TXNM: no texture names; write an empty list.
    {
      int start = LL_StartChunk(out, "TXNM");
      int32_t zero = 0;
      out << zero;
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
