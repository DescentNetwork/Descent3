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
//   OBJS  - minimal object placement data (type/id/roomnum/pos/orient)
//   TRIG  - trigger table
//   INFO  - level name/designer/copyright/notes + level physics params
//   PSTR  - skipped (player starts are editor-only; ignored on load)
//
// All other chunks are skipped by seeking to chunk_start + chunk_size.
// SaveLevel writes those chunks in the same format so LoadLevel round-trips.

#include "LoadLevel.h"
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

#include <cstring>
#include <cstdio>

#define LL_TAG "D3LV"

static bool IsChunk(const char *chunk_name, const char *id) { return chunk_name[0] == id[0] && chunk_name[1] == id[1] && chunk_name[2] == id[2] && chunk_name[3] == id[3]; }

// A level's faces reference textures by an index into the level's own
// texture-name list (TXNM chunk).  texture_xlate[levelIdx] maps that index to
// the corresponding slot in the global GameTextures[] (matched by name).
// Reset to identity before each LoadLevel so the raw index survives when no
// TXNM list is present (e.g. our own saved files).
static int texture_xlate[MAX_TEXTURES];

static int LL_FindTextureName(const char *name) {
  if (name == nullptr)
    return -1;
  for (int i = 0; i < Num_textures; i++) {
    if (stricmp(GameTextures[i].name, name) == 0)
      return i;
  }
  return -1;
}

// Reads a TXNM chunk: an int32 count, then that many null-terminated texture
// names.  Builds texture_xlate[] so faces can map level-texture-index → global
// GameTextures index.
static void LL_ReadTextureList(CFILE *ifile, int chunk_size) {
  int n = cf_ReadInt(ifile);
  long end = cftell(ifile) + (chunk_size - 4);
  for (int i = 0; i < n; i++) {
    char name[PAGENAME_LEN];
    cf_ReadString(name, sizeof(name), ifile);
    int g = LL_FindTextureName(name);
    if (g < 0)
      g = 0;
    texture_xlate[i] = g;
    if (cftell(ifile) >= end)
      break;
  }
}

// Read/write a vector (3 floats)
static void LL_ReadVector(CFILE *f, vector &v) {
  float x = cf_ReadFloat(f);
  float y = cf_ReadFloat(f);
  float z = cf_ReadFloat(f);
  v = vector{x, y, z};
}
static void LL_WriteVector(CFILE *f, const vector &v) {
  cf_WriteFloat(f, v.x());
  cf_WriteFloat(f, v.y());
  cf_WriteFloat(f, v.z());
}

// Read/write a matrix (rvec, uvec, fvec each a vector)
static void LL_ReadMatrix(CFILE *f, matrix &m) {
  LL_ReadVector(f, m.rvec);
  LL_ReadVector(f, m.uvec);
  LL_ReadVector(f, m.fvec);
}
static void LL_WriteMatrix(CFILE *f, const matrix &m) {
  LL_WriteVector(f, m.rvec);
  LL_WriteVector(f, m.uvec);
  LL_WriteVector(f, m.fvec);
}

// Writes a chunk header (4-char name + size placeholder), returns the position
// of the size field so EndChunk can seek back and patch it.
static int LL_StartChunk(CFILE *ofile, const char *chunk_name) {
  cf_WriteBytes((uint8_t *)chunk_name, 4, ofile);
  int chunk_start_pos = cftell(ofile);
  cf_WriteInt(ofile, 0); // placeholder
  return chunk_start_pos;
}

static void LL_EndChunk(CFILE *ofile, int chunk_start_pos) {
  int save_pos = cftell(ofile);
  int len = save_pos - chunk_start_pos;
  while (len & 3) { // pad to multiple of four
    cf_WriteByte(ofile, 0);
    len++;
    save_pos++;
  }
  cfseek(ofile, chunk_start_pos, SEEK_SET);
  cf_WriteInt(ofile, len);
  cfseek(ofile, save_pos, SEEK_SET);
}

// RLE byte compression used by the engine inside ROOM for volume lights.
static void LL_ReadCompressionByte(CFILE *fp, uint8_t *vals, int total) {
  int count = 0;
  uint8_t compressed = cf_ReadByte(fp);
  if (compressed == 0) {
    for (int i = 0; i < total; i++)
      vals[i] = cf_ReadByte(fp);
    return;
  }
  while (count != total) {
    uint8_t command = cf_ReadByte(fp);
    if (command == 0) { // next byte is raw
      vals[count] = cf_ReadByte(fp);
      count++;
    } else if (command >= 2 && command <= 250) {
      uint8_t height = cf_ReadByte(fp);
      for (int k = 0; k < command; k++) {
        vals[count] = height;
        count++;
      }
    } else {
      cfclose(fp);
      throw std::runtime_error("bad compression run");
    }
  }
}

static void LL_WriteCompressionByte(CFILE *fp, uint8_t *vals, int total) {
  cf_WriteByte(fp, 0); // no compression
  for (int i = 0; i < total; i++)
    cf_WriteByte(fp, vals[i]);
}

static int LL_ReadFace(CFILE *ifile, face *fp, int version) {
  int nverts = cf_ReadByte(ifile);
  int i;

  InitRoomFace(fp, nverts);

  for (i = 0; i < fp->num_verts; i++)
    fp->face_verts[i] = cf_ReadShort(ifile);

  int alphaed = 0;
  for (i = 0; i < fp->num_verts; i++) {
    fp->face_uvls[i].u = cf_ReadFloat(ifile);
    fp->face_uvls[i].v = cf_ReadFloat(ifile);

    if (version < 56) {
      for (int k = 0; k < 4; k++)
        cf_ReadFloat(ifile);
    }

    if (version >= 21) {
      if (version < 61)
        fp->face_uvls[i].alpha = (uint8_t)(cf_ReadFloat(ifile) * 255.0f);
      else
        fp->face_uvls[i].alpha = cf_ReadByte(ifile);
    } else
      fp->face_uvls[i].alpha = 255;

    if (fp->face_uvls[i].alpha != 255)
      alphaed = 1;
  }

  if (version < 27)
    fp->flags = cf_ReadByte(ifile);
  else
    fp->flags = (uint16_t)cf_ReadShort(ifile);

  if (alphaed)
    fp->flags |= FF_VERTEX_ALPHA;
  else
    fp->flags &= ~FF_VERTEX_ALPHA;

  if (version >= 23)
    fp->portal_num = cf_ReadByte(ifile);
  else
    fp->portal_num = 0;

  // Map the level-texture-index to the global GameTextures[] slot via the
  // TXNM list (identity when no list was present).  Clamp to safe range.
  int16_t tmap = cf_ReadShort(ifile);
  fp->tmap = (tmap >= 0 && tmap < MAX_TEXTURES) ? texture_xlate[tmap] : 0;

  if ((fp->flags & FF_LIGHTMAP) && (version >= 19)) {
    if (version <= 29) {
      uint8_t w = cf_ReadByte(ifile);
      uint8_t h = cf_ReadByte(ifile);
      for (i = 0; i < w * h; i++)
        cf_ReadShort(ifile);
      fp->flags &= ~FF_LIGHTMAP;
    } else {
      fp->lmi_handle = (uint16_t)cf_ReadShort(ifile);
      if (version <= 88) {
        for (int k = 0; k < 4; k++)
          cf_ReadByte(ifile);
      }
      for (i = 0; i < fp->num_verts; i++) {
        fp->face_uvls[i].u2 = cf_ReadFloat(ifile);
        fp->face_uvls[i].v2 = cf_ReadFloat(ifile);
      }
    }
  }

  if (version >= 22 && version <= 29) {
    vector trash;
    LL_ReadVector(ifile, trash);
  }
  if (version >= 40 && version <= 60) {
    cf_ReadShort(ifile);
    cf_ReadShort(ifile);
  }

  if (version >= 50) {
    fp->light_multiple = cf_ReadByte(ifile);
    if (fp->light_multiple == 186)
      fp->light_multiple = 4;
    if (version <= 52)
      fp->light_multiple *= 4;
  } else
    fp->light_multiple = 4;

  // Special (specular) faces are not modeled by the mini.  Consume the block
  // (if present) to keep real-level alignment; we never write one ourselves.
  if (version >= 71) {
    uint8_t special = cf_ReadByte(ifile);
    if (special) {
      if (version < 77) {
        cf_ReadByte(ifile);
        vector trash;
        LL_ReadVector(ifile, trash);
        cf_ReadShort(ifile);
      } else {
        cf_ReadByte(ifile); // type
        uint8_t num = cf_ReadByte(ifile);
        int smooth = 0;
        int num_smooth = 0;
        if (version >= 117) {
          smooth = cf_ReadByte(ifile);
          if (smooth)
            num_smooth = cf_ReadByte(ifile);
        }
        for (i = 0; i < num; i++) {
          vector trash;
          LL_ReadVector(ifile, trash);
          cf_ReadShort(ifile);
        }
        if (smooth) {
          for (i = 0; i < num_smooth; i++) {
            vector trash;
            LL_ReadVector(ifile, trash);
          }
        }
      }
    }
  }

  return 1;
}

static int LL_WriteFace(CFILE *ofile, face *fp) {
  int i;
  cf_WriteByte(ofile, (int8_t)fp->num_verts);
  for (i = 0; i < fp->num_verts; i++)
    cf_WriteShort(ofile, fp->face_verts[i]);

  fp->flags &= ~FF_VERTEX_ALPHA;
  for (i = 0; i < fp->num_verts; i++) {
    cf_WriteFloat(ofile, fp->face_uvls[i].u);
    cf_WriteFloat(ofile, fp->face_uvls[i].v);
    cf_WriteByte(ofile, fp->face_uvls[i].alpha);
    if (fp->face_uvls[i].alpha != 255)
      fp->flags |= FF_VERTEX_ALPHA;
  }

  cf_WriteShort(ofile, (int16_t)fp->flags);
  cf_WriteByte(ofile, fp->portal_num);
  cf_WriteShort(ofile, fp->tmap);

  if (fp->flags & FF_LIGHTMAP) {
    cf_WriteShort(ofile, (int16_t)fp->lmi_handle);
    for (i = 0; i < fp->num_verts; i++) {
      cf_WriteFloat(ofile, fp->face_uvls[i].u2);
      cf_WriteFloat(ofile, fp->face_uvls[i].v2);
    }
  }

  if (fp->light_multiple == 186)
    fp->light_multiple = 4;
  cf_WriteByte(ofile, (int8_t)fp->light_multiple);

  // No specular support: emit the "no special face" byte.
  cf_WriteByte(ofile, 0);

  return 1;
}

static int LL_ReadPortal(CFILE *ifile, portal *pp, int version) {
  pp->flags = cf_ReadInt(ifile);
  if (version < 103)
    pp->flags &= ~(1 << 2);

  if (version < 80) {
    int num_verts = cf_ReadShort(ifile);
    for (int i = 0; i < num_verts; i++)
      cf_ReadShort(ifile);
    cf_ReadShort(ifile); // num_faces, assumed 1
  }

  pp->portal_face = cf_ReadShort(ifile);
  pp->croom = cf_ReadInt(ifile);
  pp->cportal = cf_ReadInt(ifile);

  if (version >= 123)
    pp->bnode_index = cf_ReadShort(ifile);
  else
    pp->bnode_index = -1;

  if (version >= 63)
    LL_ReadVector(ifile, pp->path_pnt);

  if (version >= 100)
    pp->combine_master = cf_ReadInt(ifile);

  return 1;
}

static int LL_WritePortal(CFILE *ofile, portal *pp) {
  cf_WriteInt(ofile, pp->flags);
  cf_WriteShort(ofile, pp->portal_face);
  cf_WriteInt(ofile, pp->croom);
  cf_WriteInt(ofile, pp->cportal);
  cf_WriteShort(ofile, pp->bnode_index);
  LL_WriteVector(ofile, pp->path_pnt);
  cf_WriteInt(ofile, pp->combine_master);
  return 1;
}

// Reads a room from a disk file.  Mirrors the engine field-by-field for
// versions >= 127 (the mini writes/reads flash these fields).
static int LL_ReadRoom(CFILE *ifile, room *rp, int version) {
  int nverts = cf_ReadInt(ifile);
  int nfaces = cf_ReadInt(ifile);
  int nportals = cf_ReadInt(ifile);

  int i;
  InitRoom(rp, nverts, nfaces, nportals);

  rp->name = nullptr;
  if (version >= 96) {
    char tempname[ROOM_NAME_LEN + 1];
    cf_ReadString(tempname, sizeof(tempname), ifile);
    if (std::strlen(tempname))
      rp->name = mem_strdup(tempname);
  }

  if (version >= 63)
    LL_ReadVector(ifile, rp->path_pnt);

  for (i = 0; i < rp->num_verts; i++) {
    LL_ReadVector(ifile, rp->verts[i]);
    if (version >= 52 && version <= 67) {
      cf_ReadByte(ifile);
      cf_ReadByte(ifile);
    } else if (version >= 68 && version < 71) {
      vector trash;
      LL_ReadVector(ifile, trash);
      cf_ReadShort(ifile);
    }
  }

  for (i = 0; i < rp->num_faces; i++) {
    LL_ReadFace(ifile, &rp->faces[i], version);
    ComputeFaceNormal(rp, i);
  }

  for (i = 0; i < rp->num_portals; i++)
    LL_ReadPortal(ifile, &rp->portals[i], version);

  rp->flags = cf_ReadInt(ifile);
  if (version < 29)
    cf_ReadFloat(ifile);

  if (version >= 68) {
    rp->pulse_time = cf_ReadByte(ifile);
    rp->pulse_offset = cf_ReadByte(ifile);
  }

  if (version >= 79)
    rp->mirror_face = cf_ReadShort(ifile);
  else
    rp->mirror_face = -1;

  if (rp->flags & RF_DOOR) {
    // The mini does not model 3d doors, but we must consume the bytes so the
    // rest of the chunk stays aligned. (Engine writes these as:
    // flags(byte) keys(byte) doornum(int) position(float) for version >= 106.)
    cf_ReadByte(ifile); // flags
    cf_ReadByte(ifile); // keys_needed
    cf_ReadInt(ifile);  // doornum
    cf_ReadFloat(ifile); // position (version >= 106 only)
  }

  if (version >= 67) {
    if (cf_ReadByte(ifile) == 1) {
      int w = cf_ReadInt(ifile);
      int h = cf_ReadInt(ifile);
      int d = cf_ReadInt(ifile);
      int size = w * h * d;
      if (size) {
        rp->volume_lights = mem_rmalloc<uint8_t>(size);
        LL_ReadCompressionByte(ifile, rp->volume_lights, size);
      }
      rp->volume_width = (int16_t)w;
      rp->volume_height = (int16_t)h;
      rp->volume_depth = (int16_t)d;
    }
  }

  if (version >= 73) {
    rp->fog_depth = cf_ReadFloat(ifile);
    rp->fog_r = cf_ReadFloat(ifile);
    rp->fog_g = cf_ReadFloat(ifile);
    rp->fog_b = cf_ReadFloat(ifile);
  }

  if (version >= 78) {
    char tbuf[64];
    cf_ReadString(tbuf, sizeof(tbuf), ifile);
    // Mini has no ambient sound page; leave ambient_sound at InitRoom's -1.
  }

  rp->env_reverb = (version >= 98) ? ((uint8_t)cf_ReadByte(ifile)) : 0;

  if (version >= 108) {
    rp->damage = cf_ReadFloat(ifile);
    rp->damage_type = cf_ReadByte(ifile);
  }

  return 1;
}

static int LL_WriteRoom(CFILE *ofile, room *rp) {
  int i;

  cf_WriteInt(ofile, rp->num_verts);
  cf_WriteInt(ofile, rp->num_faces);
  cf_WriteInt(ofile, rp->num_portals);

  cf_WriteString(ofile, rp->name ? rp->name : "");
  LL_WriteVector(ofile, rp->path_pnt);

  for (i = 0; i < rp->num_verts; i++)
    LL_WriteVector(ofile, rp->verts[i]);

  for (i = 0; i < rp->num_faces; i++)
    LL_WriteFace(ofile, &rp->faces[i]);

  for (i = 0; i < rp->num_portals; i++)
    LL_WritePortal(ofile, &rp->portals[i]);

  cf_WriteInt(ofile, rp->flags);

  cf_WriteByte(ofile, (int8_t)rp->pulse_time);
  cf_WriteByte(ofile, (int8_t)rp->pulse_offset);

  cf_WriteShort(ofile, rp->mirror_face);

  if (rp->flags & RF_DOOR) {
    // No doorway data in the mini; write neutral values the reader consumes.
    cf_WriteByte(ofile, 0);  // flags
    cf_WriteByte(ofile, 0);  // keys
    cf_WriteInt(ofile, 0);   // door
    cf_WriteFloat(ofile, 0.0f); // position
  }

  if (!rp->volume_lights)
    cf_WriteByte(ofile, 0);
  else {
    cf_WriteByte(ofile, 1);
    cf_WriteInt(ofile, rp->volume_width);
    cf_WriteInt(ofile, rp->volume_height);
    cf_WriteInt(ofile, rp->volume_depth);
    LL_WriteCompressionByte(ofile, rp->volume_lights, rp->volume_width * rp->volume_height * rp->volume_depth);
  }

  cf_WriteFloat(ofile, rp->fog_depth);
  cf_WriteFloat(ofile, rp->fog_r);
  cf_WriteFloat(ofile, rp->fog_g);
  cf_WriteFloat(ofile, rp->fog_b);

  cf_WriteString(ofile, ""); // ambient sound name (unused in mini)

  cf_WriteByte(ofile, (int8_t)rp->env_reverb);

  cf_WriteFloat(ofile, rp->damage);
  cf_WriteByte(ofile, (int8_t)rp->damage_type);

  return 1;
}

static void LL_ReadInfo(CFILE *ifile, int version) {
  std::strcpy(Level_info.name, "Unnamed");
  std::strcpy(Level_info.designer, "Anonymous");
  std::strcpy(Level_info.copyright, "");
  std::strcpy(Level_info.notes, "");

  cf_ReadString(Level_info.name, sizeof(Level_info.name), ifile);
  cf_ReadString(Level_info.designer, sizeof(Level_info.designer), ifile);
  cf_ReadString(Level_info.copyright, sizeof(Level_info.copyright), ifile);
  cf_ReadString(Level_info.notes, sizeof(Level_info.notes), ifile);

  if (version >= 83)
    Gravity_strength = cf_ReadFloat(ifile);
  else
    Gravity_strength = -32.2f;

  if (version >= 131)
    Level_powerups_ignore_wind = (cf_ReadInt(ifile) != 0);
  else
    Level_powerups_ignore_wind = false;

  if (version >= 122)
    FVI_always_check_ceiling = (cf_ReadInt(ifile) != 0);
  else
    FVI_always_check_ceiling = false;

  if (version >= 127)
    Ceiling_height = cf_ReadFloat(ifile);
  else
    Ceiling_height = MAX_TERRAIN_HEIGHT;
}

static void LL_WriteInfo(CFILE *ofile) {
  cf_WriteString(ofile, Level_info.name);
  cf_WriteString(ofile, Level_info.designer);
  cf_WriteString(ofile, Level_info.copyright);
  cf_WriteString(ofile, Level_info.notes);
  cf_WriteFloat(ofile, Gravity_strength);
  cf_WriteInt(ofile, (int)FVI_always_check_ceiling);
  cf_WriteFloat(ofile, Ceiling_height);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool LoadLevel(char *filename, void (*cb_fn)(const char *, int, int)) {
  CFILE *ifile = cfopen(filename, "rb");
  if (!ifile)
    return false;

  // Default texture mapping is identity so faces that precede a TXNM chunk
  // (or files without one) still index GameTextures[] directly.
  for (int i = 0; i < MAX_TEXTURES; i++)
    texture_xlate[i] = i;

  FreeAllRooms();

  // Reset the object table.
  for (int i = 0; i < MAX_OBJECTS; i++) {
    std::memset(&Objects[i], 0, sizeof(object));
    Objects[i].type = OBJ_NONE;
    Objects[i].handle = i;
  }
  Highest_object_index = -1;
  Num_triggers = 0;

  const uint32_t filelen = cfilelength(ifile);

  try {
    char tag[4];
    cf_ReadBytes((uint8_t *)tag, 4, ifile);
    if (tag[0] != 'D' || tag[1] != '3' || tag[2] != 'L' || tag[3] != 'V') {
      cfclose(ifile);
      return false;
    }
    int version = cf_ReadInt(ifile);
    if (version > LEVEL_FILE_VERSION || version < LEVEL_FILE_OLDEST_COMPATIBLE_VERSION) {
      cfclose(ifile);
      return false;
    }

    while (!cfeof(ifile)) {
      char chunk_name[4];
      cf_ReadBytes((uint8_t *)chunk_name, 4, ifile);
      long chunk_start = cftell(ifile);
      int chunk_size = cf_ReadInt(ifile);

      if (IsChunk(chunk_name, "ROOM")) {
        int num_rooms = cf_ReadInt(ifile);
        if (version >= 85) {
          cf_ReadInt(ifile); // nverts
          cf_ReadInt(ifile); // nfaces
          cf_ReadInt(ifile); // nfaceverts
          cf_ReadInt(ifile); // nportals
        }
        int roomnum = 0;
        for (int i = 0; i < num_rooms; i++) {
          roomnum = (version >= 96) ? cf_ReadShort(ifile) : i;
          LL_ReadRoom(ifile, &Rooms[roomnum], version);
        }
        Highest_room_index = roomnum;
        if (Highest_room_index < 0 || Highest_room_index >= MAX_ROOMS)
          Highest_room_index = MAX_ROOMS - 1;
      } else if (IsChunk(chunk_name, "TXNM")) {
        // Level-local texture name list.  Builds the level→global texture
        // index so faces (ReadFace's raw tmap index) resolve correctly.
        LL_ReadTextureList(ifile, chunk_size);
      } else if (IsChunk(chunk_name, "RWND")) {
        int nrooms = cf_ReadInt(ifile);
        for (int i = 0; i < nrooms; i++) {
          int roomnum = cf_ReadShort(ifile);
          LL_ReadVector(ifile, Rooms[roomnum].wind);
        }
      } else if (IsChunk(chunk_name, "OBJS")) {
        int n = cf_ReadInt(ifile);
        for (int i = 0; i < n; i++) {
          int objnum = cf_ReadInt(ifile);
          if (objnum < 0 || objnum >= MAX_OBJECTS)
            continue;
          object *obj = &Objects[objnum];
          std::memset(obj, 0, sizeof(object));
          obj->type = cf_ReadByte(ifile);
          obj->id = cf_ReadShort(ifile);
          obj->roomnum = cf_ReadInt(ifile);
          LL_ReadVector(ifile, obj->pos);
          LL_ReadMatrix(ifile, obj->orient);
          if (objnum > Highest_object_index)
            Highest_object_index = objnum;
        }
      } else if (IsChunk(chunk_name, "TRIG")) {
        Num_triggers = cf_ReadInt(ifile);
        if (Num_triggers > 500)
          Num_triggers = 500;
        for (int i = 0; i < Num_triggers; i++) {
          trigger *tp = &Triggers[i];
          std::memset(tp, 0, sizeof(trigger));
          cf_ReadString(tp->name, sizeof(tp->name), ifile);
          tp->roomnum = cf_ReadShort(ifile);
          tp->facenum = cf_ReadShort(ifile);
          tp->flags = cf_ReadShort(ifile);
          tp->activator = cf_ReadShort(ifile);
        }
      } else if (IsChunk(chunk_name, "INFO")) {
        LL_ReadInfo(ifile, version);
      } else {
        // unknown / skipped chunk (TXNM, RWND is separate, PSTR, lightmaps, ...)
      }

      // Seek past any leftover body bytes to the next chunk boundary.
      long body_end = chunk_start + chunk_size;
      if (cftell(ifile) != body_end)
        cfseek(ifile, body_end, SEEK_SET);

      if (cb_fn)
        cb_fn(chunk_name, chunk_size, (int)filelen);
    }
  } catch (cfile_error *) {
    cfclose(ifile);
    return false;
  }

  cfclose(ifile);

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

bool SaveLevel(char *filename, bool f_save_room_AABB) {
  (void)f_save_room_AABB;
  CFILE *ofile = cfopen(filename, "wb");
  if (!ofile)
    return false;

  try {
    cf_WriteBytes((uint8_t *)"D3LV", 4, ofile);
    cf_WriteInt(ofile, LEVEL_FILE_VERSION);

    // TXNM: no texture names; write an empty list.
    {
      int start = LL_StartChunk(ofile, "TXNM");
      cf_WriteInt(ofile, 0);
      LL_EndChunk(ofile, start);
    }

    // ROOM
    {
      int start = LL_StartChunk(ofile, CHUNK_ROOMS);
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
      cf_WriteInt(ofile, nrooms);
      cf_WriteInt(ofile, nverts);
      cf_WriteInt(ofile, nfaces);
      cf_WriteInt(ofile, nfaceverts);
      cf_WriteInt(ofile, nportals);
      for (int i = 0; i <= Highest_room_index; i++) {
        if (!Rooms[i].used)
          continue;
        cf_WriteShort(ofile, (int16_t)i);
        LL_WriteRoom(ofile, &Rooms[i]);
      }
      LL_EndChunk(ofile, start);
    }

    // RWND (room wind)
    {
      int nwind = 0;
      for (int i = 0; i <= Highest_room_index; i++)
        if (Rooms[i].used && (Rooms[i].wind.x() != 0.0f || Rooms[i].wind.y() != 0.0f || Rooms[i].wind.z() != 0.0f))
          nwind++;
      if (nwind) {
        int start = LL_StartChunk(ofile, CHUNK_ROOM_WIND);
        cf_WriteInt(ofile, nwind);
        for (int i = 0; i <= Highest_room_index; i++) {
          if (Rooms[i].used && (Rooms[i].wind.x() != 0.0f || Rooms[i].wind.y() != 0.0f || Rooms[i].wind.z() != 0.0f)) {
            cf_WriteShort(ofile, (int16_t)i);
            LL_WriteVector(ofile, Rooms[i].wind);
          }
        }
        LL_EndChunk(ofile, start);
      }
    }

    // OBJS
    {
      int start = LL_StartChunk(ofile, CHUNK_OBJECTS);
      int count = 0;
      for (int i = 0; i <= Highest_object_index; i++)
        if (Objects[i].type != OBJ_NONE)
          count++;
      cf_WriteInt(ofile, count);
      for (int i = 0; i <= Highest_object_index; i++) {
        if (Objects[i].type == OBJ_NONE)
          continue;
        cf_WriteInt(ofile, i);
        cf_WriteByte(ofile, (int8_t)Objects[i].type);
        cf_WriteShort(ofile, (int16_t)Objects[i].id);
        cf_WriteInt(ofile, Objects[i].roomnum);
        LL_WriteVector(ofile, Objects[i].pos);
        LL_WriteMatrix(ofile, Objects[i].orient);
      }
      LL_EndChunk(ofile, start);
    }

    // TRIG
    {
      int start = LL_StartChunk(ofile, CHUNK_TRIGGERS);
      cf_WriteInt(ofile, Num_triggers);
      for (int i = 0; i < Num_triggers; i++) {
        cf_WriteString(ofile, Triggers[i].name);
        cf_WriteShort(ofile, (int16_t)Triggers[i].roomnum);
        cf_WriteShort(ofile, (int16_t)Triggers[i].facenum);
        cf_WriteShort(ofile, Triggers[i].flags);
        cf_WriteShort(ofile, Triggers[i].activator);
      }
      LL_EndChunk(ofile, start);
    }

    // INFO
    {
      int start = LL_StartChunk(ofile, CHUNK_LEVEL_INFO);
      LL_WriteInfo(ofile);
      LL_EndChunk(ofile, start);
    }
  } catch (cfile_error *&throw_ptr) {
    (void)throw_ptr;
    cfclose(ofile);
    return false;
  } catch (std::exception &) {
    cfclose(ofile);
    return false;
  }

  cfclose(ofile);
  return true;
}