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

// Generic page reader (ported from the full engine's manage/generic.cpp), plus
// the shared physics / weapon-battery / lighting chunk readers that are used by
// several per-page readers.  Ported for the mini editor so it reads the D3
// page-table metadata from Table.gam (inside d3.hog) into the mini editor's
// global arrays without touching the game renderer.  This file links only
// against Qt + OpenGL + the mini cfile implementation.

#include "genericpage.h"

#include <algorithm>
#include <utility>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <QtGlobal>

#include "manage.h"
#include "mem.h"     // mem_rmalloc
#include "objinfo.h" // object_info, object_type::powerup, ...
#include "aistruct.h" // ai_info_t
#include "aistruct_external.h"
#include "object_external_struct.h" // physics_info, light_info, MAX_OBJECTS
#include "gamedata_helpers.h"
#include "log.h"          // LOG_ERROR
#include "polymodel.h"    // LoadPolyModel, Poly_models
#include "soundpage.h"    // mng_GetGuaranteedSoundPage
#include "string_helpers.h"
#include "ssl_lib.h"      // Sounds
#include "weapon.h"       // Weapons, FindWeaponName
#include "weapon_external.h" // LASER_INDEX
#include "weaponpage.h"   // mng_GetGuaranteedWeaponPage

// Old delay types (originally #defined locally in the full-engine generic.cpp)
#ifndef OLD_DF_DELAY_MIN_MAX
#define OLD_DF_DELAY_MIN_MAX 0x0000001
#endif
#ifndef OLD_DF_DELAY_MASK
#define OLD_DF_DELAY_MASK 0x0000003
#endif

// Scratch size for serializing a generic page.  Real pages (as produced by the
// original D3 tools) fit comfortably within this; any page that would not is
// rejected by the Q_ASSERT in mng_WriteNewGenericPage.
constexpr size_t kGenericPageBufferSize = 1u << 20;

//-----------------------------------------------------------------------------
// Generic page (manage/generic.cpp : 448-1337)
//-----------------------------------------------------------------------------

static void mng_InitGenericPage(mngs_generic_page *genericpage) {
  int i;

  *genericpage = mngs_generic_page{};
  genericpage->image_name.clear();

  genericpage->med_image_name.clear();
  genericpage->lo_image_name.clear();

  genericpage->objinfo_struct.description.clear();
  genericpage->objinfo_struct.icon_name[0] = '\0';

  genericpage->objinfo_struct.med_lod_distance = DEFAULT_MED_LOD_DISTANCE;
  genericpage->objinfo_struct.lo_lod_distance = DEFAULT_LO_LOD_DISTANCE;

  genericpage->objinfo_struct.phys_info.hit_die_dot = 1.0f;
  genericpage->objinfo_struct.respawn_scalar = 1.0f;

  // ai_info inherits its play-balance defaults from the ai_info_t declaration

  genericpage->objinfo_struct.module_name.clear();
}

static void GenericPageSetPowerupDefaultAmmo(object_info *ip) {
  // Default is zero
  ip->ammo_count = 0;

  const std::unordered_map<std::string, uint32_t> default_ammo =
  {
    { "Vauss", 5000 },
    { "Napalm", 500 },
    { "MassDriver", 20 },
    { "Frag", 1 },
    { "ImpactMortar", 1 },
    { "NapalmRocket", 1 },
    { "Cyclone", 1 },
    { "BlackShark", 1 },
    { "Concussion", 1 },
    { "Homing", 1 },
    { "Smart", 1 },
    { "Mega", 1 },
    { "Guided", 1 },
    { "4PackHoming", 4 },
    { "4PackConc", 4 },
    { "4PackFrag", 4 },
    { "4PackGuided", 4 },
    { "Vauss clip", 1250 },
    { "MassDriverAmmo", 5 },
    { "NapalmTank", 100 },
  };

  ip->ammo_count = default_ammo.at(ip->name);
}

// Reads a generic page from an open file.  Returns 0 on error.
bool mng_ReadNewGenericPage(posix_istream &infile, mngs_generic_page *genericpage) {
  int i, j;

  mng_InitGenericPage(genericpage);

  int16_t version_tmp = 0;
  infile >> version_tmp;
  int version = version_tmp;

  infile >> reinterpret_cast<uint8_t&>(genericpage->objinfo_struct.type);

  // Read object name
  infile >> genericpage->objinfo_struct.name;

  // Read model names
  infile >> genericpage->image_name;
  infile >> genericpage->med_image_name;
  infile >> genericpage->lo_image_name;

  // Read out impact data
  infile >> genericpage->objinfo_struct.impact_size;
  infile >> genericpage->objinfo_struct.impact_time;
  infile >> genericpage->objinfo_struct.damage;

  // Read score
    infile >> genericpage->objinfo_struct.score;

  // Read ammo
  if (genericpage->objinfo_struct.type == object_type_e::powerup) {
    infile >> genericpage->objinfo_struct.ammo_count;
  } else
    genericpage->objinfo_struct.ammo_count = 0;

  // Read script name (discarded)
  std::string dummy;
  infile >> dummy; // genericpage->objinfo_struct.script_name

  if (version >= 18) {
    infile >> genericpage->objinfo_struct.module_name;
  } else {
    genericpage->objinfo_struct.module_name[0] = '\0';
  }

  if (version >= 19)
    infile >> genericpage->objinfo_struct.script_name_override;

  int desc = 0;
  {
    uint8_t db = 0;
    infile >> db;
    desc = db;
  }
  if (desc) {
    // Read description if there is one

    infile >> genericpage->objinfo_struct.description;
  } else
    genericpage->objinfo_struct.description.clear();

  // Read icon name
  infile >> genericpage->objinfo_struct.icon_name;

  // Read LOD distances
  infile >> genericpage->objinfo_struct.med_lod_distance;
  infile >> genericpage->objinfo_struct.lo_lod_distance;

  // Read physics stuff
  infile >> genericpage->objinfo_struct.phys_info;

  // Read size
  infile >> genericpage->objinfo_struct.size;

  // Read light info
  infile >> genericpage->objinfo_struct.lighting_info;

  // Read hit points
  infile >> genericpage->objinfo_struct.hit_points;

  // Read flags (stored as a raw 32-bit value on disk; the packed bitfield
  // struct shares its storage, so it can be streamed in directly).
  infile >> reinterpret_cast<uint32_t&>(genericpage->objinfo_struct.flags);

  // Read AI info (current-version layout)
  infile >> genericpage->ai_info;

  // Read out objects spewed
  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    infile >> reinterpret_cast<uint8_t&>(genericpage->objinfo_struct.f_dspew);
    infile >> genericpage->objinfo_struct.dspew_percent[i];
    infile >> genericpage->objinfo_struct.dspew_number[i];

    // Read spew name
    infile >> genericpage->dspew_name[i];
  }

  // Read out animation info
  if (version < 20) {
    for (i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
      for (j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
        uint8_t f = 0, t = 0;
        infile >> f;
        infile >> t;
        genericpage->anim[i].elem[j].from = f;
        genericpage->anim[i].elem[j].to = t;
        infile >> genericpage->anim[i].elem[j].spc;
      }
    }
  } else {
    infile >> genericpage->anim;
  }

  // read weapon batteries
  infile >> genericpage->static_wb;

  // read weapon names
  infile >> genericpage->weapon_name;

  // read sounds
  Q_ASSERT(generic_sound::count == 2);
  infile >> genericpage->sound_name;
  if (version < 26) { // used to be three sounds
    std::string temp_sound_name;
    infile >> temp_sound_name;
  }

  infile >> genericpage->ai_sound_name;

  infile >> genericpage->fire_sound_name;

  infile >> genericpage->anim_sound_name;

  // Read respawn scalar
  if (version >= 21)
    infile >> genericpage->objinfo_struct.respawn_scalar;
  else
    genericpage->objinfo_struct.respawn_scalar = 1.0;

  if (version >= 22) {
    int16_t n = 0;
    infile >> n;
    const int n_death_types = std::clamp(static_cast<int>(n), 0, MAX_DEATH_TYPES);
    auto& death_types = genericpage->objinfo_struct.death_types;
    auto& death_probabilities = genericpage->objinfo_struct.death_probabilities;
    death_types.resize(n_death_types);
    death_probabilities.resize(n_death_types);
    for (i = 0; i < n_death_types; i++)
    {
      infile >> reinterpret_cast<uint32_t&>(death_types[i].flags);
      if (version == 22) { // translate death flags
        Q_ASSERT(false);            // this version no longer supported
      }

      infile >> death_types[i].delay_min;
      infile >> death_types[i].delay_max;
      infile >> death_probabilities[i];

      // Fix up for changed flags
      if (version < 27) {
        const uint32_t flags = std::bit_cast<uint32_t>(death_types[i].flags);
        if ((flags & OLD_DF_DELAY_MASK) != OLD_DF_DELAY_MIN_MAX) {
          death_types[i].delay_min = 0.0;
          death_types[i].delay_max = 0.0;
        }
      }
    }
  }

  // Set score from hitpoints if old version
  if (version < 24) {
    if ((genericpage->objinfo_struct.type == object_type_e::robot) ||
        (genericpage->objinfo_struct.type == object_type_e::building && genericpage->objinfo_struct.flags.control_ai))
      if (genericpage->objinfo_struct.flags.destroyable)
        genericpage->objinfo_struct.score = 3 * genericpage->objinfo_struct.hit_points;
  }

  Q_ASSERT(genericpage->objinfo_struct.type != object_type_e::none);

  return true; // successfully read
}

//-----------------------------------------------------------------------------
// Chunk writers (the exact mirror of the readers above: same field order and
// encodings, so SaveTable can round-trip pages it has loaded).
//-----------------------------------------------------------------------------

// Serializes one page (header + payload) into a concrete posix_ostream with
// the [page_type::generic][int32 len] frame back-patched, mirroring the
// original StartManagePage/EndManagePage.  The public mng_WriteNewGenericPage
// runs this against a scratch buffer so it can talk to any byte_ostream.
static void mng_WriteNewGenericPageFramed(posix_ostream &outfile, mngs_generic_page *genericpage) {
  int i, j;

  outfile << std::to_underlying(page_type::generic);
  const off_t chunk_start_pos = outfile.tell();
  int32_t idum = 0; // placeholder for chunk len
  outfile << idum;

  int16_t version = GENERICFILE_VERSION;
  outfile << version;

  outfile << reinterpret_cast<const uint8_t&>(genericpage->objinfo_struct.type);

  // Write object name
  outfile << genericpage->objinfo_struct.name;

  // Write model names
  outfile << genericpage->image_name;
  outfile << genericpage->med_image_name;
  outfile << genericpage->lo_image_name;

  // Write out impact data
  outfile << genericpage->objinfo_struct.impact_size;
  outfile << genericpage->objinfo_struct.impact_time;
  outfile << genericpage->objinfo_struct.damage;

  // Write score
  outfile << genericpage->objinfo_struct.score;

  // Write ammo
  if (genericpage->objinfo_struct.type == object_type_e::powerup)
    outfile << genericpage->objinfo_struct.ammo_count;

  // Write script name (discarded by the reader)
  outfile << std::string();

  // Write module name / scriptname override
  outfile << genericpage->objinfo_struct.module_name;
  outfile << genericpage->objinfo_struct.script_name_override;

  if (!genericpage->objinfo_struct.description.empty()) {
    // Write description if there is one
    outfile << static_cast<uint8_t>(1);
    outfile << genericpage->objinfo_struct.description;
  } else
    outfile << static_cast<uint8_t>(0);

  // Write icon name
  outfile << genericpage->objinfo_struct.icon_name;

  // Write LOD distances
  outfile << genericpage->objinfo_struct.med_lod_distance;
  outfile << genericpage->objinfo_struct.lo_lod_distance;

  // Write physics stuff
  outfile << genericpage->objinfo_struct.phys_info;

  // Write size
  outfile << genericpage->objinfo_struct.size;

  // Write light info
  outfile << genericpage->objinfo_struct.lighting_info;

  // Write hit points
  outfile << genericpage->objinfo_struct.hit_points;

  // Write flags (the on-disk form is a raw 32-bit value; the packed bitfield
  // struct shares storage, so it can be streamed out directly).
  outfile << reinterpret_cast<const uint32_t&>(genericpage->objinfo_struct.flags);

  // Write AI info (current-version layout)
  outfile << genericpage->ai_info;

  // Write out objects spewed
  for (i = 0; i < MAX_DSPEW_TYPES; i++) {
    outfile << reinterpret_cast<const uint8_t&>(genericpage->objinfo_struct.f_dspew);
    outfile << genericpage->objinfo_struct.dspew_percent[i];
    outfile << genericpage->objinfo_struct.dspew_number[i];
    outfile << genericpage->dspew_name[i];
  }

  // Write out animation info
  outfile << genericpage->anim;

  // Write out weapon batteries
  outfile << genericpage->static_wb;

  // Write out weapon names
  outfile << genericpage->weapon_name;

  // Write out sounds
  outfile << genericpage->sound_name;

  outfile << genericpage->ai_sound_name;

  outfile << genericpage->fire_sound_name;

  outfile << genericpage->anim_sound_name;

  // Write out respawn scalar
  outfile << genericpage->objinfo_struct.respawn_scalar;

  // Write out death information (count-prefixed like the reader: the number of
  // entries actually stored, capped at MAX_DEATH_TYPES so a single page can
  // never exceed the on-disk bound).
  {
    const size_t n = std::min(genericpage->objinfo_struct.death_types.size(),
                              genericpage->objinfo_struct.death_probabilities.size());
    const int16_t nd = static_cast<int16_t>(std::min(n, static_cast<size_t>(MAX_DEATH_TYPES)));
    outfile << nd;
    for (int k = 0; k < nd; k++) {
      outfile << reinterpret_cast<const uint32_t&>(genericpage->objinfo_struct.death_types[k].flags)
              << genericpage->objinfo_struct.death_types[k].delay_min
              << genericpage->objinfo_struct.death_types[k].delay_max
              << genericpage->objinfo_struct.death_probabilities[k];
    }
  }

  // Fill in page length when done writing
  const off_t save_pos = outfile.tell();
  const off_t chunk_len = save_pos - chunk_start_pos;
  outfile.seek(chunk_start_pos, std::ios_base::beg);
  int32_t len = static_cast<int32_t>(chunk_len);
  outfile << len; // write chunk length
  outfile.seek(save_pos, std::ios_base::beg);
}

// Serializes a generic page in the current table-file format (mirrors
// mng_ReadNewGenericPage: same field order and encodings, so a page written
// here parses back bit-for-bit with the reader) and writes the full page
// frame downstream.
void mng_WriteNewGenericPage(byte_ostream &outfile, mngs_generic_page *genericpage) {
  // The page frame's length field has to be patched in after the payload is
  // written, which needs seek/tell, so serialize into a scratch buffer first.
  std::vector<uint8_t> buffer(kGenericPageBufferSize);
  posix_ostream scratch(buffer.data(), buffer.size(), std::ios_base::out);
  mng_WriteNewGenericPageFramed(scratch, genericpage);
  const size_t bytes = static_cast<size_t>(scratch.tell());
  Q_ASSERT(bytes <= buffer.size());
  // Close first: fmemopen's stdio buffering only materializes the bytes into
  // the caller-visible memory array on flush/close.
  scratch.close();
  outfile.write(buffer.data(), bytes);
}

//-----------------------------------------------------------------------------
// Name <-> handle resolution for the name-based fields of a generic page
// (ported from generic.cpp: 1907-2196 and generic.cpp: 2325-2351).
//-----------------------------------------------------------------------------

// The table writer stores "INVALID NAME" in place of an unused name slot.
static bool mng_PageNameUsable(const std::string &name) {
  return !name.empty() && !match(name, "INVALID NAME");
}

static std::string mng_ModelNameOf(int handle) {
  if (handle >= 0 && handle < MAX_POLY_MODELS)
    return Poly_models[handle].name;
  return {};
}

// First searches through the object index to see if the object is already
// loaded.  If not, searches in the table file and loads it.
// Returns index of object if found, std::nullopt if not.
index_t mng_GetGuaranteedGenericPage(const std::string &name) {
  return FindObjectIDName(name);
}

int mng_AssignGenericPageToObjInfo(mngs_generic_page &genericpage, index_t n) {
  if (!n)
    return 0;

  object_info &obj = Object_info[*n];

  // The page carries its own anim / static_wb / ai_info copies, so the
  // destination's allocations are kept and filled from the page below (the
  // original memcpy skipped those three pointer fields for the same reason).
  std::vector<anim_elem> anim = std::move(obj.anim);
  std::vector<otype_wb_info> static_wb = std::move(obj.static_wb);
  std::vector<ai_info_t> ai_info = std::move(obj.ai_info);

  obj = genericpage.objinfo_struct;
  obj.anim = std::move(anim);
  obj.static_wb = std::move(static_wb);
  obj.ai_info = std::move(ai_info);

  if (!obj.anim.empty())
    obj.anim.assign(genericpage.anim.begin(), genericpage.anim.end());

  if (!obj.static_wb.empty())
    obj.static_wb.assign(genericpage.static_wb.begin(), genericpage.static_wb.end());

  if (!obj.ai_info.empty())
    obj.ai_info[0] = genericpage.ai_info;

  obj.multi_allowed = true;

  // Try and load our generic model from the disk
  const index_t img_handle = LoadPolyModel(genericpage.image_name, 1);
  if (!img_handle) {
    LOG_ERROR("Couldn't load file '%s' in AssignGenericPage...", genericpage.image_name.c_str());
    obj.render_handle = -1;
    return 0;
  }
  obj.render_handle = static_cast<int>(*img_handle);

  if (mng_PageNameUsable(genericpage.med_image_name)) {
    const index_t med_handle = LoadPolyModel(genericpage.med_image_name, 1);
    if (!med_handle) {
      LOG_ERROR("Couldn't load file '%s' in AssignGenericPage...", genericpage.med_image_name.c_str());
      obj.med_render_handle = -1;
      return 0;
    }
    obj.med_render_handle = static_cast<int>(*med_handle);
  } else {
    obj.med_render_handle = -1;
  }

  if (mng_PageNameUsable(genericpage.lo_image_name)) {
    const index_t lo_handle = LoadPolyModel(genericpage.lo_image_name, 1);
    if (!lo_handle) {
      LOG_ERROR("Couldn't load file '%s' in AssignGenericPage...", genericpage.lo_image_name.c_str());
      obj.lo_render_handle = -1;
      return 0;
    }
    obj.lo_render_handle = static_cast<int>(*lo_handle);
  } else {
    obj.lo_render_handle = -1;
  }

  // Try and load the various sounds
  for (size_t i = 0; i < generic_sound::count; i++) {
    if (mng_PageNameUsable(genericpage.sound_name[i])) {
      const index_t sound_handle = mng_GetGuaranteedSoundPage(genericpage.sound_name[i]);
      if (!sound_handle) {
        LOG_WARNING("Couldn't load sound file '%s' in AssignGenericPage %s...", genericpage.sound_name[i].c_str(),
                    genericpage.objinfo_struct.name.c_str());
        obj.sounds[i] = std::nullopt;
      } else {
        obj.sounds[i] = sound_handle;
      }
    } else {
      obj.sounds[i] = std::nullopt;
    }
  }

  for (size_t i = 0; i < MAX_DSPEW_TYPES; i++) {
    if (!genericpage.dspew_name[i].empty()) {
      const index_t obj_handle = mng_GetGuaranteedGenericPage(genericpage.dspew_name[i]);
      if (!obj_handle) {
        obj.dspew[i] = 0;
        obj.dspew_number[i] = 0;
        obj.dspew_percent[i] = 0.0f;
      } else {
        obj.dspew[i] = static_cast<int16_t>(*obj_handle);
      }
    } else {
      obj.dspew[i] = -1;
      obj.dspew_number[i] = 0;
      obj.dspew_percent[i] = 0.0f;
    }
  }

  if (!obj.ai_info.empty()) {
    for (size_t i = 0; i < MAX_AI_SOUNDS; i++) {
      if (mng_PageNameUsable(genericpage.ai_sound_name[i])) {
        const index_t sound_handle = mng_GetGuaranteedSoundPage(genericpage.ai_sound_name[i]);
        if (!sound_handle) {
          LOG_ERROR("Couldn't load ai sound file '%s' in AssignGenericPage %s...",
                    genericpage.ai_sound_name[i].c_str(), genericpage.objinfo_struct.name.c_str());
          obj.ai_info[0].sound[i] = -1;
        } else {
          obj.ai_info[0].sound[i] = static_cast<int>(*sound_handle);
        }
      } else {
        obj.ai_info[0].sound[i] = -1;
      }
    }
  }

  // Try and load the various weapons
  if (!obj.static_wb.empty()) {
    for (size_t i = 0; i < MAX_WBS_PER_OBJ; i++) {
      for (size_t j = 0; j < MAX_WB_GUNPOINTS; j++) {
        if (!genericpage.weapon_name[i][j].empty()) {
          const index_t weapon_handle = mng_GetGuaranteedWeaponPage(genericpage.weapon_name[i][j]);
          if (!weapon_handle) {
            LOG_ERROR("Couldn't load weapon file '%s' in AssignGenericPage %s...",
                      genericpage.weapon_name[i][j].c_str(), genericpage.objinfo_struct.name.c_str());
            obj.static_wb[i].gp_weapon_index[j] = std::to_underlying(weapon_index::laser);
          } else {
            obj.static_wb[i].gp_weapon_index[j] = static_cast<uint16_t>(*weapon_handle);
          }
        } else {
          obj.static_wb[i].gp_weapon_index[j] = std::to_underlying(weapon_index::laser);
        }
      }
    }

    // Try and load the various wb sounds
    for (size_t i = 0; i < MAX_WBS_PER_OBJ; i++) {
      for (size_t j = 0; j < MAX_WB_FIRING_MASKS; j++) {
        if (!genericpage.fire_sound_name[i][j].empty()) {
          const index_t fire_sound_handle = mng_GetGuaranteedSoundPage(genericpage.fire_sound_name[i][j]);
          if (!fire_sound_handle) {
            LOG_ERROR("Couldn't load fire sound file '%s' in AssignGenericPage %s...",
                      genericpage.fire_sound_name[i][j].c_str(), genericpage.objinfo_struct.name.c_str());
            obj.static_wb[i].fm_fire_sound_index[j] = static_cast<uint16_t>(-1);
          } else {
            obj.static_wb[i].fm_fire_sound_index[j] = static_cast<uint16_t>(*fire_sound_handle);
          }
        } else {
          obj.static_wb[i].fm_fire_sound_index[j] = static_cast<uint16_t>(-1);
        }
      }
    }
  }

  // Try and load the various anim sounds
  if (!obj.anim.empty()) {
    for (size_t i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
      for (size_t j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
        if (mng_PageNameUsable(genericpage.anim_sound_name[i][j])) {
          const index_t anim_sound_handle = mng_GetGuaranteedSoundPage(genericpage.anim_sound_name[i][j]);
          if (!anim_sound_handle) {
            LOG_ERROR("Couldn't load anim sound file '%s' in AssignGenericPage %s...",
                      genericpage.anim_sound_name[i][j].c_str(), genericpage.objinfo_struct.name.c_str());
            obj.anim[i].elem[j].anim_sound_index = -1;
          } else {
            obj.anim[i].elem[j].anim_sound_index = static_cast<int>(*anim_sound_handle);
          }
        } else {
          obj.anim[i].elem[j].anim_sound_index = -1;
        }
      }
    }
  }

  return 1;
}

// Copies values from a Generic into a generic_page
void mng_AssignObjInfoToGenericPage(index_t n, mngs_generic_page &genericpage) {
  if (!n)
    return;

  const object_info &obj = Object_info[*n];

  genericpage.objinfo_struct = obj;

  if (!obj.anim.empty())
    std::copy_n(obj.anim.begin(), std::min(obj.anim.size(), genericpage.anim.size()), genericpage.anim.begin());

  if (!obj.static_wb.empty())
    std::copy_n(obj.static_wb.begin(), std::min(obj.static_wb.size(), genericpage.static_wb.size()),
                genericpage.static_wb.begin());

  if (!obj.ai_info.empty())
    genericpage.ai_info = obj.ai_info[0];

  genericpage.image_name = mng_ModelNameOf(obj.render_handle);
  genericpage.med_image_name = mng_ModelNameOf(obj.med_render_handle);
  genericpage.lo_image_name = mng_ModelNameOf(obj.lo_render_handle);

  for (size_t i = 0; i < generic_sound::count; i++) {
    if (obj.sounds[i] && *obj.sounds[i] < Sounds.size())
      genericpage.sound_name[i] = Sounds[*obj.sounds[i]].name;
    else
      genericpage.sound_name[i].clear();
  }

  for (size_t i = 0; i < MAX_DSPEW_TYPES; i++) {
    if (obj.dspew[i] >= 0 && static_cast<uint32_t>(obj.dspew[i]) < MAX_OBJECTS &&
        Object_info[obj.dspew[i]].type != object_type_e::none)
      genericpage.dspew_name[i] = Object_info[obj.dspew[i]].name;
    else
      genericpage.dspew_name[i].clear();
  }

  for (size_t i = 0; i < MAX_AI_SOUNDS; i++) {
    const int sound = obj.ai_info.empty() ? -1 : obj.ai_info[0].sound[i];
    if (sound >= 0 && static_cast<size_t>(sound) < Sounds.size())
      genericpage.ai_sound_name[i] = Sounds[sound].name;
    else
      genericpage.ai_sound_name[i].clear();
  }

  for (size_t i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (size_t j = 0; j < MAX_WB_FIRING_MASKS; j++) {
      const int sound = i < obj.static_wb.size() ? static_cast<int16_t>(obj.static_wb[i].fm_fire_sound_index[j]) : -1;
      if (sound >= 0 && static_cast<size_t>(sound) < Sounds.size())
        genericpage.fire_sound_name[i][j] = Sounds[sound].name;
      else
        genericpage.fire_sound_name[i][j].clear();
    }
  }

  for (size_t i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (size_t j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
      const int sound = i < obj.anim.size() ? obj.anim[i].elem[j].anim_sound_index : -1;
      if (sound >= 0 && static_cast<size_t>(sound) < Sounds.size())
        genericpage.anim_sound_name[i][j] = Sounds[sound].name;
      else
        genericpage.anim_sound_name[i][j].clear();
    }
  }

  for (size_t i = 0; i < MAX_WBS_PER_OBJ; i++) {
    for (size_t j = 0; j < MAX_WB_GUNPOINTS; j++) {
      const int weapon = i < obj.static_wb.size() ? static_cast<int16_t>(obj.static_wb[i].gp_weapon_index[j]) : -1;
      if (weapon >= 0 && static_cast<size_t>(weapon) < Weapons.size())
        genericpage.weapon_name[i][j] = Weapons[weapon].name;
      else
        genericpage.weapon_name[i][j] = "Laser";
    }
  }
}
