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

#include <string>
#include <stdexcept>

int FindTextureName(const std::string &name) {
  for (int i = 0; i < Num_textures; i++) {
    if (!GameTextures[i].name.empty() && name == GameTextures[i].name)
      return i;
  }
  std::runtime_error("texture not found!");
  return -1;
}

// Searches thru all textures for a bitmap of a specific name, returns -1 if
// not found or index of texture with name
int FindTextureBitmapName(const std::string &name) {
  for (int i = 0; i < Num_textures; i++) {
    if (!GameTextures[i].used)
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

  return -1;
}


// Given a texture handle, returns that textures bitmap
// If the texture is animated, returns framenum mod num_of_frames in the animation
// Force is to force the evaluation of a procedural
// Also figures in gametime
int GetTextureBitmap(int handle, int framenum, bool force) {
  int src_bitmap;

  if (!GameTextures[handle].used)
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

  return src_bitmap;
}

int AllocateProceduralForTexture(int handle)
{
  return -1;
}

