/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * Implementation of the Briefing editor data model (see brief_model.h).
 */

#include "brief_model.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

int Briefing_root_screen = -1;
tBriefScreen Briefing_screens[MAX_TELCOM_SCREENS];
BriefGlobalValues Briefing_globals;

tLayoutPredef *PBlayouts = nullptr;
int *PBnum_layouts = nullptr;

void BriefEditInitScreens() {
  Briefing_root_screen = -1;
  for (auto &bscr : Briefing_screens)
    bscr.init();
  Briefing_globals = BriefGlobalValues{};
}

void BriefEditInitEffect(tBriefEffect *efx) {
  if (efx)
    efx->init();
}

void BriefEditFreeScreen(tBriefScreen *bscr) {
  if (bscr == nullptr)
    return;
  for (auto &e : bscr->effects) {
    if (e.used)
      BriefEditFreeEffect(&e);
  }
  bscr->init();
}

void BriefEditFreeScreens() {
  for (auto &bscr : Briefing_screens)
    if (bscr.used)
      BriefEditFreeScreen(&bscr);
  Briefing_root_screen = -1;
  Briefing_globals = BriefGlobalValues{};
}

void BriefEditFreeEffect(tBriefEffect *efx) {
  if (efx == nullptr)
    return;
  efx->init();
}

// Finds the first free effect slot in `screen` and links it into the screen's
// effect list.  Returns false if no effect slot is free.
static bool alloc_effect_in_screen(tBriefScreen *scr, int *out_eff) {
  int chosen = -1;
  for (int i = 0; i < MAX_EFFECTS_PER_SCREEN; i++) {
    if (!scr->effects[i].used) {
      chosen = i;
      break;
    }
  }
  if (chosen == -1)
    return false;

  tBriefEffect *efx = &scr->effects[chosen];
  efx->init();
  efx->used = true;

  // Link into list (append after root).
  if (scr->root_effect == -1) {
    scr->root_effect = chosen;
    efx->prev = -1;
    efx->next = -1;
  } else {
    efx->prev = -1;
    efx->next = scr->root_effect;
    scr->effects[scr->root_effect].prev = chosen;
    scr->root_effect = chosen;
  }

  if (out_eff)
    *out_eff = chosen;
  return true;
}

bool BriefEditAllocateEffect(int *ret_screen, int *ret_effect) {
  int scr = -1;
  BriefEditGetCurScreenEffect(&scr, nullptr);
  if (scr < 0 || !Briefing_screens[scr].used) {
    for (int i = 0; i < MAX_TELCOM_SCREENS; i++) {
      if (Briefing_screens[i].used) {
        scr = i;
        break;
      }
    }
  }
  if (scr < 0)
    return false;
  if (ret_screen)
    *ret_screen = scr;
  return alloc_effect_in_screen(&Briefing_screens[scr], ret_effect);
}

void BriefEditGetCurScreenEffect(int *ret_screen, int *ret_effect) {
  int scr = (Briefing_root_screen >= 0) ? Briefing_root_screen : -1;
  if (scr < 0) {
    for (int i = 0; i < MAX_TELCOM_SCREENS; i++) {
      if (Briefing_screens[i].used) {
        scr = i;
        break;
      }
    }
  }
  if (ret_screen)
    *ret_screen = scr;
  if (ret_effect) {
    *ret_effect = (scr >= 0) ? Briefing_screens[scr].root_effect : -1;
  }
}

// Parses a BriefingLayouts.txt file into the PBlayouts / PBnum_layouts arrays.
void ParseLayoutScreenFile(const std::filesystem::path &filename) {
  if (PBlayouts) {
    delete[] PBlayouts;
    PBlayouts = nullptr;
  }
  if (PBnum_layouts) {
    delete[] PBnum_layouts;
    PBnum_layouts = nullptr;
  }

  std::ifstream in(filename);
  if (!in.is_open())
    return;

  std::vector<tLayoutPredef> layouts;
  tLayoutPredef cur;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      if (!cur.filename.empty()) {
        layouts.push_back(cur);
        cur = tLayoutPredef{};
      }
      continue;
    }
    if (line[0] == '[') {
      if (!cur.filename.empty()) {
        layouts.push_back(cur);
        cur = tLayoutPredef{};
      }
      std::string name = line.substr(1);
      auto close = name.find(']');
      if (close != std::string::npos)
        name.erase(close);
      cur.filename = name;
      continue;
    }
    std::istringstream ss(line);
    std::string cmd;
    ss >> cmd;
    if (cmd == "text" && cur.num_texts < MAX_LAYOUT_PREDEFS) {
      auto &t = cur.texts[cur.num_texts];
      ss >> t.lx >> t.ty >> t.rx >> t.by;
      cur.num_texts++;
    } else if (cmd == "bmp" && cur.num_bmps < MAX_LAYOUT_PREDEFS) {
      auto &b = cur.bmps[cur.num_bmps];
      ss >> b.x >> b.y;
      cur.num_bmps++;
    }
  }
  if (!cur.filename.empty())
    layouts.push_back(cur);

  if (layouts.empty())
    return;

  PBlayouts = new tLayoutPredef[layouts.size()];
  PBnum_layouts = new int[layouts.size()];
  for (size_t i = 0; i < layouts.size(); i++) {
    PBlayouts[i] = layouts[i];
    PBnum_layouts[i] = (int)layouts.size();
  }
}
// ---------------------------------------------------------------------------
// .brf serialization (text command format, compatible with the game).
// Ported from editor/BriefEdit.cpp.  Uses std::string / std::filesystem; no
// C strings, no CFILE.
// ---------------------------------------------------------------------------

static std::string missionFlagTokens(uint32_t mask, bool isSet) {
  std::ostringstream oss;
  int counter = 0;
  uint32_t bit = 0x01;
  for (int index = 0; index < 32; index++) {
    if (mask & bit) {
      oss << (isSet ? "isset " : "isnset ") << counter << " ";
    }
    counter++;
    bit <<= 1;
  }
  return oss.str();
}

// Maps a text-effect subtype (desc.type / desc.flags) to the "effect" word.
static std::string textEffectWord(const TCTEXTDESC &desc) {
  switch (desc.type) {
  case TC_TEXT_SCROLL:
    switch (desc.flags) {
    case TC_TEXTF_L2R: return "scroll_l2r";
    case TC_TEXTF_R2L: return "scroll_r2l";
    case TC_TEXTF_T2B: return "scroll_t2b";
    case TC_TEXTF_B2T: return "scroll_b2t";
    default: return "None";
    }
  case TC_TEXT_FADE:
    if (desc.flags == TC_TEXTF_OUT)
      return "fade_out";
    return "fade_in";
  case TC_TEXT_FLASH:
    return "flash";
  default:
    return "None";
  }
}

// Maps a bitmap-effect subtype to the "effect" word.
static std::string bmpEffectWord(const TCBMPDESC &desc) {
  std::string suffix = (desc.flags == TC_BMPF_OUT) ? "_out" : "_in";
  switch (desc.type) {
  case TC_BMP_BLUR: return "Blur" + suffix;
  case TC_BMP_SCANLINE: return "Scan" + suffix;
  case TC_BMP_INVERT: return "Invert" + suffix;
  case TC_BMP_STRETCH: return "Stretch" + suffix;
  default: return "None";
  }
}

static std::string fontWord(int font) {
  // BRIEF_FONT_INDEX vs BBRIEF_FONT_INDEX; both are 0/1 in practice.
  return (font == 1) ? "lg_brief" : "sm_brief";
}

bool BriefEditSaveScreens(const std::filesystem::path &filename, BriefGlobalValues *glob) {
  std::ofstream out(filename);
  if (!out.is_open())
    return false;

  if (glob == nullptr)
    glob = &Briefing_globals;

  out << "$title \"" << glob->title << "\"\n";
  out << "$glitch " << glob->glitch_val << "\n";
  out << "$static " << glob->static_val << "\n\n";

  int curr_screen = Briefing_root_screen;
  int screen_count = 0;
  while (curr_screen != -1) {
    if (curr_screen < 0 || curr_screen >= MAX_TELCOM_SCREENS || !Briefing_screens[curr_screen].used)
      break;
    tBriefScreen *scr = &Briefing_screens[curr_screen];

    std::string sm = missionFlagTokens(scr->mission_mask_set, true);
    std::string um = missionFlagTokens(scr->mission_mask_unset, false);

    out << "$screen " << screen_count;
    if (!sm.empty())
      out << " " << sm;
    if (!um.empty())
      out << " " << um;
    out << " desc \"Screen " << screen_count << "\"";
    if (!scr->layout.empty())
      out << " layout \"" << scr->layout << "\"";
    out << " start\n";

    int curr_effect = scr->root_effect;
    while (curr_effect != -1) {
      if (curr_effect < 0 || curr_effect >= MAX_EFFECTS_PER_SCREEN || !scr->effects[curr_effect].used)
        break;
      tBriefEffect *efx = &scr->effects[curr_effect];

      switch (efx->type) {
      case BE_TEXT: {
        TCTEXTDESC *desc = &efx->desc.text_desc;
        std::string red = std::to_string(GR_COLOR_RED(desc->color));
        std::string green = std::to_string(GR_COLOR_GREEN(desc->color));
        std::string blue = std::to_string(GR_COLOR_BLUE(desc->color));
        out << "$text effect " << textEffectWord(*desc) << " font " << fontWord(desc->font)
            << ((desc->caps & TCTD_TABSTOP) ? " tabstop" : "") << " speed " << desc->speed
            << " box " << desc->textbox.left << "," << desc->textbox.right << "," << desc->textbox.top << ","
            << desc->textbox.bottom << " color " << red << "," << green << "," << blue
            << " desc \"" << efx->description << "\" id " << efx->id << " starttime " << desc->waittime;
        std::string sm2 = missionFlagTokens(desc->mission_mask_set, true);
        std::string um2 = missionFlagTokens(desc->mission_mask_unset, false);
        if (!sm2.empty())
          out << " " << sm2;
        if (!um2.empty())
          out << " " << um2;
        out << " show\n";
        out << efx->text << "\n";
        out << "$endtext\n";
      } break;
      case BE_BMP: {
        TCBMPDESC *desc = &efx->desc.bmp_desc;
        out << "$bitmap effect " << bmpEffectWord(*desc) << " starttime " << desc->waittime;
        if (desc->flags & TC_NOEARLYRENDER)
          out << " no_early_rend";
        out << " speed " << desc->speed << " position " << desc->x << "," << desc->y;
        std::string sm2 = missionFlagTokens(desc->mission_mask_set, true);
        std::string um2 = missionFlagTokens(desc->mission_mask_unset, false);
        if (!sm2.empty())
          out << " " << sm2;
        if (!um2.empty())
          out << " " << um2;
        out << " desc \"" << efx->description << "\" show \"" << desc->filename << "\"\n";
      } break;
      case BE_MOVIE: {
        TCMOVIEDESC *desc = &efx->desc.movie_desc;
        out << "$movie position " << desc->x << "," << desc->y << " fps " << desc->fps
            << (desc->looping ? " looping" : "") << " starttime " << desc->waittime;
        std::string sm2 = missionFlagTokens(desc->mission_mask_set, true);
        std::string um2 = missionFlagTokens(desc->mission_mask_unset, false);
        if (!sm2.empty())
          out << " " << sm2;
        if (!um2.empty())
          out << " " << um2;
        out << " desc \"" << efx->description << "\" show \"" << desc->filename << "\"\n";
      } break;
      case BE_SND: {
        TCSNDDESC *desc = &efx->desc.snd_desc;
        out << "$sound " << (desc->once ? "once" : "") << " starttime " << desc->waittime;
        std::string sm2 = missionFlagTokens(desc->mission_mask_set, true);
        std::string um2 = missionFlagTokens(desc->mission_mask_unset, false);
        if (!sm2.empty())
          out << " " << sm2;
        if (!um2.empty())
          out << " " << um2;
        out << " desc \"" << efx->description << "\" play \"" << desc->filename << "\"\n";
      } break;
      case BE_BUTTON: {
        TCBUTTONDESC *desc = &efx->desc.button_desc;
        std::string ctype;
        switch (desc->click_type) {
        case 0: ctype = "ClickDown"; break;
        case 1: ctype = "ClickUp"; break;
        case 2: ctype = "HoldDown"; break;
        default: ctype = "ClickUp"; break;
        }
        std::string btype;
        switch (desc->button_type) {
        case 0: btype = "Up"; break;
        case 1: btype = "Down"; break;
        case 2: btype = "Next"; break;
        case 3: btype = "Prev"; break;
        case 4: btype = "Quit"; break;
        case 6: btype = "Jump " + std::to_string(desc->jump_page); break;
        default: btype = "Next"; break;
        }
        out << "$button " << desc->x << "," << desc->y << " type " << btype;
        if (desc->osflags & OBF_FLASH)
          out << " flash " << desc->flash_time << " \"" << desc->flash_filename << "\" \""
              << desc->flash_filename_focus << "\"";
        if (desc->osflags & OBF_GLOW)
          out << " glow";
        out << " click " << ctype << " id " << efx->id << " desc \"" << efx->description << "\" parent_id "
            << desc->parent_id << " sibling_id " << desc->sibling_id;
        std::string sm2 = missionFlagTokens(desc->mission_mask_set, true);
        std::string um2 = missionFlagTokens(desc->mission_mask_unset, false);
        if (!sm2.empty())
          out << " " << sm2;
        if (!um2.empty())
          out << " " << um2;
        out << " show \"" << desc->filename << "\" \"" << desc->filename_focus << "\"\n";
      } break;
      case BE_BKG:
      case BE_POLY:
      default:
        break;
      }

      curr_effect = scr->effects[curr_effect].next;
    }

    out << "$endscreen\n";
    curr_screen = scr->next;
    screen_count++;
  }

  out.close();
  return true;
}

// Simple line-based .brf loader that round-trips what BriefEditSaveScreens
// writes.  Screens are assigned by order; effects are appended to the current
// screen's list.
bool BriefEditLoadScreens(const std::filesystem::path &filename, BriefGlobalValues *glob) {
  std::ifstream in(filename);
  if (!in.is_open())
    return false;

  if (glob == nullptr)
    glob = &Briefing_globals;

  BriefEditFreeScreens();
  BriefEditInitScreens();

  int screen_index = 0;
  tBriefScreen *cur = nullptr;
  int cur_effect_slot = -1;
  bool in_text = false;
  std::string pending_text;

  auto flushScreen = [&]() { cur = nullptr; };

  std::string line;
  while (std::getline(in, line)) {
    if (in_text) {
      // Accumulate text-body lines until $endtext.
      if (line.rfind("$endtext", 0) == 0) {
        if (cur && cur_effect_slot >= 0) {
          std::string &t = cur->effects[cur_effect_slot].text;
          t = pending_text;
        }
        pending_text.clear();
        in_text = false;
      } else {
        if (!pending_text.empty())
          pending_text += "\n";
        pending_text += line;
      }
      continue;
    }

    std::istringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "$title") {
      std::string rest;
      std::getline(ss, rest);
      glob->title = rest;
      // strip surrounding quotes
      while (!glob->title.empty() && (glob->title.front() == '"' || glob->title.front() == ' '))
        glob->title.erase(glob->title.begin());
      while (!glob->title.empty() && (glob->title.back() == '"' || glob->title.back() == ' '))
        glob->title.pop_back();
    } else if (cmd == "$glitch") {
      ss >> glob->glitch_val;
    } else if (cmd == "$static") {
      ss >> glob->static_val;
    } else if (cmd == "$screen") {
      int idx = 0;
      ss >> idx;
      if (idx >= MAX_TELCOM_SCREENS)
        idx = MAX_TELCOM_SCREENS - 1;
      if (idx < 0)
        idx = screen_index;
      // Locate a free screen slot.
      int slot = idx;
      for (int i = 0; i < MAX_TELCOM_SCREENS; i++) {
        if (!Briefing_screens[i].used) {
          slot = i;
          break;
        }
      }
      tBriefScreen *scr = &Briefing_screens[slot];
      scr->init();
      scr->used = true;
      // parse layout + mission flags from the rest
      std::string rest;
      std::getline(ss, rest);
      // layout "..." extraction
      auto lit = rest.find("layout \"");
      if (lit != std::string::npos) {
        size_t start = lit + 8;
        size_t end = rest.find('"', start);
        if (end != std::string::npos)
          scr->layout = rest.substr(start, end - start);
      }
      if (Briefing_root_screen == -1)
        Briefing_root_screen = slot;
      cur = scr;
      cur_effect_slot = -1;
      screen_index++;
    } else if (cmd == "$endscreen") {
      flushScreen();
    } else if ((cmd == "$text" || cmd == "$bitmap" || cmd == "$movie" || cmd == "$sound" ||
                cmd == "$button") && cur) {
      // Allocate a new effect on the current screen.
      int eff = -1;
      if (!BriefEditAllocateEffect(nullptr, nullptr)) {
        // find a free slot manually and set it used
        for (int i = 0; i < MAX_EFFECTS_PER_SCREEN; i++) {
          if (!cur->effects[i].used) {
            eff = i;
            break;
          }
        }
      } else {
        // BriefEditAllocateEffect picks the current screen; locate the just-added
        // slot by scanning.
        for (int i = 0; i < MAX_EFFECTS_PER_SCREEN; i++)
          if (cur->effects[i].used && cur->effects[i].type == BE_NONE) {
            eff = i;
            break;
          }
      }
      if (eff < 0)
        eff = 0;
      tBriefEffect *efx = &cur->effects[eff];
      efx->init();
      efx->used = true;
      // hook into list as the new root
      if (cur->root_effect == -1) {
        cur->root_effect = eff;
      } else {
        efx->next = cur->root_effect;
        cur->effects[cur->root_effect].prev = eff;
        cur->root_effect = eff;
      }
      cur_effect_slot = eff;

      std::string rest;
      std::getline(ss, rest);

      if (cmd == "$text") {
        efx->type = BE_TEXT;
        TCTEXTDESC *desc = &efx->desc.text_desc;
        desc->caps = TCTD_FONT | TCTD_COLOR | TCTD_SPEED | TCTD_WAITTIME | TCTD_TEXTBOX | TCTD_SCROLL;
        // parse key fields
        auto pos = rest.find("font ");
        if (pos != std::string::npos) {
          std::istringstream f(rest.substr(pos + 5));
          std::string fw;
          f >> fw;
          desc->font = (fw == "lg_brief") ? 1 : 0;
        }
        pos = rest.find(" speed ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 7)) >> desc->speed;
        pos = rest.find(" box ");
        if (pos != std::string::npos) {
          std::istringstream b(rest.substr(pos + 5));
          char comma;
          b >> desc->textbox.left >> comma >> desc->textbox.right >> comma >> desc->textbox.top >> comma >>
              desc->textbox.bottom;
        }
        pos = rest.find(" starttime ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 10)) >> desc->waittime;
        if (rest.find("tabstop") != std::string::npos)
          desc->caps |= TCTD_TABSTOP;
        // color
        pos = rest.find(" color ");
        if (pos != std::string::npos) {
          std::istringstream c(rest.substr(pos + 7));
          int r, g, b;
          char comma;
          c >> r >> comma >> g >> comma >> b;
          desc->color = GR_RGB(r, g, b);
        }
        pos = rest.find(" desc \"");
        if (pos != std::string::npos) {
          size_t s = pos + 7;
          size_t e = rest.find('"', s);
          if (e != std::string::npos)
            efx->description = rest.substr(s, e - s);
        }
        pos = rest.find(" id ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 4)) >> efx->id;
        // effect word -> desc.type/flags
        if (rest.find("scroll_l2r") != std::string::npos) { desc->type = TC_TEXT_SCROLL; desc->flags = TC_TEXTF_L2R; }
        else if (rest.find("scroll_r2l") != std::string::npos) { desc->type = TC_TEXT_SCROLL; desc->flags = TC_TEXTF_R2L; }
        else if (rest.find("scroll_t2b") != std::string::npos) { desc->type = TC_TEXT_SCROLL; desc->flags = TC_TEXTF_T2B; }
        else if (rest.find("scroll_b2t") != std::string::npos) { desc->type = TC_TEXT_SCROLL; desc->flags = TC_TEXTF_B2T; }
        else if (rest.find("fade_in") != std::string::npos) { desc->type = TC_TEXT_FADE; desc->flags = TC_TEXTF_IN; }
        else if (rest.find("fade_out") != std::string::npos) { desc->type = TC_TEXT_FADE; desc->flags = TC_TEXTF_OUT; }
        else if (rest.find("flash") != std::string::npos) { desc->type = TC_TEXT_FLASH; }
        else { desc->type = TC_TEXT_STATIC; }
        in_text = true;
        pending_text.clear();
      } else if (cmd == "$bitmap") {
        efx->type = BE_BMP;
        TCBMPDESC *desc = &efx->desc.bmp_desc;
        auto pos = rest.find(" starttime ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 10)) >> desc->waittime;
        pos = rest.find(" speed ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 7)) >> desc->speed;
        pos = rest.find(" position ");
        if (pos != std::string::npos) {
          std::istringstream p(rest.substr(pos + 9));
          char comma;
          p >> desc->x >> comma >> desc->y;
        }
        if (rest.find("no_early_rend") != std::string::npos)
          desc->flags |= TC_NOEARLYRENDER;
        pos = rest.find(" desc \"");
        if (pos != std::string::npos) {
          size_t s = pos + 7;
          size_t e = rest.find('"', s);
          if (e != std::string::npos)
            efx->description = rest.substr(s, e - s);
        }
        pos = rest.find(" show \"");
        if (pos != std::string::npos) {
          size_t s = pos + 7;
          size_t e = rest.find('"', s);
          std::string fn = (e != std::string::npos) ? rest.substr(s, e - s) : std::string();
          if (fn.size() >= MAX_FILELEN)
            fn = fn.substr(0, MAX_FILELEN - 1);
          std::memcpy(desc->filename, fn.c_str(), fn.size());
          desc->filename[std::min<std::size_t>(fn.size(), MAX_FILELEN - 1)] = 0;
        }
      } else if (cmd == "$movie") {
        efx->type = BE_MOVIE;
        TCMOVIEDESC *desc = &efx->desc.movie_desc;
        auto pos = rest.find(" position ");
        if (pos != std::string::npos) {
          std::istringstream p(rest.substr(pos + 9));
          char comma;
          p >> desc->x >> comma >> desc->y;
        }
        pos = rest.find(" fps ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 5)) >> desc->fps;
        pos = rest.find(" starttime ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 10)) >> desc->waittime;
        desc->looping = (rest.find("looping") != std::string::npos);
        pos = rest.find(" show \"");
        if (pos != std::string::npos) {
          size_t s = pos + 7;
          size_t e = rest.find('"', s);
          std::string fn = (e != std::string::npos) ? rest.substr(s, e - s) : std::string();
          std::memcpy(desc->filename, fn.c_str(), std::min<std::size_t>(fn.size(), MAX_FILELEN - 1));
          desc->filename[std::min<std::size_t>(fn.size(), MAX_FILELEN - 1)] = 0;
        }
      } else if (cmd == "$sound") {
        efx->type = BE_SND;
        TCSNDDESC *desc = &efx->desc.snd_desc;
        auto pos = rest.find(" starttime ");
        if (pos != std::string::npos)
          std::istringstream(rest.substr(pos + 10)) >> desc->waittime;
        desc->once = (rest.find("once") != std::string::npos);
        pos = rest.find(" play \"");
        if (pos != std::string::npos) {
          size_t s = pos + 7;
          size_t e = rest.find('"', s);
          std::string fn = (e != std::string::npos) ? rest.substr(s, e - s) : std::string();
          std::memcpy(desc->filename, fn.c_str(), std::min<std::size_t>(fn.size(), MAX_FILELEN - 1));
          desc->filename[std::min<std::size_t>(fn.size(), MAX_FILELEN - 1)] = 0;
        }
      }
      // (button omitted in parse for now; round-trip focuses on text/bmp/movie/sound)
    }
  }

  return true;
}
