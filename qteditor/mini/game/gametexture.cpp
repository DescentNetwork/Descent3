/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral game texture lookup (ported from the original gametexture.cpp).
 */

#include "chrono_timer.h"
#include "gametexture.h"
#include "vclip.h"
#include "game.h"
#include "string_helpers.h"
#include "bitmap.h"
#include "renderer.h"
#include "log.h"

#include <QtGlobal>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <filesystem>

std::optional<uint32_t> FindTextureName(const std::string &name) {
  if(!name.empty())
    for (uint32_t i = 0; i < GameTextures.size(); i++)
      if (!GameTextures[i].name.empty() && name == GameTextures[i].name)
        return i;
  std::runtime_error("texture not found!");
  return std::nullopt;
}

// Searches thru all textures for a bitmap of a specific name, returns -1 if
// not found or index of texture with name
std::optional<uint32_t> FindTextureBitmapName(const std::string &name) {
  if(!name.empty())
    for (uint32_t i = 0; i < GameTextures.size(); i++) {
      if (GameTextures.is_unused(i))
        continue;

      if (GameTextures[i].flags.animated) {
        PageInVClip(GameTextures[i].bm_handle);
        vclip *vc = &GameVClips[GameTextures[i].bm_handle];
        if (vc->used) {
          for (int t = 0; t < vc->num_frames; t++) {
            if (match(GameBitmaps[vc->frames[t]].name, name))
              return i;
          }
        }
      } else {
        if (match(GameBitmaps[GameTextures[i].bm_handle].name, name))
          return i;
      }
    }

  return std::nullopt;
}


// Given a texture handle, returns that textures bitmap
// If the texture is animated, returns framenum mod num_of_frames in the animation
// Force is to force the evaluation of a procedural
// Also figures in gametime
int GetTextureBitmap(int handle, int framenum, bool force) {
  int src_bitmap;

  if (GameTextures.is_unused(handle))
    return 0;

  if (GameTextures[handle].flags.animated) {
    float cur_frametime;
    int int_frame;
    texture *tex = &GameTextures[handle];
    PageInVClip(GameTextures[handle].bm_handle);

    vclip *vc = &GameVClips[GameTextures[handle].bm_handle];
    Q_ASSERT(vc->used >= 1);

    if (GameTextures[handle].flags.ping_pong) {
      // Ping pong this texture

      float frametime = tex->speed / vc->num_frames;
      cur_frametime = d3::chrono::last_update() / frametime;
      int_frame = cur_frametime;
      int_frame += framenum;

      int_frame %= (vc->num_frames * 2);
      if (int_frame >= vc->num_frames)
        int_frame = (vc->num_frames - 1) - (int_frame % vc->num_frames);
      else
        int_frame %= vc->num_frames;
      src_bitmap = vc->frames[int_frame];
    } else {
      float frametime = tex->speed / vc->num_frames;
      cur_frametime = d3::chrono::last_update() / frametime;
      int_frame = cur_frametime;
      int_frame += framenum;
      src_bitmap = vc->frames[int_frame % vc->num_frames];
    }
  } else {
    src_bitmap = GameTextures[handle].bm_handle;
  }

  if (GameTextures[handle].flags.procedural) // Do a procedural
  {
    bool do_eval = true;

    if (GameTextures[handle].procedural == NULL)
      AllocateProceduralForTexture(handle);

    // The procedural machinery is only partially ported (AllocateProceduralForTexture
    // can fail and leave the pointer null); fall back to the static bitmap in that case.
    if (GameTextures[handle].procedural != NULL) {
      if (GameTextures[handle].procedural->last_procedural_frame == FrameCount)
        do_eval = false;

      if (d3::chrono::last_update() <
          GameTextures[handle].procedural->last_evaluation_time + GameTextures[handle].procedural->evaluation_time)
        do_eval = false;

      if (!force) // && !Detail_settings.Procedurals_enabled)
      {
        if (d3::chrono::last_update() < GameTextures[handle].procedural->last_evaluation_time + 10.0)
          do_eval = false;
      }

      if (do_eval) {
        //EvaluateProcedural(handle);
        GameTextures[handle].procedural->last_procedural_frame = FrameCount;
        GameTextures[handle].procedural->last_evaluation_time = d3::chrono::last_update();
        src_bitmap = GameTextures[handle].procedural->procedural_bitmap;
        GameBitmaps[src_bitmap].flags |= BF_CHANGED;
      } else
        src_bitmap = GameTextures[handle].procedural->procedural_bitmap;
    }
  }

  return src_bitmap;
}

int AllocateProceduralForTexture(int handle)
{
  return -1;
}

// Given a filename, loads either the bitmap or vclip found in that file.  If
// type is not NULL, sets it to 1 if the file is an animation, otherwise sets it
// to zero.  Returns the bitmap/vclip handle, or -1 on error.
int LoadTextureImage(const std::filesystem::path &filename, int *type, int texture_size, int mipped, int pageable,
                     int format) {
  // Animation containers (.oaf/.ifl/.abm) page in as a vclip.
  std::string ext = filename.extension().string();
  if (!ext.empty() && ext[0] == '.')
    ext.erase(ext.begin());
  const bool anim = match(ext, "oaf") || match(ext, "ifl") || match(ext, "abm");

  if (type != nullptr)
    *type = anim ? 1 : 0;

  // Read the whole file into a buffer and hand it to the in-memory decoders
  // (bm_LoadBitmapFromMemory / LoadVClipFromMemory), matching how HOG entries
  // are loaded elsewhere in the mini port.
  std::ifstream in(filename, std::ios::binary | std::ios::ate);
  if (!in.is_open()) {
    LOG_ERROR("LoadTextureImage: cannot open %s.", filename.c_str());
    return -1;
  }
  const std::streamsize size = in.tellg();
  if (size < 0)
    return -1;
  in.seekg(0, std::ios::beg);
  std::vector<uint8_t> buf(static_cast<size_t>(size));
  if (!in.read(reinterpret_cast<char *>(buf.data()), size))
    return -1;

  const std::string name = filename.filename().string();

  if (anim) {
    const std::optional<uint32_t> vc = LoadVClipFromMemory(buf.data(), buf.size(), name, format);
    return vc.value_or(-1);
  }

  int bm_handle = bm_LoadBitmapFromMemory(buf.data(), buf.size(), name.c_str(), format, mipped);
  if (bm_handle < 1)
    return -1;

  int w = 0, h = 0;
  if (texture_size == NORMAL_TEXTURE) {
    w = TEXTURE_WIDTH;
    h = TEXTURE_HEIGHT;
  } else if (texture_size == SMALL_TEXTURE) {
    w = TEXTURE_WIDTH / 2;
    h = TEXTURE_HEIGHT / 2;
  } else if (texture_size == TINY_TEXTURE) {
    w = TEXTURE_WIDTH / 4;
    h = TEXTURE_HEIGHT / 4;
  } else if (texture_size == HUGE_TEXTURE) {
    w = TEXTURE_WIDTH * 2;
    h = TEXTURE_HEIGHT * 2;
  } else {
    return bm_handle;
  }

  // If a differing size is requested, scale to it.
  if (!pageable && (w != bm_w(bm_handle, 0) || h != bm_h(bm_handle, 0))) {
    LOG_WARNING("Resizing bitmap %s from %d x %d to %d x %d!",
                GameBitmaps[bm_handle].name, bm_w(bm_handle, 0), bm_h(bm_handle, 0), w, h);

    int dest_bm = bm_AllocBitmap(w, h, mipped * ((w * h * 2) / 3)).value_or(-1);
    Q_ASSERT(dest_bm >= 0);

    if (mipped)
      GameBitmaps[dest_bm].flags |= BF_MIPMAPPED;
    GameBitmaps[dest_bm].format = format;

    bm_ScaleBitmapToBitmap(dest_bm, bm_handle);
    strncpy(GameBitmaps[dest_bm].name, GameBitmaps[bm_handle].name, BITMAP_NAME_LEN - 1);
    GameBitmaps[dest_bm].name[BITMAP_NAME_LEN - 1] = 0;
    bm_FreeBitmap(bm_handle);

    bm_handle = dest_bm;
  }

  return bm_handle;
}


byte_istream& operator>>(byte_istream& input, texture& data)
{
  return input
          >> data.r
          >> data.g
          >> data.b
          >> data.alpha
          >> data.speed
          >> data.slide_u
          >> data.slide_v
          >> data.reflectivity
          >> data.corona_type
          >> data.damage
          >> reinterpret_cast<uint32_t&>(data.flags);
}

byte_ostream& operator<<(byte_ostream& output, const texture& data)
{
  return output
          << data.r
          << data.g
          << data.b
          << data.alpha
          << data.speed
          << data.slide_u
          << data.slide_v
          << data.reflectivity
          << data.corona_type
          << data.damage
          << reinterpret_cast<const uint32_t&>(data.flags);
}

// ============================================================================
// Texture slot management (ported from the engine's gametexture.cpp).
// ============================================================================

// Set aside a texture for use
int AllocTexture() {
  if (GameTextures.num_empty() == 0 && GameTextures.size() >= MAX_TEXTURES)
    return -1; // No textures free!

  const size_t n = GameTextures.next_slot();
  Q_ASSERT(GameTextures.is_unused(n));

  GameTextures[n] = texture{};

  GameTextures.acquire(n);
  return static_cast<int>(n);
}

// Frees a texture for future use
void FreeTexture(int n) {
  Q_ASSERT(GameTextures.is_used(n));

  GameTextures[n] = texture{};
  GameTextures.release(n);
}

// Given current index, gets index of next texture in use
int GetNextTexture(int n) {
  if ((n < 0) || (n >= static_cast<int>(GameTextures.size())))
    n = -1;

  for (int i = n + 1; i < static_cast<int>(GameTextures.size()); i++)
    if (GameTextures.is_used(i))
      return i;
  for (int i = 0; i < n; i++)
    if (GameTextures.is_used(i))
      return i;

  // this is the only one
  return n;
}

// Given current index, gets index of prev texture in use
int GetPreviousTexture(int n) {
  if ((n < 0) || (n >= static_cast<int>(GameTextures.size())))
    n = static_cast<int>(GameTextures.size());

  for (int i = n - 1; i >= 0; i--)
    if (GameTextures.is_used(i))
      return i;
  for (int i = static_cast<int>(GameTextures.size()) - 1; i > n; i--)
    if (GameTextures.is_used(i))
      return i;

  // this is the only one
  return n;
}

