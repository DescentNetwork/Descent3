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

class QSettings;

struct d3edit_state;

namespace QtEditor {

// Qt port of editor.cpp::SaveEditorSettings / LoadEditorSettings. The Win32
// helpers wrote every D3EditState field to the Windows registry via
// oeLnxAppDatabase; the Qt port writes the same field set to a QSettings
// store, so on Linux it lands under
// ~/.config/DescentDevelopers/Descent 3 Editor.conf, on macOS in a property
// list, and on Windows in the same registry hive the Win32 editor used.
//
// The QSettings reference is taken by reference so callers (the test suite
// in particular) can inject an isolated INI-backed store without having to
// mutate the global app name. Pass `QSettings()` to use the default
// organisation/application the QApplication already provides.

void saveEditorSettings(QSettings &settings, const d3edit_state &state);
void loadEditorSettings(QSettings &settings, d3edit_state &state);

} // namespace QtEditor
