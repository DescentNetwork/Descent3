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
 */

// Ported from the full engine's manage/ module (manage.cpp, generic.cpp,
// shippage.cpp, weaponpage.cpp, texpage.cpp, soundpage.cpp, doorpage.cpp,
// megapage.cpp).  The per-page *readers* now live in their own translation
// units (genericpage.cpp / weaponpage.cpp / texpage.cpp / soundpage.cpp /
// doorpage.cpp / shippage.cpp / megapage.cpp).  This file keeps the top-level
// loader that parses Table.gam (inside d3.hog) and hands each page body to the
// corresponding reader, filling the mini editor's global arrays without
// touching the game renderer (no model/bitmap/sound/procedural loading).  This
// file links only against Qt + OpenGL + the mini cfile implementation.

#include "gamedata_loader.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>

#include "posix_helpers.h"
#include <hog2_format.h>

#include "manage.h"
#include "aistruct.h"  // t_ai_info
#include "aistruct_external.h"
#include "mem.h"     // mem_rmalloc
#include "log.h"     // LOG_ERROR
#include "objinfo.h"
#include "ship.h"
#include "weapon.h"
#include "gametexture.h"
#include "door.h"
#include "ssl_lib.h" // sound_info
#include "object_external_struct.h" // physics_info, light_info, MAX_OBJECTS
#include "genericpage.h"
#include "shippage.h"
#include "weaponpage.h"
#include "texpage.h"
#include "soundpage.h"
#include "doorpage.h"
#include "megapage.h"
#include "megacell.h"
#include "iff.h"
#include "bitmap.h"

#include "string_helpers.h"

// The mini manage.h declares this extern but no mini source defines it.  Net
// tables never use the old command-based method, so it is always 0 here.  A
// single translation-unit definition satisfies the extern (ODR) requirement.
int Old_table_method = 0;

// The mini tree defines `object_info Object_info[MAX_OBJECTS];` in stubs.cpp
// but no header declares the count; declare it here so the loader can track it.
extern int Num_objects;

//-----------------------------------------------------------------------------
// Top-level loader
//-----------------------------------------------------------------------------

// Discards exactly `count` bytes from an open page file (used to skip pages
// whose bodies are not consumed by a reader, e.g. game-only ROBOT/POWERUP
// pages or pages that overflow a global array).
static void discardBytes(posix_istream &infile, int count) {
  uint8_t buf[4096];
  while (count > 0) {
    int take = count > (int)sizeof(buf) ? (int)sizeof(buf) : count;
    infile.read(buf, take);
    if (infile.eof())
      break;
    count -= take;
  }
}

// Locates `img` inside the open HOG `archive`, reads its full payload from the
// still-open HOG stream `hogin`, and hands the bytes to the decoder via
// bm_LoadBitmapFromMemory (fmemopen posix_istream).  Returns the bitmap handle,
// or -1 if the image is not in the Hog or fails to decode.
static int loadTextureFromArchive(hog2::archive_t &archive, posix_istream &hogin, const std::string &img, int format) {
  auto entry = archive.end();
  const std::string needle = lowercase(img);
  for (auto it = archive.begin(); it != archive.end(); ++it) {
    if (lowercase(it->name.string()) == needle) {
      entry = it;
      break;
    }
  }
  if (entry == archive.end())
    return -1;

  const size_t off = archive.fileOffset(entry);
  const size_t len = entry->len;
  std::vector<uint8_t> buf(len);
  hogin.seek(off, std::ios_base::beg);
  hogin.read(buf.data(), len);

  return bm_LoadBitmapFromMemory(buf.data(), buf.size(), img.c_str(), format, 0);
}

bool loadGameDataTable(const std::filesystem::path& d3HogPath) {
  // Table.gam and every texture image are pulled straight from the HOG payload
  // below using posix_istream + hog2::archive_t (independent of CFILE/cfopen).

  // Read the whole HOG into memory via posix_stream.
  posix_istream hogin;
  if (!hogin.open(d3HogPath, std::ios_base::in)) {
    return false;
  }

  hog2::archive_t archive;
  try {
    hogin >> archive;
  } catch (const std::invalid_argument &) {
    hogin.close();
    return false;
  }

  // Locate the table.gam entry so its payload offset/length can be computed.
  auto entry = archive.end();
  for (auto it = archive.begin(); it != archive.end(); ++it) {
    if (lowercase(std::string(it->name.string())) == "table.gam") {
      entry = it;
      break;
    }
  }
  if (entry == archive.end()) {
    hogin.close();
    return false;
  }

  const size_t payload_offset = archive.fileOffset(entry);
  const size_t payload_len = entry->len;

  // Read the whole Table.gam payload into memory, then wrap it in an in-memory
  // posix_istream (fmemopen) so the page loop below can reuse the same readers.
  // The HOG stream `hogin` is kept open so texture images can be looked up in
  // `archive` and seeked/read per-texture during the page loop.
  std::vector<uint8_t> payload(payload_len);
  hogin.seek(payload_offset, std::ios_base::beg);
  hogin.read(payload.data(), payload_len);

  posix_istream infile(payload.data(), payload.size(), std::ios_base::in);

  // Always read new-style (net) pages.
  Old_table_method = 0;

  // local page containers
  mngs_generic_page genericpage{};
  mngs_ship_page shippage{};
  mngs_weapon_page weaponpage{};
  mngs_texture_page texpage{};
  mngs_sound_page soundpage{};
  mngs_door_page doorpage{};
  mngs_megacell_page megacellpage{};

  bool ok = true;
  while (!infile.eof()) {
    uint8_t pagetype = 0;
    int32_t len = 0;
    infile >> pagetype;
    infile >> len;

    switch (pagetype) {
    case PAGETYPE_TEXTURE:
      if (Num_textures < MAX_TEXTURES) {
        if (!mng_ReadNewTexturePage(infile, &texpage))
          ok = false;
        GameTextures[Num_textures] = texpage.tex_struct;
        GameTextures[Num_textures].name = texpage.tex_struct.name;
        // Load the texture's image so textured faces render: the payload is
        // read straight out of the open d3.hog archive and decoded from memory.
        GameTextures[Num_textures].bm_handle = -1;
        if (!texpage.bitmap_name.empty()) {
          int bm = loadTextureFromArchive(archive, hogin, texpage.bitmap_name, BITMAP_FORMAT_1555);
          if (bm >= 0)
            GameTextures[Num_textures].bm_handle = bm;
        }
        Num_textures++;
      } else {
        // Metadata only: we do NOT load bitmaps/procedurals, so just discard.
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_WEAPON:
      if (Num_weapons < MAX_WEAPONS) {
        if (!mng_ReadNewWeaponPage(infile, &weaponpage))
          ok = false;
        Weapons[Num_weapons] = weaponpage.weapon_struct;
        Num_weapons++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_DOOR:
      if (Num_doors < MAX_DOORS) {
        if (!mng_ReadNewDoorPage(infile, &doorpage))
          ok = false;
        Doors[Num_doors] = doorpage.door_struct;
        Num_doors++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_SHIP:
      if (Num_ships < MAX_SHIPS) {
        if (!mng_ReadNewShipPage(infile, &shippage))
          ok = false;
        Ships[Num_ships] = shippage.ship_struct;
        Num_ships++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_SOUND:
      if (Num_sounds < MAX_SOUNDS) {
        if (!mng_ReadNewSoundPage(infile, &soundpage))
          ok = false;
        Sounds[Num_sounds] = soundpage.sound_struct;
        Num_sounds++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_GENERIC:
      if (Num_objects < MAX_OBJECTS) {
        if (!mng_ReadNewGenericPage(infile, &genericpage))
          ok = false;
        Object_info[Num_objects] = genericpage.objinfo_struct;
        Object_info[Num_objects].name = genericpage.objinfo_struct.name;
        Num_objects++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_MEGACELL:
      if (Num_megacells < MAX_MEGACELLS) {
        if (!mng_ReadNewMegacellPage(infile, &megacellpage))
          ok = false;
        Megacells[Num_megacells] = megacellpage.megacell_struct;
        Num_megacells++;
      } else {
        discardBytes(infile, len);
      }
      break;

    case PAGETYPE_ROBOT:
    case PAGETYPE_POWERUP:
    case PAGETYPE_GAMEFILE:
    case PAGETYPE_UNKNOWN:
    default:
      // Unsupported/game-only page types: read and discard the payload.
      discardBytes(infile, len);
      break;
    }

    if (!ok)
      break;
  }

  hogin.close();

  return ok;
}

//-----------------------------------------------------------------------------
// Name lookups over the loaded game tables (replaces the original stricmp
// searches in objinfo.cpp / weapon.cpp / soundload.cpp).
//-----------------------------------------------------------------------------

// Searches all object ids for a specific name.  Returns the found id, or -1.
int FindObjectIDName(const std::string &name) {
  for (int i = 0; i < Num_objects; i++)
    if ((Object_info[i].type != OBJ_NONE) && match(name, Object_info[i].name))
      return i;

  return -1;
}

// Searches the weapons table for a matching name.  Returns the id, or -1.
int FindWeaponName(const std::string &name) {
  for (int i = 0; i < Num_weapons; i++)
    if (Weapons[i].used && match(name, Weapons[i].name))
      return i;

  return -1;
}

// Searches the sound table for a matching name.  Returns the id, or -1.
int FindSoundName(const std::string &name) {
  for (int i = 0; i < Num_sounds; i++)
    if (Sounds[i].used && match(name, Sounds[i].name))
      return i;

  return -1;
}
