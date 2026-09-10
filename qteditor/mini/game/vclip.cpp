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

 * $Logfile: /DescentIII/Main/vclip.cpp $
 * $Revision: 26 $
 * $Date: 5/10/00 2:43p $
 * $Author: Jeff $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/Main/vclip.cpp $
 *
 * 26    5/10/00 2:43p Jeff
 * fixed bug when paging in data if the tablefile specifies a full path to
 * a primative instead of just a filename (bug in 3rd party tablefile
 * editors)
 *
 * 25    4/19/00 5:07p Matt
 * From Duane for 1.4
 * Added checks, asserts, and fixes for bad return values
 *
 * 24    10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 23    5/07/99 7:23p Jason
 * changes for low video memory
 *
 * 22    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 21    4/12/99 3:53p Jason
 * fixed loading of erroneous vclips
 *
 * 20    3/02/99 1:17p Jason
 * fixed loading of alpha numeric IFLs
 *
 * 19    2/10/99 3:47p Jason
 * table filter changes
 *
 * 18    1/22/99 3:59p Jason
 * added 256x256 textures to help with terrain skies
 *
 * 17    1/19/99 11:04a Jason
 * fixed scaling problem
 *
 * 16    1/18/99 11:11a Jason
 * made vclips be specular
 *
 * 15    10/18/98 2:59p Jason
 * fixes for beta4
 *
 * 14    10/14/98 5:50p Jason
 * added lowmem modes for quartering textures
 *
 * 13    10/09/98 3:32p Kevin
 * New memory library
 *
 * 12    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 11    5/19/98 5:43p Jason
 * free all vclips at the end of the program
 *
 * 10    5/14/98 2:38p Jason
 * made temp solution for vclip residency problem
 *
 * 9     5/14/98 12:56p Jason
 * changes to help lower memory usage
 *
 * 8     2/26/98 2:47p Mark
 * added assert
 *
 * 7     2/11/98 4:58p Jason
 * fixed some memory problems with resizing and mipping
 *
 * 6     11/03/97 6:21p Jason
 * fixed some problems with animating textures being remapped improperly
 *
 * 5     10/31/97 6:33p Jason
 * fixed bug with whitespace in ifl files
 *
 * 4     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 3     9/04/97 12:03p Matt
 * Got rid of warnings
 *
 * 2     8/20/97 6:09p Jason
 * fixed out of bounds problem that bounds checker reported
 *
 * 16    5/13/97 2:24p Jason
 * fixed a couple of animating texture bugs
 *
 * 15    5/12/97 11:41a Jason
 * made game work (default) to 16bit no mip maps mode
 * Saves us alot of memory
 *
 * 14    5/08/97 1:16p Jason
 * made ChangeEndName work with device independent calls
 *
 * 13    5/01/97 1:05p Mark
 * FROM JASON: Fixed stupid =/== bug in vclip loading
 *
 * 12    4/28/97 6:36p Jason
 * fixed a bug with vclips
 *
 * 11    4/25/97 3:31p Jason
 * implemented better memory management for vclips and bitmaps
 *
 * 10    4/24/97 5:42p Jason
 * got fireball vclips working
 *
 * 9     3/03/97 6:21p Matt
 * Changed cfile functions to use D3 naming convention
 *
 * 8     2/20/97 3:27p Jason
 * made ifl reader ignore lines beginning with spaces
 *
 * 7     2/19/97 3:23p Jason
 * added hooks to import animated iffs
 *
 * 6     2/19/97 11:37a Jason
 * take gametime into consideration when animating a vclip
 *
 * 5     2/10/97 1:58p Jason
 * made vclips free their bitmaps when they are deallocated
 *
 * 4     2/04/97 3:19p Jason
 * better error checking
 *
 * 3     1/25/97 5:56p Jason
 * changes to support new 8bit palettized textures and vclips
 *
 * 2     1/21/97 12:22p Jason
 * added vclip support
 *
 * $NoKeywords: $
 */

#include <QtGlobal>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>

#include "bitmap.h"
#include "cfile.h"
#include "ddio.h"
#include "game.h"
#include "gametexture.h"
#include "iff.h"
#include "log.h"
#include "mem.h"
#include "pstypes.h"
#include "vclip.h"
#include "string_helpers.h"

vclip GameVClips[MAX_VCLIPS];
int Num_vclips = 0;

#define DEFAULT_FRAMETIME .07f

#define VCLIP_VERSION 1
// Frees all the memory used by vclips
void FreeAllVClips() {
  LOG_DEBUG("Freeing all vclips!");

  for (int i = 0; i < MAX_VCLIPS; i++) {
    if (GameVClips[i].used > 0) {
      GameVClips[i].used = 1;
      FreeVClip(i);
    }
  }
}

// Simply sets all vclips to unused
void InitVClips() {
  for (int i = 0; i < MAX_VCLIPS; i++)
    GameVClips[i].used = 0;

  atexit(FreeAllVClips);
}

// Allocs a vclip for use
// Returns -1 on error
int AllocVClip() {
  int i;

  for (i = 0; i < MAX_VCLIPS; i++) {
    if (GameVClips[i].used == 0) {
      GameVClips[i] = vclip{};
      GameVClips[i].frames.resize(VCLIP_MAX_FRAMES);
      GameVClips[i].frame_time = DEFAULT_FRAMETIME;
      GameVClips[i].flags = VCF_NOT_RESIDENT;
      GameVClips[i].used = 1;
      Num_vclips++;
      return i;
    }
  }

  Q_ASSERT(false); // Ran out of vclips.  What the hell are you doing?  Get Jason
  return -1;
}

void FreeVClip(int num) {
  Q_ASSERT(GameVClips[num].used > 0);

  GameVClips[num].used--;
  if (GameVClips[num].used > 0)
    return; // other things are using this vclip

  if (!(GameVClips[num].flags & VCF_NOT_RESIDENT)) {
    for (int i = 0; i < GameVClips[num].num_frames; i++)
      bm_FreeBitmap(GameVClips[num].frames[i]);
  }

  GameVClips[num].frames.clear();
  GameVClips[num].num_frames = 0;

  Num_vclips--;
  Q_ASSERT(Num_vclips >= 0);
}

// Saves a given video clip to a file
// Returns 1 if everything ok, 0 otherwise
// "num" is index into GameVClip array
int SaveVClip(const std::filesystem::path& filename, int num) {
#if 0
  CFILE *outfile;
  vclip *vc = &GameVClips[num];

  Q_ASSERT(vc->used);
  Q_ASSERT(!filename.empty());

  PageInVClip(num);

  outfile = (CFILE *)cfopen(filename, "wb");
  if (!outfile) {
    LOG_WARNING("Couldn't save vclip %s!", filename.string().c_str());
    return 0;
  }

  // write out the header for this vclip
  cf_WriteByte(outfile, 127);
  cf_WriteByte(outfile, (int8_t)VCLIP_VERSION);

  cf_WriteByte(outfile, vc->num_frames);
  // cf_WriteFloat (outfile,vc->play_time);
  cf_WriteFloat(outfile, vc->frame_time);
  // cf_WriteInt (outfile,vc->flags);
  // cf_WriteFloat (outfile,vc->light_value);

  // Now save each frame of this vclip
  for (int i = 0; i < vc->num_frames; i++) {
    if (bm_SaveBitmap(outfile, vc->frames[i]) != 1) {
      LOG_ERROR("Couldn't save frame %d of vclip %s!", i, filename.u8string().c_str());
      Q_ASSERT(false);
      cfclose(outfile);
      return 0;
    }
  }

  cfclose(outfile);
  return 1;
#endif
  Q_ASSERT(false);
  return 0;
}

extern int Low_vidmem;
// Pages in a vclip if it needs to be
void PageInVClip(int vcnum) {
  Q_ASSERT(GameVClips[vcnum].used >= 1);
  if (!(GameVClips[vcnum].flags & VCF_NOT_RESIDENT))
    return;

  // The mini editor pages every vclip in eagerly through LoadVClipFromMemory,
  // so a non-resident vclip here means the loading flow changed (e.g. a
  // lazily-paged vclip was created without a payload).  There is no CFILE/HOG
  // re-open helper in the mini, so this cannot be satisfied on demand.
  Q_ASSERT(false);
}

// Reads one 32-bit little-endian word (used for the OAF container header).
static uint32_t readOafWord(posix_istream &in) {
  uint8_t b[4]{};
  in.read(reinterpret_cast<char *>(b), sizeof(b));
  return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

// Allocs and loads a fully-resident vclip from an in-memory OAF payload (a HOG
// entry).  This is the mini port of the engine's AllocLoadVClip + PageInVClip
// pair (Descent3/vclip.cpp): the OAF container leads with a vclip header
//   versioned: 0x7f version num_frames frame_time(float32, unused)
//   legacy:    num_frames frame_time(float32) (float32) (int32) (float32)
// followed by num_frames contiguous OGF/TGA bitmaps, each decoded with
// bm_tga_alloc_file (which leaves the stream positioned past its frame).  All
// frames are stored in GameVClips[].  Returns the vclip index, or -1 on error.
int LoadVClipFromMemory(const uint8_t *data, size_t size, const std::string &name, int format) {
  if (size < 7)
    return -1;

  // If this vclip is already in memory, just reference it again.
  for (int i = 0; i < MAX_VCLIPS; i++) {
    if (GameVClips[i].used && match(GameVClips[i].name, name)) {
      GameVClips[i].used++;
      return i;
    }
  }

  posix_istream infile(const_cast<uint8_t *>(data), size, std::ios_base::in);
  if (!infile.is_open()) {
    LOG_ERROR("LoadVClipFromMemory: Can't open in-memory stream for %s.", name.c_str());
    return -1;
  }

  const int vcnum = AllocVClip();
  if (vcnum < 0)
    return -1;

  vclip *vc = &GameVClips[vcnum];
  vc->name = name;

  // Container header (engine Descent3/vclip.cpp PageInVClip).
  int num_frames = (uint8_t)infile.get();
  if (num_frames == 127) {
    infile.get();     // version
    num_frames = (uint8_t)infile.get();
    readOafWord(infile); // frame_time bits (the engine always uses DEFAULT_FRAMETIME)
  } else {
    readOafWord(infile); // legacy header: frame_time + two unknown words + light value
    readOafWord(infile);
    readOafWord(infile);
    readOafWord(infile);
  }

  if (num_frames <= 0 || num_frames > VCLIP_MAX_FRAMES) {
    LOG_ERROR("LoadVClipFromMemory: Bad frame count %d in %s.", num_frames, name.c_str());
    FreeVClip(vcnum);
    return -1;
  }

  for (int i = 0; i < num_frames; i++) {
    std::string frame_name(BITMAP_NAME_LEN, '\0');
    int n = bm_tga_alloc_file(infile, frame_name.data(), format);
    if (n < 0) {
      LOG_ERROR("LoadVClipFromMemory: Couldn't load frame %d of %s.", i, name.c_str());
      for (int j = 0; j < i; j++)
        bm_FreeBitmap(vc->frames[j]);
      FreeVClip(vcnum);
      return -1;
    }
    vc->frames[i] = (int16_t)n;
  }

  vc->num_frames = (int16_t)num_frames;
  vc->frame_time = DEFAULT_FRAMETIME;
  vc->flags &= ~VCF_NOT_RESIDENT;
  return vcnum;
}

// Allocs and loads a vclip from the file named "filename"
// Returns -1 on error, index into GameVClip array on success
int AllocLoadVClip(const std::filesystem::path& filename, int texture_size, int mipped, int pageable, int format) {
#if 0
  char name[PAGENAME_LEN];
  int i;

  Q_ASSERT(filename != NULL);

  i = strlen(filename);

  if (filename[i - 4] == '.' && filename[i - 3] == 'i' && filename[i - 2] == 'f' && filename[i - 1] == 'l')
    return AllocLoadIFLVClip(IGNORE_TABLE(filename), texture_size, mipped, format);

  if (filename[i - 4] == '.' && filename[i - 3] == 'a' && filename[i - 2] == 'b' && filename[i - 1] == 'm')
    Q_ASSERT(false); // Get Jason
            // return AllocLoadIFFAnimClip(filename,);

  ChangeVClipName(filename, name);

  // Check to see if this vclip already exists in memory
  if ((i = FindVClipName(IGNORE_TABLE(name))) != -1) {
    GameVClips[i].used++;
    return i;
  }

  // mprintf(0,"Loading OAF vclip %s\n",name);

  int vcnum = AllocVClip();

  Q_ASSERT(vcnum >= 0);
  strncpy(GameVClips[vcnum].name, name, PAGENAME_LEN);

  if (mipped)
    GameVClips[vcnum].flags |= VCF_WANTS_MIPPED;

  GameVClips[vcnum].target_size = texture_size;

  if (pageable == 1)
    return vcnum;

  PageInVClip(vcnum);

  if (GameVClips[vcnum].num_frames < 1) {
    FreeVClip(vcnum);
    return -1;
  }

  return vcnum;
#endif
  Q_ASSERT(false);
  return -1;
}

// Allocs and loads a vclip from a 3DS ILS file
// Returns -1 on error, else index into GameVClips on success
// Argument texture means that this vclip is an animated texture and
// needs to have an 8bit version
int AllocLoadIFLVClip(const char *filename, int texture_size, int mipped, int format) {
#if 0
  CFILE *infile;
  char name[PAGENAME_LEN];
  uint32_t i, done = 0;

  Q_ASSERT(filename != NULL);

  ChangeVClipName(filename, name);

  // Check to see if this vclip already exists in memory
  if ((i = FindVClipName(IGNORE_TABLE(name))) != -1) {
    GameVClips[i].used++;
    return i;
  }

  infile = (CFILE *)cfopen(filename, "rt");
  if (!infile) {
    LOG_WARNING("Couldn't load IFL vclip %s!", filename);
    return -1;
  }

  LOG_DEBUG("Loading IFL vclip %s", name);

  int vcnum = AllocVClip();

  Q_ASSERT(vcnum >= 0);

  vclip *vc = &GameVClips[vcnum];

  while (!done) {
    char curline[200];

    if (cfeof(infile)) {
      done = 1;
      continue;
    }

    // Read a line and parse it
    cf_ReadString(curline, 200, infile);

    if (curline[0] == ';' || curline[1] == ';' || curline[0] == ' ' || curline[1] == ' ')
      continue;
    if (!(isalnum(curline[0])))
      continue;

    else if (curline[0] == '$') {
      char new_command[50];

      int i;
      for (i = 0; curline[i + 1] != '=' && i < 50; i++) {
        new_command[i] = curline[i + 1];
      }
      if (i == 50) {
        Q_ASSERT(false); // bad command in IFL!
        return -1;
      }

      i++; // advance to data

      // parse data
      if (!stricmp(new_command, "TIME")) {
        // Set play time
        float play_time = atof(&curline[i]);
        Q_ASSERT(play_time >= 0);
      }
    } else {
      int lastslash = -1;
      char bmname[200];

      for (i = 0; i < strlen(curline); i++)
        if (curline[i] == '\\')
          lastslash = i;

      if (lastslash == -1) {
        for (i = 0; i < strlen(filename); i++)
          if (filename[i] == '\\')
            lastslash = i;

        Q_ASSERT(lastslash != -1);

        strcpy(bmname, filename);
        bmname[lastslash + 1] = 0;
        strcat(bmname, curline);
      } else
        strcpy(bmname, curline);

      // Try and load this file

      int bm = bm_AllocLoadFileBitmap(IGNORE_TABLE(bmname), 0, format);
      if (bm < 1) {
        Error("Error loading frame %d of ILS file %s!\n", vc->num_frames, filename);
        cfclose(infile);
        return -1;
      }

      int w, h;

      if (texture_size == NORMAL_TEXTURE) {
        w = TEXTURE_WIDTH;
        h = TEXTURE_HEIGHT;
      } else if (texture_size == SMALL_TEXTURE) {
        // Make small textures a quarter of the size of normal textures
        w = TEXTURE_WIDTH / 2;
        h = TEXTURE_HEIGHT / 2;
      } else if (texture_size == TINY_TEXTURE) {
        // Make these tinys an eigth of the size of normal textures
        w = TEXTURE_WIDTH / 4;
        h = TEXTURE_HEIGHT / 4;
      } else {
        w = bm_w(bm, 0);
        h = bm_h(bm, 0);
      }

      // If differing size, resize!
      if (w != bm_w(bm, 0) || h != bm_h(bm, 0)) {
        int dest_bm;

        dest_bm = bm_AllocBitmap(w, h, mipped * ((w * h) / 3));
        Q_ASSERT(dest_bm >= 0);
        if (mipped)
          GameBitmaps[dest_bm].flags |= BF_MIPMAPPED;
        GameBitmaps[dest_bm].format = GameBitmaps[bm].format;

        bm_ScaleBitmapToBitmap(dest_bm, bm);
        strcpy(GameBitmaps[dest_bm].name, GameBitmaps[bm].name);
        bm_FreeBitmap(bm);

        bm = dest_bm;
      }

      vc->frames[vc->num_frames] = bm;
      vc->num_frames++;
    }
  }

  cfclose(infile);

  if (vc->num_frames == 0) {
    LOG_WARNING("vclip had no valid bitmap names!");
    FreeVClip(vcnum);
    return -1;
  }

  vc->name = name;
  return vcnum;
#endif
  Q_ASSERT(false);
  return -1;
}

// gets the filename from a path, plus appends our .oaf extension
void ChangeVClipName(const std::filesystem::path&  src, std::string& dest)
{
  Q_ASSERT(false);
  /*
  int limit;
  char path[256], ext[256], filename[256];

  limit = PAGENAME_LEN - 5;

  ddio_SplitPath(src, path, filename, ext);

  // Make sure we don't go over our name length limit
  strncpy(dest, filename, limit);

  strcat(dest, ".oaf");
*/
}
// Searches thru all vclips for a specific name, returns -1 if not found
// or index of vclip with name
int FindVClipName(const std::string& name) {
  int i;

  for (i = 0; i < MAX_VCLIPS; i++)
    if (GameVClips[i].used && match(GameVClips[i].name, name))
      return i;
  Q_ASSERT(false);
  return -1;
}
