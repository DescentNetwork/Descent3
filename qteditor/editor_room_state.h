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
#include "vecmat.h"

struct face;
struct room;
struct roomUVL;

// Room selection list (editor/selectedroom.cpp in Win32).
int IsRoomSelected(int roomnum);
void AddRoomToSelectedList(int roomnum);
void RemoveRoomFromSelectedList(int roomnum);
void ClearRoomSelectedList();
int ToggleRoomSelectedState(int roomnum);
int SelectConnectedRooms(int roomnum);
void SaveRoomSelectedList();
void RestoreRoomSelectedList();


// Allocate / free an editor room slot. CreateNewRoom allocates the per-
// room verts[]/faces[]/portals[] arrays via new[] (DestroyRoom restores
// the heap); the Win32 editor/Erooms.cpp version uses its free-list
// allocator, which we don't replicate.
room *CreateNewRoom(int nverts, int nfaces, bool palette_room = false);
void DestroyRoom(int roomnum);

// Port of editor/Erooms.cpp:AssignDefaultUVsToRoomFace — projects each
// vertex onto the face's normal plane and assigns UVs with a 1/20.0 scale.
void AssignDefaultUVsToRoomFace(room *rp, int facenum);

// Port of editor/Erooms.cpp — room operations.
void CopyFace(face *dfp, face *sfp);
void CopyFaceFlags(face *dfp, face *sfp);
void CopyRoom(room *destp, room *srcp);
void ReInitRoomFace(face *fp, int nverts);
int RoomAddVertices(room *rp, int num_new_verts);
int RoomAddFaces(room *rp, int num_new_faces);
bool ResetRoomFaceNormals(room *rp);
bool FaceIsPlanar(int nv, int16_t *face_verts, vector3 *normal, vector3 *verts);
int CheckFaceConcavity(int num_verts, int16_t *face_verts, vector3 *normal, vector3 *verts);
bool FindSharedEdge(face *fp0, face *fp1, int *vn0, int *vn1);
void DeleteRoomFace(room *rp, int facenum);
void DeleteRoomPortal(room *rp, int portalnum);
int AddPortal(room *rp);
void LinkRooms(room *roomlist, int room0, int face0, int room1, int face1);
void AssignUVsToFace(room *rp, int facenum, roomUVL *uva, roomUVL *uvb, int va, int vb);
void AssignDefaultUVsToRoom(room *rp);
void FixConcaveFaces(room *rp, int *facelist, int facecount);
void FlipFace(room *rp, int facenum);

// Port of editor/HRoom.cpp — room operations.
bool CombineFaces(room *rp, int face0, int face1);
void DeletePortalPair(room *rp, int portalnum);
void RotateRooms(angle p, angle h, angle b);
void ConnectPortal(room *rp, int portal_num, int dest_room);
void DetachPortal(room *rp, int portal_num);
void AttachRoom();
void ComputePlacedRoomMatrix();
void PlaceRoom(room *baseroomp, int baseface, int placed_room, int placed_room_face, int placed_room_door);
void PlaceDoor(room *baseroomp, int baseface, int placed_door);

// Port of editor/RoomUVs.cpp and editor/HTexture.cpp — UV manipulation.
void GetUVLForRoomPoint(int roomnum, int facenum, int vertnum, roomUVL *uvl);
void StretchRoomUVs(room *rp, int facenum, int edge);
void ScaleFaceUVs(room *rp, int facenum, float scale);
void HTextureSlide(room *rp, int facenum, float right, float up);
void HTextureRotate(room *rp, int facenum, float angle_rad);
void HTextureFlipX(room *rp, int facenum);
void HTextureFlipY(room *rp, int facenum);
void HTextureRoomStretch(room *rp, int facenum, int edge, int direction);
void HTextureStretchMore(room *rp, int facenum, int edge, float texscale);
void HTextureStretchLess(room *rp, int facenum, int edge, float texscale);
void HTextureSetDefault(room *rp, int facenum);
int HTexturePropagateToFace(room *destrp, int destface, room *srcrp, int srcface, bool tex = true);
int HTextureCopyUVsToFace(room *destrp, int destface, room *srcrp, int srcface, int offset);
void HTextureApplyToRoomFace(room *rp, int facenum, int tnum);

