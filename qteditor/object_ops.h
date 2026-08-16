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

// Qt implementations of the "Object" menu operations routed through
// editor/editorView.cpp::OnObjectPlaceObject / OnObjectDeleteObject /
// OnObjectSetCameraFromViewer / OnObjectPlaceCameraAtViewer /
// OnObjectSetViewerFromCamera / OnObjectMovePlayer. All six delegate
// down to Descent3Core's object API (object.h / Descent3/object.cpp).

// Drop a new camera object into the current room at the viewer's eye.
// Returns the object's index in Objects[] or -1 on failure. Mirrors the
// Win32 OnObjectPlaceCameraAtViewer handler closely enough that
// subsequent editor code (viewer-move-with-camera) keeps working.
int PlaceCameraAtViewer();

// Move the viewer's pose onto the camera object's pose so the editor
// "sees through" the camera. Sets Viewer_object->pos/orient/roomnum to
// the camera's and bumps Mine_changed.
void SetViewerFromCamera();

// Move the camera's pose onto the viewer's pose so the camera becomes
// a portable copy of where the user is currently looking.
void SetCameraFromViewer();

// Delete the currently-selected object (Cur_object_index). After the
// call, Cur_object_index is -1 and Mine_changed/New_mine are set.
void DeleteCurrentObject();

// Move the player (object 0) to the current room. Clears the player's
// orientation to Identity_matrix and resets its roomnum to ROOMNUM(Curroomp).
void MovePlayerToCurrentRoom();

// Reset Cur_object_index to the first used slot (or -1) so subsequent
// edits target something deterministic.
void SelectNextObject(int from);
void SelectPrevObject(int from);

} // namespace QtEditor
