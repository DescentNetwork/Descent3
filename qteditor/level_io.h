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

// Cross-platform Qt-side replacements for the Win32 editor's
// CreateNewMine / EditorLoadLevel / EditorSaveLevel helpers from
// editor/HFile.cpp. Those live in the editor/ CMake target, which only
// builds on Windows; the Qt port on Linux needs stand-ins so the File menu
// is fully wired (New / Open / Save / Save As) without dragging in MFC.
//
// The stubs intentionally do nothing on the level data — the real mine load
// path lives in editor/HFile.cpp and depends on MFC state. We only need a
// safe symbol that lets the editor link on Linux. The Qt port will replace
// these with the real engine-side level reader once that lands.

namespace QtEditor {

// Trim leading and trailing spaces from a C string in place. Mirrors the
// Win32 helper in editor/HFile.cpp; exposed for tests so the contract
// (returns true iff anything was stripped) is pinned independently of
// EditorLoadLevel's pass-through call sites.
bool StripLeadingTrailingSpaces(char *s);

// Replace CEditorDoc::OnNewDocument. Win32: calls CreateNewMine() which
// wipes Rooms[], Terrain_segs[], etc. and seeds a single boot segment.
void CreateNewMine();

// Replace CEditorDoc::OnOpenDocument. Returns true on success.
bool EditorLoadLevel(const char *filename);

// Replace CEditorDoc::OnSaveDocument. Returns true on success (1) or 0.
int EditorSaveLevel(const char *filename);

// Verify that every named object/trigger/room in the current mine has a
// unique, well-formed name (no leading/trailing spaces). Calls from
// EditorLoadLevel after LoadLevel(). Routes duplicate-name and
// stripped-space reports to stderr.
void CheckLevelNames();

// Build the multi-line "Level Stats:" text block described in
// editor/HFile.cpp::ShowLevelStats(). Returns a heap-allocated buffer owned
// by the caller; delete[] when done. The Qt port hands this string to
// QMessageBox::information / a clipboard helper once the engine-side
// stats walker ships.
char *RenderLevelStats();

} // namespace QtEditor
