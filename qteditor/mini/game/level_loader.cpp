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

// Read/write a vector (3 floats)
static void LL_ReadVector(posix_istream &f, vector3 &v) {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  f >> x;
  f >> y;
  f >> z;
  v = vector3{x, y, z};
}
static void LL_WriteVector(posix_ostream &f, const vector3 &v) {
  f << v.x();
  f << v.y();
  f << v.z();
}

// Read a matrix (rvec, uvec, fvec each a vector)
static void LL_ReadMatrix(posix_istream &f, matrix &m) {
  LL_ReadVector(f, m.rvec);
  LL_ReadVector(f, m.uvec);
  LL_ReadVector(f, m.fvec);
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
static void LL_ReadCompressionByte(posix_istream &fp, uint8_t *vals, int total) {
  int count = 0;
  uint8_t compressed = 0;
  fp >> compressed;
  if (compressed == 0) {
    for (int i = 0; i < total; i++)
      fp >> vals[i];
    return;
  }
  while (count != total) {
    uint8_t command = 0;
    fp >> command;
    if (command == 0) { // next byte is raw
      fp >> vals[count];
      count++;
    } else if (command >= 2 && command <= 250) {
      uint8_t height = 0;
      fp >> height;
      for (int k = 0; k < command; k++) {
        vals[count] = height;
        count++;
      }
    } else {
      fp.close();
      throw std::runtime_error("bad compression run");
    }
  }
}

static int LL_ReadFace(posix_istream &ifile, face *fp, int version) {
  uint8_t nverts_b = 0;
  ifile >> nverts_b;
  int nverts = nverts_b;
  int i;

  InitRoomFace(fp, nverts);

  for (i = 0; i < fp->num_verts; i++)
    ifile >> fp->face_verts[i];

  int alphaed = 0;
  for (i = 0; i < fp->num_verts; i++) {
    ifile >> fp->face_uvls[i].u;
    ifile >> fp->face_uvls[i].v;

    if (version < 56) {
      for (int k = 0; k < 4; k++) {
        float trash;
        ifile >> trash;
      }
    }

    if (version >= 21) {
      if (version < 61) {
        float a = 0.0f;
        ifile >> a;
        fp->face_uvls[i].alpha = (uint8_t)(a * 255.0f);
      } else
        ifile >> fp->face_uvls[i].alpha;
    } else
      fp->face_uvls[i].alpha = 255;

    if (fp->face_uvls[i].alpha != 255)
      alphaed = 1;
  }

  if (version < 27) {
    uint8_t fb = 0;
    ifile >> fb;
    fp->flags = fb;
  } else
    ifile >> fp->flags;

  if (alphaed)
    fp->flags |= FF_VERTEX_ALPHA;
  else
    fp->flags &= ~FF_VERTEX_ALPHA;

  if (version >= 23)
    ifile >> fp->portal_num;
  else
    fp->portal_num = 0;

  // Map the level-texture-index to the global GameTextures[] slot via the
  // TXNM list (identity when no list was present).  Clamp to safe range.
  ifile >> fp->tmap;
  fp->tmap = (fp->tmap >= 0 && fp->tmap < MAX_TEXTURES) ? texture_xlate[fp->tmap] : 0;

  if ((fp->flags & FF_LIGHTMAP) && (version >= 19)) {
    if (version <= 29) {
      uint8_t w = 0;
      uint8_t h = 0;
      ifile >> w;
      ifile >> h;
      for (i = 0; i < w * h; i++) {
        int16_t trash;
        ifile >> trash;
      }
      fp->flags &= ~FF_LIGHTMAP;
    } else {
      ifile >> fp->lmi_handle;
      if (version <= 88) {
        for (int k = 0; k < 4; k++) {
          uint8_t trash;
          ifile >> trash;
        }
      }
      for (i = 0; i < fp->num_verts; i++) {
        ifile >> fp->face_uvls[i].u2;
        ifile >> fp->face_uvls[i].v2;
      }
    }
  }

  if (version >= 22 && version <= 29) {
    vector3 trash;
    LL_ReadVector(ifile, trash);
  }
  if (version >= 40 && version <= 60) {
    int16_t trash;
    ifile >> trash;
    ifile >> trash;
  }

  if (version >= 50) {
    ifile >> fp->light_multiple;
    if (fp->light_multiple == 186)
      fp->light_multiple = 4;
    if (version <= 52)
      fp->light_multiple *= 4;
  } else
    fp->light_multiple = 4;

  // Special (specular) faces are not modeled by the mini.  Consume the block
  // (if present) to keep real-level alignment; we never write one ourselves.
  if (version >= 71) {
    uint8_t special = 0;
    ifile >> special;
    if (special) {
      if (version < 77) {
        uint8_t trash;
        ifile >> trash;
        vector3 tv;
        LL_ReadVector(ifile, tv);
        int16_t s;
        ifile >> s;
      } else {
        uint8_t trash;
        ifile >> trash; // type
        uint8_t num = 0;
        ifile >> num;
        int smooth = 0;
        int num_smooth = 0;
        if (version >= 117) {
          uint8_t sb = 0;
          ifile >> sb;
          smooth = sb ? 1 : 0;
          if (smooth) {
            uint8_t ns = 0;
            ifile >> ns;
            num_smooth = ns;
          }
        }
        for (i = 0; i < num; i++) {
          vector3 tv;
          LL_ReadVector(ifile, tv);
          int16_t s;
          ifile >> s;
        }
        if (smooth) {
          for (i = 0; i < num_smooth; i++) {
            vector3 tv;
            LL_ReadVector(ifile, tv);
          }
        }
      }
    }
  }

  return 1;
}

// Legacy (pre-127) portal layout.  Current-format portals serialize through
// the portal stream operator (see room_serialization.cpp).
static int LL_ReadPortal(posix_istream &ifile, portal *pp, int version) {
  ifile >> pp->flags;
  if (version < 103)
    pp->flags &= ~(1 << 2);

  if (version < 80) {
    int16_t num_verts = 0;
    ifile >> num_verts;
    for (int i = 0; i < num_verts; i++) {
      int16_t trash;
      ifile >> trash;
    }
    int16_t trash;
    ifile >> trash; // num_faces, assumed 1
  }

  ifile >> pp->portal_face;
  // croom/cportal are stored as int32 on disk (matching the engine writer)
  // even though the mini struct holds them as int16.
  int32_t room32 = 0;
  ifile >> room32;
  pp->croom = (int16_t)room32;
  int32_t portal32 = 0;
  ifile >> portal32;
  pp->cportal = (int16_t)portal32;

  if (version >= 123)
    ifile >> pp->bnode_index;
  else
    pp->bnode_index = -1;

  if (version >= 63)
    LL_ReadVector(ifile, pp->path_pnt);

  if (version >= 100)
    ifile >> pp->combine_master;

  return 1;
}

// Reads a room from a disk file.  Mirrors the engine field-by-field for
// versions >= 127 (the mini writes/reads flash these fields).
static int LL_ReadRoom(posix_istream &ifile, room *rp, int version) {
  if (version >= 127) {
    // Current canonical layout: the room stream operator reads every field
    // (verts, faces, portals, lights, ...) exactly as the engine does.
    ifile >> *rp;
    // Map raw level-local texture indices into global GameTextures[] slots.
    // (LL_ReadFace does this inline for the legacy path.)
    for (int f = 0; f < rp->num_faces; f++) {
      const int raw = rp->faces[f].tmap;
      rp->faces[f].tmap = (raw >= 0 && raw < MAX_TEXTURES) ? texture_xlate[raw] : 0;
    }
    return 1;
  }

  int32_t nverts32 = 0;
  int32_t nfaces32 = 0;
  int32_t nportals32 = 0;
  ifile >> nverts32;
  ifile >> nfaces32;
  ifile >> nportals32;
  int nverts = nverts32;
  int nfaces = nfaces32;
  int nportals = nportals32;

  int i;
  InitRoom(rp, nverts, nfaces, nportals);

  rp->name.clear();
  if (version >= 96) {
    ifile >> rp->name;
  }

  if (version >= 63)
    LL_ReadVector(ifile, rp->path_pnt);

  for (i = 0; i < rp->num_verts; i++) {
    LL_ReadVector(ifile, rp->verts[i]);
    if (version >= 52 && version <= 67) {
      uint8_t trash;
      ifile >> trash;
      ifile >> trash;
    } else if (version >= 68 && version < 71) {
      vector3 trash;
      LL_ReadVector(ifile, trash);
      int16_t s;
      ifile >> s;
    }
  }

  for (i = 0; i < rp->num_faces; i++) {
    LL_ReadFace(ifile, &rp->faces[i], version);
    ComputeFaceNormal(rp, i);
  }

  for (i = 0; i < rp->num_portals; i++)
    LL_ReadPortal(ifile, &rp->portals[i], version);

  ifile.read(&rp->flags, sizeof(uint32_t));
  if (version < 29) {
    float trash;
    ifile >> trash;
  }

  if (version >= 68) {
    ifile >> rp->pulse_time;
    ifile >> rp->pulse_offset;
  }

  if (version >= 79)
    ifile >> rp->mirror_face;
  else
    rp->mirror_face = -1;

  if (rp->flags.door) {
    // The mini does not model 3d doors, but we must consume the bytes so the
    // rest of the chunk stays aligned. (Engine writes these as:
    // flags(byte) keys(byte) doornum(int) position(float) for version >= 106.)
    uint8_t dflags;
    ifile >> dflags; // flags
    uint8_t dkeys;
    ifile >> dkeys; // keys_needed
    int32_t door = 0;
    ifile >> door; // doornum
    float pos = 0.0f;
    ifile >> pos; // position (version >= 106 only)
  }

  if (version >= 67) {
    uint8_t haslights = 0;
    ifile >> haslights;
    if (haslights == 1) {
      int32_t w = 0;
      int32_t h = 0;
      int32_t d = 0;
      ifile >> w;
      ifile >> h;
      ifile >> d;
      int size = w * h * d;
      if (size) {
        rp->volume_lights.resize(size);
        LL_ReadCompressionByte(ifile, rp->volume_lights.data(), size);
      }
      rp->volume_width = (int16_t)w;
      rp->volume_height = (int16_t)h;
      rp->volume_depth = (int16_t)d;
    }
  }

  if (version >= 73) {
    ifile >> rp->fog_depth;
    ifile >> rp->fog_r;
    ifile >> rp->fog_g;
    ifile >> rp->fog_b;
  }

  if (version >= 78) {
    std::string s;
    ifile >> s;
    // Mini has no ambient sound page; leave ambient_sound at InitRoom's -1.
  }

  if (version >= 98) {
    uint8_t e = 0;
    ifile >> e;
    rp->env_reverb = e;
  } else
    rp->env_reverb = 0;

  if (version >= 108) {
    ifile >> rp->damage;
    ifile >> rp->damage_type;
  }

  return 1;
}

static void LL_ReadInfo(posix_istream &ifile, int version) {
  Level_info.name =  "Unnamed";
  Level_info.designer = "Anonymous";
  Level_info.copyright.clear();
  Level_info.notes.clear();

  ifile >> Level_info.name;
  ifile >> Level_info.designer;
  ifile >> Level_info.copyright;
  ifile >> Level_info.notes;

  if (version >= 83)
    ifile >> Gravity_strength;
  else
    Gravity_strength = -32.2f;

  if (version >= 131) {
    int32_t v = 0;
    ifile >> v;
    Level_powerups_ignore_wind = (v != 0);
  } else
    Level_powerups_ignore_wind = false;

  if (version >= 122) {
    int32_t v = 0;
    ifile >> v;
    FVI_always_check_ceiling = (v != 0);
  } else
    FVI_always_check_ceiling = false;

  if (version >= 127)
    ifile >> Ceiling_height;
  else
    Ceiling_height = MAX_TERRAIN_HEIGHT;
}

static void LL_WriteInfo(posix_ostream &ofile) {
  ofile << Level_info.name;
  ofile << Level_info.designer;
  ofile << Level_info.copyright;
  ofile << Level_info.notes;

  ofile << Gravity_strength;
  int32_t check = (int)FVI_always_check_ceiling;
  ofile << check;
  ofile << Ceiling_height;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

// Reads the tail of a legacy object record (all fields after pos/orient) for
// file versions whose layout predates the current canonical serialization.
// This is a faithful port of the engine's ReadObject tail so that pre-119
// levels (integer soundsource indices, parent_handle ints, OSIRIS-1 script
// stubs) keep every record aligned.  The canonical (post-119) layout is read
// by the object stream operators directly.
static void LL_ReadObjectLegacyTail(posix_istream &ifile, object &obj, int version) {
  int8_t c = 0;
  ifile >> c; obj.contains_type = c;
  ifile >> c; obj.contains_id = c;
  ifile >> c; obj.contains_count = c;
  ifile >> obj.lifeleft;

  if (version >= 65 && version < 111)
    ifile >> obj.parent_handle;

  // Pre-119 files store the sound-source sound as a raw index, not a name.
  if (obj.type == OBJ_SOUNDSOURCE) {
    ifile >> obj.ctype.soundsource_info.sound_index;
    ifile >> obj.ctype.soundsource_info.volume;
  }

  // Script / module names are length-prefixed (uint8) since version 105.
  if (version >= 105) {
    uint8_t nlen = 0;
    ifile >> nlen;
    obj.custom_default_script_name.assign(nlen, '\0');
    if (nlen) {
      ifile.read(obj.custom_default_script_name.data(), nlen);
    }
    uint8_t mlen = 0;
    ifile >> mlen;
    obj.custom_default_module_name.assign(mlen, '\0');
    if (mlen) {
      ifile.read(obj.custom_default_module_name.data(), mlen);
    }
  }

  // Obsolete OSIRIS-1 script info (dropped at LEVEL_FILE_OSIRIS1DEAD, 97).
  if (version < LEVEL_FILE_OSIRIS1DEAD) {
    if (version < LEVEL_FILE_SCRIPTNAMES)
      { int32_t pad = 0; ifile >> pad; }
    else
      { std::string pad; ifile >> pad; }
    if (version >= LEVEL_FILE_SCRIPTPARMS) {
      int16_t s = 0; ifile >> s;
      for (int i = 0; i < s; i++) {
        int8_t stype = 0; ifile >> stype;
        if (stype == PARMTYPE_NUMBER || stype == PARMTYPE_REF) { float pad = 0.0f; ifile >> pad; }
        else if (stype == PARMTYPE_VECTOR) { vector3 pad; ifile >> pad; }
        else Q_ASSERT(false);
      }
    }
    if (version >= LEVEL_FILE_SCRIPTCHECK) { uint8_t pad = 0; ifile >> pad; }
  }

  // Embedded per-object lightmap data.
  int lmdata = 0;
  if (version >= 35) { uint8_t lm = 0; ifile >> lm; lmdata = lm; }
  if (lmdata) {
    uint8_t num_models = 0; ifile >> num_models;
    obj.lm_object.num_models = num_models;
    for (int m = 0; m < num_models; m++) {
      int16_t num_faces = 0; ifile >> num_faces;
      obj.lm_object.num_faces.push_back(num_faces);
      auto &faces = obj.lm_object.lightmap_faces.emplace_back();
      faces.resize(num_faces);
      for (int t = 0; t < num_faces; t++) {
        lightmap_object_face &f = faces[t];
        ifile >> f.lmi_handle;
        if (version <= 88) { uint8_t pad2; ifile >> pad2; ifile >> pad2; ifile >> pad2; ifile >> pad2; }
        if (version >= 58) {
          if (version <= 59) { vector3 pad; ifile >> pad; }
          ifile >> f.rvec >> f.uvec;
        } else { f.rvec = {}; f.uvec = {}; f.uvec.y() = 1; }
        uint8_t nv = 0; ifile >> nv;
        f.num_verts = nv;
        f.u2 = new float[nv]; f.v2 = new float[nv];
        for (int k = 0; k < nv; k++) ifile >> f.u2[k] >> f.v2[k];
      }
    }
    obj.lm_object.used = 1;
  }
}

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
    if (version > LEVEL_FILE_VERSION || version < LEVEL_FILE_OLDEST_COMPATIBLE_VERSION) {
      ifile.close();
      return false;
    }

    while (!ifile.eof()) {
      char chunk_name[4];
      ifile.read(chunk_name, 4);
      long chunk_start = static_cast<long>(ifile.tell());
      int32_t chunk_size32 = 0;
      ifile >> chunk_size32;
      int chunk_size = chunk_size32;

      if (IsChunk(chunk_name, "ROOM")) {
        int32_t num = 0;
        ifile >> num;
        int num_rooms = num;
        if (version >= 85) {
          int32_t t;
          ifile >> t; // nverts
          ifile >> t; // nfaces
          ifile >> t; // nfaceverts
          ifile >> t; // nportals
        }
        int roomnum = 0;
        for (int i = 0; i < num_rooms; i++) {
          if (version >= 96) {
            int16_t room = 0;
            ifile >> room;
            roomnum = room;
          } else
            roomnum = i;
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
          LL_ReadVector(ifile, Rooms[roomnum].wind);
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
          int objnum = (version >= 45) ? (handle & HANDLE_OBJNUM_MASK) : i;
          if (objnum < 0 || objnum >= MAX_OBJECTS)
            continue;
          object *obj = &Objects[objnum];
          // Value-initialise (NOT memset: object contains std::string members).
          *obj = object{};

          // Version >= 119 uses the current canonical record: the object
          // stream operator reads every field (through the embedded lightmap
          // block) exactly as the engine's ReadObject does, keeping the
          // record aligned.
          if (version >= 119) {
            ifile >> *obj;
          } else {
            // Older layouts read the placement prefix by hand then run the
            // legacy tail (see LL_ReadObjectLegacyTail) for the remainder.

            int8_t ty = 0;
            ifile >> ty;
            obj->type = ty;
            obj->id = 0;
            if (version >= 34) {
              ifile >> obj->id;
            } else {
              uint8_t idb = 0;
              ifile >> idb;
              obj->id = idb;
            }

            // Object name (engine writes a null-terminated string since v95).
            if (version >= 95) {
              std::string nam;
              ifile >> nam;
              obj->name = nam;
            }

            // Object flags (int32 since v101, uint16 before that).
            obj->flags = 0;
            if (version >= 101) {
              int32_t flags32 = 0;
              ifile >> flags32;
              obj->flags = static_cast<uint32_t>(flags32);
            } else {
              uint16_t flags16 = 0;
              ifile >> flags16;
              obj->flags = flags16;
            }

            // Door shields (engine writes a short since v109).
            if (obj->type == OBJ_DOOR && version >= 109) {
              int16_t shields = 0;
              ifile >> shields;
              obj->shields = static_cast<float>(shields);
            }

            int32_t roomnum3 = 0;
            ifile >> roomnum3;
            obj->roomnum = roomnum3;
            LL_ReadVector(ifile, obj->pos);
            LL_ReadMatrix(ifile, obj->orient);
            LL_ReadObjectLegacyTail(ifile, *obj, version);
          }

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
            LL_WriteVector(out, Rooms[i].wind);
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
