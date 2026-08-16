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

namespace QtEditor {

// Qt port of the "Room" menu operations from editor/editorView.cpp's
// ON_COMMAND list (OnRoomAdd, OnRoomDelete, OnRoomMark, OnRoomRenameRoom,
// OnRoomSaveCurrentRoom, OnRoomSelectByNumber, OnRoomAddVertToNewFace,
// OnRoomStartNewFace, OnRoomFinishNewFace, OnRoomGrabTexture, OnRoomCombine,
// OnRoomSnapPointToEdge/ToPoint/ToFace, OnRoomPropagateToAll, OnRoomLinkTo-
// NewExternal, OnRoomBuildSmoothBridge, ...). The Win32 entry points live
// behind MFC + editor/HRoom.cpp + editor/HFile.cpp; on the Qt port the
// stdout + status-bar paths are kept and the geometry mutations go through
// editor/HRoom.cpp once qteditor links it.

// Drop a new cube room at Mine_origin (or the marked room's portal). Sets
// the new room as the current selection so the user's next Edit operation
// lands on it. Returns true on success; false if Mine_origin place is
// already used and the editor declined.
bool AddRoom();

// Forgets the current room: sets Curroomp = nullptr, Curface = Curedge =
// Curvert = Curportal = -1. The Win32 entry point also clears the marked
// room; we leave Markedroomp alone so a separate "Mark" operation stays
// authoritative.
bool DeleteRoom();

// Tag the current room as the "marked" one for legacy operations like
// OnRoomSwapMarkedAndCurrentRoomFace. Mirrors editor/selectedroom.cpp's
// SetMarkedRoom() (which uses the MFC keypad "Mark" button).
void MarkRoom();

// Mark-by-number: prompts the user for a room index and updates Curroomp.
// Returns the number entered or -1 if the dialog was cancelled.
int SelectRoomByNumber();

// Rename the current room. Pops a QInputDialog pre-filled with the
// existing name; returns true if the user picked a new value, false
// otherwise (cancellation or no change). Leading/trailing spaces are
// stripped in line with editor/HFile.cpp's StripLeadingTrailingSpaces().
bool RenameRoom();

// Save the current room to a piggyback .orf file next to the level.
// Mirrors editor/HRoom.cpp::SaveRoom (which writes a per-room snippet
// under the .d3l filename). Until the engine-side room walker ships, this
// is a status-bar-only stub that records what would have been written.
bool SaveCurrentRoom();

} // namespace QtEditor
