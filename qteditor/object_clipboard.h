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

// Qt port of editor/ObjectClipboard.cpp's global clipboard object. The
// legacy build holds a single `object ClipBoardObject` and a positional
// marker; here we expose typed accessors so the Edit>Cut/Copy/Paste menu
// handlers can route through them without dragging in MFC.
//
// The clipboard object is stored by-value in the editor module (see
// object_clipboard.cpp) so it persists across calls until PasteObject
// consumes it. The Win32 build clears the slot on copy; so do we, to
// match.
//
// Wait — the include path for `object` is Descent3/object.h, so the
// object struct is reachable from main_window.cpp via the
// "object.h" include there already.

void CopyObjectToClipboard();
void CutObjectToClipboard();
void PasteObjectFromClipboard();
bool HasClipboardObject();
void ClearClipboard();

} // namespace QtEditor
