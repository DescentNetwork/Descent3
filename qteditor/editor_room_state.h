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

#include "utils.h"


struct face_t;
struct roomUVL;

// Vars for the list of selected rooms
extern int N_selected_rooms;
extern std::array<index_t, MAX_ROOMS> Selected_rooms;


// Room selection list (editor/selectedroom.cpp in Win32).
int IsRoomSelected(index_t roomnum);
void AddRoomToSelectedList(index_t roomnum);
void RemoveRoomFromSelectedList(index_t roomnum);
void ClearRoomSelectedList();
int ToggleRoomSelectedState(index_t roomnum);
int SelectConnectedRooms(index_t roomnum);
void SaveRoomSelectedList();
void RestoreRoomSelectedList();


// Allocate / free an editor room slot.  CreateNewRoom provisions a slot in the
// Rooms table (first unused slot, else appended at the high-water mark) and
// returns its index, or nullopt when the room capacity limit is reached.  It mirrors
// CreateNewRoom() in the Win32 editor/Erooms.cpp, whose free-list allocator we
// don't replicate.
index_t CreateNewRoom(int nverts, int nfaces, bool palette_room = false);
void DestroyRoom(index_t roomnum);

// Mirrors GetFreeRoom(): linear scan for the first unused slot, else append a
// fresh slot at the end of Rooms (which is the high-water mark + 1).  Returns
// the slot index or -1 when the room capacity limit is reached.
int FindFreeRoomSlot();

// Port of editor/Erooms.cpp:AssignDefaultUVsToRoomFace — projects each
// vertex onto the face's normal plane and assigns UVs with a 1/20.0 scale.
void AssignDefaultUVsToRoomFace(index_t roomnum, index_t facenum);

// Port of editor/Erooms.cpp — room operations.
void CopyFace(face_t *dfp, face_t *sfp);
void CopyFaceFlags(face_t *dfp, face_t *sfp);
void CopyRoom(int destroom, int srcroom);
void ReInitRoomFace(face_t *fp, int nverts);
int RoomAddVertices(index_t roomnum, int num_new_verts);
int RoomAddFaces(index_t roomnum, int num_new_faces);
bool ResetRoomFaceNormals(index_t roomnum);
bool FaceIsPlanar(int nv, std::vector<int16_t>& face_verts, vector3& normal, std::vector<vector3>& verts);
int CheckFaceConcavity(int num_verts, std::vector<int16_t>& face_verts, vector3& normal, std::vector<vector3>& verts);
bool FindSharedEdge(face_t *fp0, face_t *fp1, int *vn0, int *vn1);
void DeleteRoomFace(index_t roomnum, index_t facenum);
void DeleteRoomPortal(index_t roomnum, index_t portalnum);
int AddPortal(index_t roomnum);
void LinkRooms(int room0, int face0, int room1, int face1);
void AssignUVsToFace(index_t roomnum, index_t facenum, roomUVL *uva, roomUVL *uvb, int va, int vb);
void AssignDefaultUVsToRoom(index_t roomnum);
void FixConcaveFaces(index_t roomnum, int *facelist, int facecount);
void FlipFace(index_t roomnum, index_t facenum);

// Port of editor/HRoom.cpp — room operations.
bool CombineFaces(index_t roomnum, index_t face0, index_t face1);
void DeletePortalPair(index_t roomnum, index_t portalnum);
void RotateRooms(angle p, angle h, angle b);
void ConnectPortal(index_t roomnum, int portal_num, int dest_room);
void DetachPortal(index_t roomnum, int portal_num);
void AttachRoom();
void ComputePlacedRoomMatrix();
void PlaceRoom(int baseroom, int baseface, int placed_room, int placed_room_face, index_t placed_room_door);
void PlaceDoor(int baseroom, int baseface, uint32_t placed_door);

// Port of editor/RoomUVs.cpp and editor/HTexture.cpp — UV manipulation.
void GetUVLForRoomPoint(index_t roomnum, index_t facenum, int vertnum, roomUVL *uvl);
void StretchRoomUVs(index_t roomnum, index_t facenum, int edge);
void ScaleFaceUVs(index_t roomnum, index_t facenum, float scale);
void HTextureSlide(index_t roomnum, index_t facenum, float right, float up);
void HTextureRotate(index_t roomnum, index_t facenum, float angle_rad);
void HTextureFlipX(index_t roomnum, index_t facenum);
void HTextureFlipY(index_t roomnum, index_t facenum);
void HTextureRoomStretch(index_t roomnum, index_t facenum, index_t edge, int direction);
void HTextureStretchMore(index_t roomnum, index_t facenum, index_t edge, float texscale);
void HTextureStretchLess(index_t roomnum, index_t facenum, index_t edge, float texscale);
void HTextureSetDefault(index_t roomnum, index_t facenum);
int HTexturePropagateToFace(index_t destroom, index_t destface, index_t srcroom, index_t srcface, bool tex = true);
int HTextureCopyUVsToFace(index_t destroom, index_t destface, index_t srcroom, index_t srcface, int offset);
void HTextureApplyToRoomFace(index_t roomnum, index_t facenum, int tnum);

