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

#pragma once
#include <filesystem>

static inline std::filesystem::path original_pwd(void)
{
  extern std::filesystem::path orig_pwd;
  return orig_pwd;
}

// Initializes the Descent 3 core in editor mode, mirroring the original MFC
// editor's startup sequence (CMainFrame::OnCreateClient). Must be called once
// after the QApplication has been constructed.
void initD3Core(int argc, char *argv[]);

