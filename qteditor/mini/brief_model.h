/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * Qt-neutral data model + lifecycle for the Briefing editor.  This is the
 * port of the Win32 editor's CBriefEdit screen/effect model, rewritten to use
 * std::string / std::filesystem instead of C strings.
 */

#ifndef BRIEF_MODEL_H
#define BRIEF_MODEL_H

#include <cstdint>
#include <string>
#include <filesystem>

#include "TelComEfxStructs.h"

// Maximum number of briefing screens and effects per screen.
constexpr int MAX_TELCOM_SCREENS = 20;
constexpr int MAX_EFFECTS_PER_SCREEN = 32;
constexpr int MAX_LAYOUT_PREDEFS = 10;

// Effect type ids.
constexpr int BE_NONE = -1;
constexpr int BE_TEXT = 0;
constexpr int BE_BMP = 1;
constexpr int BE_MOVIE = 2;
constexpr int BE_BKG = 3;
constexpr int BE_POLY = 4;
constexpr int BE_SND = 5;
constexpr int BE_BUTTON = 6;

// Global briefing values.
struct BriefGlobalValues {
  std::string title;
  float static_val = 0.0f;
  float glitch_val = 0.0f;
};

// A single briefing effect.  `text` and `filename`/`layout` are dynamic and
// stored as std::string (no C strings in new code).
union BriefEffectDesc {
  TCTEXTDESC text_desc;
  TCBMPDESC bmp_desc;
  TCMOVIEDESC movie_desc;
  TCBKGDESC bkg_desc;
  TCPOLYDESC poly_desc;
  TCSNDDESC snd_desc;
  TCBUTTONDESC button_desc;
};

struct tBriefEffect {
  int type = BE_NONE;
  bool used = false;
  int id = 0;
  std::string text;        // text buffer for text effects
  std::string description; // effect description
  int w = -1, h = -1;      // used for bitmaps
  BriefEffectDesc desc;    // effect-specific descriptor
  int next = -1, prev = -1;
  void init() {
    type = BE_NONE;
    used = false;
    id = 0;
    text.clear();
    description.clear();
    w = h = -1;
    next = prev = -1;
    desc = {};
  }
};

struct tBriefScreen {
  bool used = false;
  int root_effect = -1;
  std::string layout;
  tBriefEffect effects[MAX_EFFECTS_PER_SCREEN];
  uint32_t mission_mask_set = 0, mission_mask_unset = 0;
  int next = -1, prev = -1;
  void init() {
    used = false;
    root_effect = -1;
    layout.clear();
    mission_mask_set = mission_mask_unset = 0;
    next = prev = -1;
    for (auto &e : effects)
      e.init();
  }
};

struct tLayoutPredef {
  struct { int lx, rx, ty, by; } texts[MAX_LAYOUT_PREDEFS];
  struct { int x, y; } bmps[MAX_LAYOUT_PREDEFS];
  std::string filename;
  int num_texts = 0, num_bmps = 0;
};

// Model globals.
extern int Briefing_root_screen;
extern tBriefScreen Briefing_screens[MAX_TELCOM_SCREENS];
extern BriefGlobalValues Briefing_globals;

// Layout predefs parsed from BriefingLayouts.txt.
extern tLayoutPredef *PBlayouts;
extern int *PBnum_layouts;

// Lifecycle.
void BriefEditInitScreens();
void BriefEditInitEffect(tBriefEffect *efx);
void BriefEditFreeScreens();
void BriefEditFreeEffect(tBriefEffect *efx);
void BriefEditFreeScreen(tBriefScreen *bscr);

// Allocate / resolve effects.
bool BriefEditAllocateEffect(int *ret_screen, int *ret_effect);
void BriefEditGetCurScreenEffect(int *ret_screen, int *ret_effect);

// Layout file parsing.
void ParseLayoutScreenFile(const std::filesystem::path &filename);

// .brf serialization (text command format compatible with the game).
bool BriefEditSaveScreens(const std::filesystem::path &filename, BriefGlobalValues *glob);
bool BriefEditLoadScreens(const std::filesystem::path &filename, BriefGlobalValues *glob);

#endif // BRIEF_MODEL_H
