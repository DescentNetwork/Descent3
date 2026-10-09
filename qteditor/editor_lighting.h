/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

#ifndef EDITOR_LIGHTING_H
#define EDITOR_LIGHTING_H

#include "3d.h"
#include "radiosity.h"
#include "room.h"
#include "object.h"
#include "lightmap_info.h"

extern vector3 ScratchCenters[MAX_LIGHTMAP_INFOS];
extern vector3 ScratchRVecs[MAX_LIGHTMAP_INFOS];
extern vector3 ScratchUVecs[MAX_LIGHTMAP_INFOS];
extern int AllowCombining;

extern std::array<room_light_t, MAX_ROOMS + MAX_PALETTE_ROOMS> room_light;
extern room_light_t global_light;
extern int Lightmaps_for_rad;

void DoRadiosityForRooms();
void DoRadiosityForTerrain();

bool SaveLevel(const std::filesystem::path& filename, bool f_save_room_AABB = true);

void BuildElementListForRoomFace(int roomnum, int facenum, rad_surface *surf);
void BuildElementListForObjectFace(object& obj, int subobj_num, int facenum, rad_surface *surf);

void AssignRoomSurfaceToLightmap(int roomnum, int facenum, rad_surface *sp);
void AssignObjectSurfaceToLightmap(int objnum, int sobj_num, int facenum, rad_surface *sp);
void AssignLightmapsToObjectSurfacesForSingleRoom(int surface_index, int roomnum);

int ClipRadPointList(rad_point **src, rad_point **dest, int *nv, g3_clip_codes_t code);
int ClipRadToPlane(int plane, rad_point *src, rad_point *dest, int nv);
void ClipRadEdge(g3_clip_codes_t plane_flag, rad_point *on_pnt, rad_point *off_pnt);
void SetRadClipLines(vector3 *tp, vector3 *rp, vector3 *bp, vector3 *lp);
void ClipSurfaceElement(vector3 *surf_verts, rad_element *ep, vector3 *clip_verts, int nv);

void BuildLightmapUVs(int *room_list, int *face_list, int count, vector3 *lightmap_poly, int nv, int external);
void BuildObjectLightmapUVs(object& obj, int *sublist, int *facelist, int count, vector3 *lightmap_poly, int nv, lmi_type lm_type);

int CombineLightFaces(vector3 *dest_verts, vector3 *averts, int nva, vector3 *norma, vector3 *bverts, int nvb, vector3 *normb, int aroom = -1, int broom = -1);
int TestLightAdjacency(int roomnum, int facenum, int external);

int ComputeSurfacesForObjectsForSingleRoom(int surface_index, int roomnum);
void ComputeAllRoomLightmapUVs(int external);
void ComputeRoomLightmapUVs(int roomnum);
int ComputeSurfacesForObjects(int surface_index, int terrain);

int GetTotalObjectFaces(int terrain);
int GetTotalObjectFacesForSingleRoom(int roomnum);

void AssignLightmapsToObjectSurfaces(int surface_index, int terrain);
void CombineObjectLightmapUVs(object& obj, lmi_type type);

void CleanupSpecularLighting(int external);
void SetupSpecularLighting(int external);
void DoRadiosityForCurrentRoom(int roomnum);

void SqueezeLightmaps(int external, int target_roomnum);
void CheckCombinePortals(int terrain);
void AssignVolumeSpectraToRoom(int roomnum);
void ComputeSurfaceRes(rad_surface *surf, int roomnum, int facenum);
void ComputeObjectSurfaceRes(rad_surface *surf, object *obj, int subnum, int facenum);
int CheckForBadFaces(int roomnum);
void AddSpectra(spectra *dest, spectra *a, spectra *b);
int SpecularPrevIndex(int val, int modulus);
int SpecularNextIndex(int val, int modulus);

extern std::vector<rad_surface> Light_surfaces;
extern std::vector<rad_surface> Terrain_light_surfaces;

extern uint8_t *TerrainLightSpeedup[];

extern int Square_surfaces;
extern int LightSpacing;
extern bool BestFit;

bool PointsAreSame(vector3 *v0, vector3 *v1);

#endif
