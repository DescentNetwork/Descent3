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

#include <filesystem>

std::filesystem::path orig_pwd;

#include <SDL3/SDL.h>

#include "appdatabase.h"
#include "application.h"
#include "args.h"
#include "d3_version.h"
#include "descent.h"
#include "init.h"
#include "lnxapp.h"
#include "program.h"

#ifdef LOGGER
#include "log.h"
#endif

namespace QtEditor {

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
}

}
