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

#include "fix.h"
#include "vecmat_external.h"
#include "room_external.h"

struct face;
struct roomUVL;

// Vars for the list of selected rooms
extern int N_selected_rooms;
extern std::array<int, MAX_ROOMS> Selected_rooms;


// Room selection list (editor/selectedroom.cpp in Win32).
int IsRoomSelected(int roomnum);
void AddRoomToSelectedList(int roomnum);
void RemoveRoomFromSelectedList(int roomnum);
void ClearRoomSelectedList();
int ToggleRoomSelectedState(int roomnum);
int SelectConnectedRooms(int roomnum);
void SaveRoomSelectedList();
void RestoreRoomSelectedList();


// Allocate / free an editor room slot.  CreateNewRoom provisions a slot in the
// Rooms table (first unused slot, else appended at the high-water mark) and
// returns its index, or -1 when the room capacity limit is reached.  It mirrors
// CreateNewRoom() in the Win32 editor/Erooms.cpp, whose free-list allocator we
// don't replicate.
int CreateNewRoom(int nverts, int nfaces, bool palette_room = false);
void DestroyRoom(int roomnum);

// Mirrors GetFreeRoom(): linear scan for the first unused slot, else append a
// fresh slot at the end of Rooms (which is the high-water mark + 1).  Returns
// the slot index or -1 when the room capacity limit is reached.
int FindFreeRoomSlot();

// Port of editor/Erooms.cpp:AssignDefaultUVsToRoomFace — projects each
// vertex onto the face's normal plane and assigns UVs with a 1/20.0 scale.
void AssignDefaultUVsToRoomFace(int roomnum, int facenum);

// Port of editor/Erooms.cpp — room operations.
void CopyFace(face *dfp, face *sfp);
void CopyFaceFlags(face *dfp, face *sfp);
void CopyRoom(int destroom, int srcroom);
void ReInitRoomFace(face *fp, int nverts);
int RoomAddVertices(int roomnum, int num_new_verts);
int RoomAddFaces(int roomnum, int num_new_faces);
bool ResetRoomFaceNormals(int roomnum);
bool FaceIsPlanar(int nv, std::vector<int16_t>& face_verts, vector3& normal, std::vector<vector3>& verts);
int CheckFaceConcavity(int num_verts, std::vector<int16_t>& face_verts, vector3& normal, std::vector<vector3>& verts);
bool FindSharedEdge(face *fp0, face *fp1, int *vn0, int *vn1);
void DeleteRoomFace(int roomnum, int facenum);
void DeleteRoomPortal(int roomnum, int portalnum);
int AddPortal(int roomnum);
void LinkRooms(int room0, int face0, int room1, int face1);
void AssignUVsToFace(int roomnum, int facenum, roomUVL *uva, roomUVL *uvb, int va, int vb);
void AssignDefaultUVsToRoom(int roomnum);
void FixConcaveFaces(int roomnum, int *facelist, int facecount);
void FlipFace(int roomnum, int facenum);

// Port of editor/HRoom.cpp — room operations.
bool CombineFaces(int roomnum, int face0, int face1);
void DeletePortalPair(int roomnum, int portalnum);
void RotateRooms(angle p, angle h, angle b);
void ConnectPortal(int roomnum, int portal_num, int dest_room);
void DetachPortal(int roomnum, int portal_num);
void AttachRoom();
void ComputePlacedRoomMatrix();
void PlaceRoom(int baseroom, int baseface, int placed_room, int placed_room_face, int placed_room_door);
void PlaceDoor(int baseroom, int baseface, int placed_door);

// Port of editor/RoomUVs.cpp and editor/HTexture.cpp — UV manipulation.
void GetUVLForRoomPoint(int roomnum, int facenum, int vertnum, roomUVL *uvl);
void StretchRoomUVs(int roomnum, int facenum, int edge);
void ScaleFaceUVs(int roomnum, int facenum, float scale);
void HTextureSlide(int roomnum, int facenum, float right, float up);
void HTextureRotate(int roomnum, int facenum, float angle_rad);
void HTextureFlipX(int roomnum, int facenum);
void HTextureFlipY(int roomnum, int facenum);
void HTextureRoomStretch(int roomnum, int facenum, int edge, int direction);
void HTextureStretchMore(int roomnum, int facenum, int edge, float texscale);
void HTextureStretchLess(int roomnum, int facenum, int edge, float texscale);
void HTextureSetDefault(int roomnum, int facenum);
int HTexturePropagateToFace(int destroom, int destface, int srcroom, int srcface, bool tex = true);
int HTextureCopyUVsToFace(int destroom, int destface, int srcroom, int srcface, int offset);
void HTextureApplyToRoomFace(int roomnum, int facenum, int tnum);

