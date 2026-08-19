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

struct room;

// Returns true if roomnum is in the selected-room list.
int IsRoomSelected(int roomnum);


// Allocate / free an editor room slot. CreateNewRoom allocates the per-
// room verts[]/faces[]/portals[] arrays via new[] (DestroyRoom restores
// the heap); the Win32 editor/Erooms.cpp version uses its free-list
// allocator, which we don't replicate.
room *CreateNewRoom(int nverts, int nfaces, bool palette_room = false);
void DestroyRoom(int roomnum);

// Port of editor/Erooms.cpp:AssignDefaultUVsToRoomFace — projects each
// vertex onto the face's normal plane and assigns UVs with a 1/20.0 scale.
void AssignDefaultUVsToRoomFace(room *rp, int facenum);

