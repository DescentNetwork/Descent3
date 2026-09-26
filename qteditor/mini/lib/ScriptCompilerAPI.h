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

#ifndef __SCRIPTCOMPILERAPI_H_
#define __SCRIPTCOMPILERAPI_H_

#include <functional>
#include <stdint.h>
#include <string>

#define ST_LEVEL 0
#define ST_GAME 1
struct tCompilerInfo {
  std::string source_filename;
  uint8_t script_type;
  std::function<void(char *str)> callback;
};

#define CERR_NOERR 0
#define CERR_NOCOMPILERDEFINED 1
#define CERR_COMPILERMISSING 2
#define CERR_SOURCENOEXIST 3

int ScriptCompile(tCompilerInfo *ci);
bool ScriptCreateEmptyScript(const std::string &filename, uint8_t script_type);
#endif
