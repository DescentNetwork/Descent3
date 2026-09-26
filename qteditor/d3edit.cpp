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

// Editor-wide state that the original MFC editor keeps in EDVARS.cpp/editor.cpp.
// The D3 core is compiled without the EDITOR define, so this is re-provided
// here for the Qt port.

#include <QSettings>
#include <QtGlobal>
#include <QMessageBox>
#include <QApplication>
#include <cstdarg>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <memory>
#include <algorithm>

#include "editor_room_state.h"
#include "mem/mem.h"
#include "moveworld.h"

#include "vecmat.h"
#include "terrain.h"
#include "slew.h"
#include "manage.h"
#include "doorway.h"

#include "objinfo.h"
#include "gamepath.h"
#include "findintersection.h"
#include "room.h"

#include "appdatabase.h"
#include "application.h"
#include "d3_version.h"

#include "descent.h"
#include "editor_settings.h"
#include "gamedata_loader.h"
#include "init.h"
#include "lnxapp.h"
#include "program.h"

#include "d3edit.h"

#include "lightmap_info.h"

#ifdef LOGGER
#include "log.h"
#endif


#define MAX_ARGS 30
#define MAX_CHARS_PER_ARG 100
char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];


// Command-line argument store. GatherArgs collects the argv tokens (including
// argv[0] as index 0) so FindArg/GetArg can be used to pass options such as
// "-datadir <path>" to locate the game data files.

void GatherArgs(char **argv) {
  if (argv == nullptr)
    return;
  int n = 0;
  for (int i = 0; argv[i] && i < MAX_ARGS; i++) {
    std::strncpy(GameArgs[n++], argv[i], MAX_CHARS_PER_ARG - 1);
    GameArgs[n - 1][MAX_CHARS_PER_ARG - 1] = '\0';
  }
}

void GatherArgs(const char *str) {
  if (str == nullptr)
    return;
  int n = 0;
  const char *p = str;
  while (*p && n < MAX_ARGS) {
    while (*p == ' ')
      p++;
    if (!*p)
      break;
    int len = 0;
    while (p[len] && p[len] != ' ' && len < MAX_CHARS_PER_ARG - 1)
      len++;
    std::memcpy(GameArgs[n], p, len);
    GameArgs[n][len] = '\0';
    n++;
    p += len;
  }
}

int FindArg(const char *which, int start = 0) {
  if (which == nullptr)
    return 0;
  for (int i = start; i < MAX_ARGS; i++) {
    if (GameArgs[i][0] && strcasecmp(GameArgs[i], which) == 0)
      return i;
  }
  return 0;
}

int FindArgChar(const char *which, char singleCharArg) { return FindArg(which); }

const char *GetArg(int index) {
  if (index < 0 || index >= MAX_ARGS)
    return "";
  return GameArgs[index];
}

// Minimal pre-init: establish the memory and error subsystems that the
// ported engine code relies on before anything else runs.
void PreInitD3Systems() {
  if (FindArg("-lowmem") || FindArg("-dedicated"))
    Mem_low_memory_mode = true;
  if (FindArg("-superlowmem")) {
    Mem_low_memory_mode = true;
    Mem_superlow_memory_mode = true;
  }
  if (FindArg("-himem")) {
    Mem_low_memory_mode = false;
    Mem_superlow_memory_mode = false;
  }
}



d3edit_state app;

// Slew movement limitations flag (defined in the MFC editor's editor.cpp).
int Slew_limitations = 0;

// Editor-side lighting globals (editor_lighting.cpp / rad_init.cpp in MFC).

int Shoot_from_patch = 1;

// Editor room/face/portal editing context (defined in the MFC editor).


// Wireframe (chase-cam) views declared in moveworld.h; now defined here so the
// EDIT level chunk can round-trip them.
wireframe_view Wireframe_view_mine = {IDENTITY_MATRIX, {0, 0, 0}, 0, 0};
wireframe_view Wireframe_view_room = {IDENTITY_MATRIX, {0, 0, 0}, 0, 0};



// SLEW.cpp guards SlewControlInit() with EDITOR; slew.cpp provides it.



// SaveLevel lives in Descent3/LoadLevel.cpp but its definition #includes
// "editor/ebnode.h" mid-file; Descent3Core doesn't have editor/ in its
// include path, so the symbol never makes it into libDescent3Core.a. We
// stub it at editor-side scope so the Qt port's level_io.cpp can keep
// EditorSaveLevel's contract (success → true) until the engine path lands.
// EditorStatus/SetErrorMessage/GetErrorMessage live in editor/MainFrm.cpp which
// is not linked into the Qt port.  Provide lightweight implementations here.
static char Editor_error_message[512] = "";

void EditorStatus(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf(Editor_error_message, sizeof(Editor_error_message), format, args);
  va_end(args);
}

void SetErrorMessage(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(Editor_error_message, sizeof(Editor_error_message), fmt, args);
  va_end(args);
}

const char *GetErrorMessage() { return Editor_error_message; }




std::filesystem::path orig_pwd;

// Try to locate the directory that contains the game data files (d3.hog).
// The user can override via -datadir <path>; otherwise a small set of common
// install locations (including this machine's known path) is probed.
static std::filesystem::path FindGameDataDir() {
  // Explicit command-line override wins.
  int arg = FindArg("-datadir");
  if (arg) {
    std::filesystem::path p = GetArg(arg + 1);
    if (std::filesystem::exists(p / "d3.hog"))
      return p;
  }

  const std::filesystem::path candidates[] = {
      "/mnt/media/games/pc/Descent 3",
      "/usr/share/descent3",
      "/usr/local/share/descent3",
  };
  for (const auto &c : candidates) {
    if (std::filesystem::exists(c / "d3.hog"))
      return c;
  }
  return {};
}

void initD3Core(int argc, char *argv[]) {
  GatherArgs(argv);

  orig_pwd = std::filesystem::current_path();

#ifdef LOGGER
  InitLog(LogSeverity::debug, false, false);
#endif

  // SDL initialization removed - using Qt for window management
  PreInitD3Systems();

  tLnxAppInfo appinfo{};
  appinfo.flags = APPFLAG_WINDOWEDMODE | APPFLAG_NOSHAREDMEMORY;
  Descent = std::make_unique<oeLnxApplication>(&appinfo);
  Database = std::make_unique<oeLnxAppDatabase>();

  ProgramVersion(DEVELOPMENT_VERSION, 0, 0, 0);

  InitLightmapInfo();
  InitRooms();
  ResetObjectList();

  // Load the gamedata tables (d3.hog -> Table.gam) so levels opening later
  // can reference object/ship/weapon/sound/texture metadata. This is what the
  // Win32 editor does during startup; without it the level's referenced data
  // is unavailable.
  {
    std::filesystem::path data_dir = FindGameDataDir();
    if (!data_dir.empty()) {
      loadGameDataTable(data_dir / "d3.hog");
    }
  }

  // Pull the user's saved UI state on top of the zero defaults so Preferences
  // (and the texture/wireframe/keypad visibility flags) reflect what they
  // closed the editor with. Only loads keys that exist; an empty store is a
  // no-op equivalent to the Win32 "registry is empty" path.
  QSettings settings;
  loadEditorSettings(settings, app);

  errno = 0; // clear any errno states
}
