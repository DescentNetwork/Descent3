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

#include "d3_editor_init.h"

#include <QSettings>

#include "appdatabase.h"
#include "application.h"
#include "args.h"
#include "d3_version.h"

#include "descent.h"
#include "editor_settings.h"
#include "gamedata_loader.h"
#include "init.h"
#include "lnxapp.h"
#include "program.h"

#include "d3edit.h"

#ifdef LOGGER
#include "log.h"
#endif


std::filesystem::path orig_pwd;

#if 0
void initD3Core(int argc, char *argv[]) {
  GatherArgs(argv);

  orig_pwd = std::filesystem::current_path();

#ifdef LOGGER
  InitLog(LogSeverity::debug, false, false);
#endif

  SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

  PreInitD3Systems();

  tLnxAppInfo appinfo{};
  appinfo.flags = APPFLAG_WINDOWEDMODE | APPFLAG_NOSHAREDMEMORY;
  Descent = new oeLnxApplication(&appinfo);
  Database = new oeLnxAppDatabase;

  ProgramVersion(DEVELOPMENT_VERSION, 0, 0, 0);

  InitD3Systems1(true);
  InitD3Systems2(true);

  // Pull the user's saved UI state on top of the zero defaults so Preferences
  // (and the texture/wireframe/keypad visibility flags) reflect what they
  // closed the editor with. Only loads keys that exist; an empty store is a
  // no-op equivalent to the Win32 "registry is empty" path.
  QSettings settings;
  loadEditorSettings(settings, D3EditState);

  errno = 0; // clear any errno states
}

#else

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
  Descent = new oeLnxApplication(&appinfo);
  Database = new oeLnxAppDatabase;

  ProgramVersion(DEVELOPMENT_VERSION, 0, 0, 0);

  InitD3Systems1(true);
  InitD3Systems2(true);

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
  loadEditorSettings(settings, D3EditState);

  errno = 0; // clear any errno states
}
#endif
