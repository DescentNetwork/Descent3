// Stub implementations for all symbols referenced by qteditor
// but not provided by any source in mini/.

#include "pstypes.h"
#include "fix.h"
#include "vecmat.h"
#include "gr.h"
#include "object.h"
#include "room.h"
#include "terrain.h"
#include "gametexture.h"
#include "polymodel.h"
#include "lightmap_info.h"
#include "cfile.h"
#include "3d.h"
#include "findintersection.h"
#include "Mission.h"
#include "aiambient.h"
#include "matcen.h"
#include "gamepath.h"
#include "soundload.h"
#include "doorway.h"
#include "weapon.h"
#include "ship.h"
#include "megacell.h"
#include "trigger.h"
#include "BOA.h"
#include "bnode.h"
#include "bsp.h"
#include "manage.h"
#include "mem.h"
#include "game.h"
#include "physics.h"
#include "lightmap.h"
#include "objinfo.h"
#include "vclip.h"
#include "bitmap.h"
#include "special_face.h"
#include "ssl_lib.h"
#include "hlsoundlib.h"
#include "levelgoal.h"
#include "d3edit.h"
#include "lnxdatabase.h"
#include "door.h"
#include "doorpage.h"
#include "genericpage.h"
#include "shippage.h"
#include "soundpage.h"
#include "texpage.h"
#include "weaponpage.h"
#include "object_external_struct.h"
#include "joystick.h"
#include "chrono_timer.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdarg>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==================== Vecmat ====================
const matrix Identity_matrix = matrix::id();
void vm_MakeIdentity(matrix *m) { *m = Identity_matrix; }
void vm_TransposeMatrix(matrix *m) {
  if (m == nullptr)
    return;
  scalar t;
  t = m->uvec.x();
  m->uvec.x() = m->rvec.y();
  m->rvec.y() = t;
  t = m->fvec.x();
  m->fvec.x() = m->rvec.z();
  m->rvec.z() = t;
  t = m->fvec.y();
  m->fvec.y() = m->uvec.z();
  m->uvec.z() = t;
}
void vm_Orthogonalize(matrix *m) {
  if (m == nullptr)
    return;
  if (vm_NormalizeVector(&m->fvec) == 0)
    return;
  m->rvec = vm_Cross3Product(m->uvec, m->fvec);
  if (vm_NormalizeVector(&m->rvec) == 0) {
    vm_VectorToMatrix(m, &m->fvec, nullptr, nullptr);
    return;
  }
  m->uvec = vm_Cross3Product(m->fvec, m->rvec);
}
scalar vm_GetMagnitude(const vector *v) { return v == nullptr ? 0 : v->mag(); }
scalar vm_GetMagnitudeFast(const vector *v) { return v == nullptr ? 0 : v->mag(); }
void vm_CrossProduct(vector *result, const vector *a, const vector *b) { *result = vm_Cross3Product(*a, *b); }
scalar vm_DotProduct(const vector *a, const vector *b) { return vm_Dot3Product(*a, *b); }
scalar vm_VectorDistance(const vector *a, const vector *b) {
  if (a == nullptr || b == nullptr)
    return 0;
  return (*a - *b).mag();
}
scalar vm_VectorDistanceQuick(const vector *a, const vector *b) { return vm_VectorDistance(a, b); }
scalar vm_NormalizeVector(vector *v) {
  if (v == nullptr)
    return 0;
  const scalar m = vm_GetMagnitude(v);
  if (m > 0.000001f)
    *v = *v * (1.0f / m);
  return m;
}
scalar vm_NormalizeVectorFast(vector *v) { return vm_NormalizeVector(v); }
void vm_SubVectors(vector *result, const vector *a, const vector *b) { *result = *a - *b; }
void vm_AddVectors(vector *result, const vector *a, const vector *b) { *result = *a + *b; }
void vm_ScaleVector(vector *result, const vector *src, scalar s) { *result = *src * s; }
void vm_ScaleAddVector(vector *d, const vector *p, const vector *v, scalar s) {
  if (d == nullptr || p == nullptr || v == nullptr)
    return;
  *d = *p + *v * s;
}
void vm_DivVector(vector *result, const vector *src, scalar s) {
  if (result == nullptr || src == nullptr || s == 0)
    return;
  *result = *src * (1.0f / s);
}
void vm_CenterVector(vector *v) {
  if (v)
    *v = vector{};
}
void vm_AverageVector(vector *v, int n) {
  if (v == nullptr || n == 0)
    return;
  for (int i = 0; i < 3; i++)
    (*v)[i] /= (scalar)n;
}
scalar vm_GetNormal(vector *n, const vector *v0, const vector *v1, const vector *v2) {
  if (n == nullptr || v0 == nullptr || v1 == nullptr || v2 == nullptr)
    return 0;
  vector a = *v1 - *v0;
  vector b = *v2 - *v0;
  *n = vm_Cross3Product(a, b);
  return vm_NormalizeVector(n);
}
void vm_GetPerp(vector *n, const vector *a, const vector *b, const vector *c) { *n = {}; }
scalar vm_GetCentroid(vector *centroid, const vector *src, int nv) {
  if (centroid == nullptr || src == nullptr || nv <= 0)
    return 0;
  *centroid = vector{};
  for (int i = 0; i < nv; i++)
    *centroid += src[i];
  *centroid = *centroid * (1.0f / (float)nv);
  return 0;
}
scalar vm_GetCentroidFast(vector *centroid, const vector *src, int nv) { return vm_GetCentroid(centroid, src, nv); }
scalar vm_ComputeBoundingSphere(vector *center, const vector *vecs, int num_verts) {
  if (center)
    *center = vector{};
  if (vecs == nullptr || num_verts <= 0)
    return 0;
  vm_GetCentroid(center, vecs, num_verts);
  scalar max_r = 0;
  for (int i = 0; i < num_verts; i++) {
    scalar d = vm_VectorDistance(center, &vecs[i]);
    if (d > max_r)
      max_r = d;
  }
  return max_r;
}
void vm_MakeRandomVector(vector *vec) {
  if (vec)
    *vec = vector{};
}
scalar vm_GetNormalizedDir(vector *dest, const vector *end, const vector *start) {
  if (dest == nullptr || end == nullptr || start == nullptr)
    return 0;
  *dest = *end - *start;
  return vm_NormalizeVector(dest);
}
scalar vm_GetNormalizedDirFast(vector *dest, const vector *end, const vector *start) {
  return vm_GetNormalizedDir(dest, end, start);
}
scalar vm_DistToPlane(const vector *checkp, const vector *norm, const vector *planep) {
  if (checkp == nullptr || norm == nullptr || planep == nullptr)
    return 0;
  return vm_Dot3Product(*checkp - *planep, *norm);
}
scalar vm_GetSlope(scalar x1, scalar y1, scalar x2, scalar y2) {
  if (y2 - y1 == 0)
    return 0;
  return (x2 - x1) / (y2 - y1);
}
scalar calc_det_value(const matrix *det) {
  if (det == nullptr)
    return 0;
  const scalar *m = det->a1d;
  return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) +
         m[2] * (m[3] * m[7] - m[4] * m[6]);
}
void vm_MakeInverseMatrix(matrix *dest) {
  if (dest)
    vm_TransposeMatrix(dest);
}
// angle is uint16_t with 65536 == 2*pi (matches the D3 256-entry trig table).
scalar FixSin(angle a) { return (scalar)(std::sin((2.0 * M_PI * (double)a) / 65536.0)); }
scalar FixCos(angle a) { return (scalar)(std::cos((2.0 * M_PI * (double)a) / 65536.0)); }

void vm_SinCos(angle a, scalar *s, scalar *c) {
  if (s)
    *s = FixSin(a);
  if (c)
    *c = FixCos(a);
}
void vm_SinCosToMatrix(matrix *m, scalar sinp, scalar cosp, scalar sinb, scalar cosb, scalar sinh, scalar cosh) {
  m->rvec.x() = (cosb * cosh) + (sinp * sinb * sinh);
  m->uvec.z() = (sinb * sinh) + (sinp * cosb * cosh);
  m->uvec.x() = (sinp * cosb * sinh) - (sinb * cosh);
  m->rvec.z() = (sinp * sinb * cosh) - (cosb * sinh);
  m->fvec.x() = (sinh * cosp);
  m->rvec.y() = (sinb * cosp);
  m->uvec.y() = (cosb * cosp);
  m->fvec.z() = (cosh * cosp);
  m->fvec.y() = -sinp;
}
angle vm_DeltaAngVec(const vector *v0, const vector *v1, const vector *fvec) {
  vector t;
  if (v0 == nullptr || v1 == nullptr || fvec == nullptr)
    return 0;
  t = *v1 - *v0;
  const scalar m = vm_GetMagnitude(&t);
  if (m > 0.000001f)
    t = t * (1.0f / m);
  return vm_DeltaAngVecNorm(&t, nullptr, fvec);
}
angle vm_DeltaAngVecNorm(const vector *v0, const vector *v1, const vector *fvec) {
  if (v0 == nullptr || fvec == nullptr)
    return 0;
  scalar s = vm_Dot3Product(*v0, *fvec);
  if (s > 1.0f)
    s = 1.0f;
  if (s < -1.0f)
    s = -1.0f;
  return (angle)((std::acos(s) / (2.0 * M_PI)) * 65536.0);
}
void vm_AnglesToMatrix(matrix *m, angle p, angle h, angle b) {
  if (m == nullptr)
    return;
  vm_SinCosToMatrix(m, FixSin(p), FixCos(p), FixSin(b), FixCos(b), FixSin(h), FixCos(h));
}
angvec *vm_ExtractAnglesFromMatrix(angvec *a, const matrix *m) {
  if (a == nullptr || m == nullptr)
    return a;
  scalar p = -m->fvec.y();
  if (p > 1.0f)
    p = 1.0f;
  if (p < -1.0f)
    p = -1.0f;
  a->p() = (angle)((std::asin(p) / (2.0 * M_PI)) * 65536.0);
  scalar cosp = std::sqrt(std::max(0.0f, 1.0f - p * p));
  if (cosp != 0.0f) {
    a->h() = (angle)((std::atan2(m->fvec.x() / cosp, m->fvec.z() / cosp) / (2.0 * M_PI)) * 65536.0);
    a->b() = (angle)((std::atan2(m->rvec.y() / cosp, m->uvec.y() / cosp) / (2.0 * M_PI)) * 65536.0);
  } else {
    a->h() = a->b() = 0;
  }
  return a;
}
void vm_VectorToMatrix(matrix *m, vector *fvec, vector *uvec, vector *rvec) {
  if (m == nullptr || fvec == nullptr)
    return;
  matrix tmp;
  tmp.fvec = *fvec;
  if (vm_NormalizeVector(&tmp.fvec) == 0)
    return;
  if (uvec != nullptr) {
    tmp.uvec = *uvec;
    if (vm_NormalizeVector(&tmp.uvec) == 0)
      tmp.uvec = vector{0, 1, 0};
    tmp.rvec = vm_Cross3Product(tmp.uvec, tmp.fvec);
    if (vm_NormalizeVector(&tmp.rvec) == 0)
      tmp.rvec = vector{1, 0, 0};
    tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
  } else if (rvec != nullptr) {
    tmp.rvec = *rvec;
    if (vm_NormalizeVector(&tmp.rvec) == 0)
      tmp.rvec = vector{1, 0, 0};
    tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
    if (vm_NormalizeVector(&tmp.uvec) == 0)
      tmp.uvec = vector{0, 1, 0};
    tmp.rvec = vm_Cross3Product(tmp.uvec, tmp.fvec);
  } else {
    if (tmp.fvec.x() == 0 && tmp.fvec.z() == 0) {
      tmp.rvec = vector{1, 0, 0};
      tmp.uvec = vector{0, (tmp.fvec.y() < 0) ? 1.0f : -1.0f, 0};
    } else {
      tmp.rvec = vector{tmp.fvec.z(), 0, -tmp.fvec.x()};
      vm_NormalizeVector(&tmp.rvec);
      tmp.uvec = vm_Cross3Product(tmp.fvec, tmp.rvec);
    }
  }
  *m = tmp;
}
void vm_VectorAngleToMatrix(matrix *m, vector *v, angle a) {
  if (m == nullptr || v == nullptr)
    return;
  scalar sinp = -v->y();
  scalar cosp = std::sqrt(std::max(0.0f, 1.0f - sinp * sinp));
  scalar sinh = 0, cosh = 1.0f;
  if (cosp != 0.0f) {
    sinh = v->x() / cosp;
    cosh = v->z() / cosp;
  }
  vm_SinCosToMatrix(m, sinp, cosp, FixSin(a), FixCos(a), sinh, cosh);
}
void vm_MatrixMulTMatrix(matrix *dest, const matrix *src0, const matrix *src1) {
  if (dest == nullptr || src0 == nullptr || src1 == nullptr)
    return;
  matrix d;
  d.rvec.x() = src0->rvec.x() * src1->rvec.x() + src0->uvec.x() * src1->uvec.x() + src0->fvec.x() * src1->fvec.x();
  d.uvec.x() = src0->rvec.x() * src1->rvec.y() + src0->uvec.x() * src1->uvec.y() + src0->fvec.x() * src1->fvec.y();
  d.fvec.x() = src0->rvec.x() * src1->rvec.z() + src0->uvec.x() * src1->uvec.z() + src0->fvec.x() * src1->fvec.z();
  d.rvec.y() = src0->rvec.y() * src1->rvec.x() + src0->uvec.y() * src1->uvec.x() + src0->fvec.y() * src1->fvec.x();
  d.uvec.y() = src0->rvec.y() * src1->rvec.y() + src0->uvec.y() * src1->uvec.y() + src0->fvec.y() * src1->fvec.y();
  d.fvec.y() = src0->rvec.y() * src1->rvec.z() + src0->uvec.y() * src1->uvec.z() + src0->fvec.y() * src1->fvec.z();
  d.rvec.z() = src0->rvec.z() * src1->rvec.x() + src0->uvec.z() * src1->uvec.x() + src0->fvec.z() * src1->fvec.x();
  d.uvec.z() = src0->rvec.z() * src1->rvec.y() + src0->uvec.z() * src1->uvec.y() + src0->fvec.z() * src1->fvec.y();
  d.fvec.z() = src0->rvec.z() * src1->rvec.z() + src0->uvec.z() * src1->uvec.z() + src0->fvec.z() * src1->fvec.z();
  *dest = d;
}
void vm_MatrixMulVector(vector *result, const vector *v, const matrix *m) {
  if (result == nullptr || v == nullptr || m == nullptr)
    return;
  result->x() = vm_Dot3Product(*v, m->rvec);
  result->y() = vm_Dot3Product(*v, m->uvec);
  result->z() = vm_Dot3Product(*v, m->fvec);
}
void vm_VectorMulTMatrix(vector *result, const vector *v, const matrix *m) {
  if (result == nullptr || v == nullptr || m == nullptr)
    return;
  result->x() = vm_Dot3Vector(v->x(), v->y(), v->z(), &m->rvec);
  result->y() = vm_Dot3Vector(v->x(), v->y(), v->z(), &m->uvec);
  result->z() = vm_Dot3Vector(v->x(), v->y(), v->z(), &m->fvec);
}
void vm_MatrixMul(matrix *dest, const matrix *a, const matrix *b) {
  if (dest == nullptr || a == nullptr || b == nullptr)
    return;
  *dest = *a * *b;
}
matrix operator*(const matrix &a, const matrix &b) {
  matrix d;
  d.rvec = vector{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), &b.rvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), &b.rvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), &b.rvec)};
  d.uvec = vector{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), &b.uvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), &b.uvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), &b.uvec)};
  d.fvec = vector{vm_Dot3Vector(a.rvec.x(), a.uvec.x(), a.fvec.x(), &b.fvec),
                  vm_Dot3Vector(a.rvec.y(), a.uvec.y(), a.fvec.y(), &b.fvec),
                  vm_Dot3Vector(a.rvec.z(), a.uvec.z(), a.fvec.z(), &b.fvec)};
  return d;
}
matrix operator*=(matrix &a, const matrix &b) { return (a = a * b); }

// ==================== 3D functions (exact signatures from 3d.h) ====================
uint8_t g3_RotatePoint(g3Point *dest, vector *src) { return 0; }
void g3_ProjectPoint(g3Point *point) {}
bool g3_CheckNormalFacing(vector *v, vector *norm) { return false; }
g3Point **g3_ClipPolygon(g3Point **pointlist, int *nv, g3Codes *cc) { return pointlist; }
void g3_FreeTempPoints(g3Point **pointlist, int nv) {}

// ==================== grSurface / grViewport / grHardwareSurface ====================
grSurface::grSurface() {}
grSurface::~grSurface() {}
void grSurface::free() {}
grViewport::grViewport(grSurface *s) {}
grViewport::~grViewport() {}
// grHardwareSurface::create returns bool, takes (int, int, int, unsigned, const char*)
bool grHardwareSurface::create(int w, int h, int bpp, unsigned flags, const char *name) { return false; }
float Float_to_ubyte(float f) { return f; }

// ==================== Object ====================
void ObjSetPos(object *obj, vector *pos, int roomnum, matrix *orient, bool f_relink) {}
void ObjDelete(int objnum) {}
int ObjCreate(uint8_t type, uint16_t id, int roomnum, vector *pos, const matrix *orient, int parent_handle) { return -1; }
void ObjSetOrient(object *obj, const matrix *orient) {}
object *ObjGet(int objnum) { return nullptr; }
void ObjReInitAll() {}
void FreeAllObjects() {}
void ResetObjectList() {}
void GetObjectPointInWorld(vector *dest, object *obj, int gun_num, int animated_frame) { *dest = {}; }

object Objects[MAX_OBJECTS];
int Highest_object_index = -1;
object *Viewer_object = &Objects[0];
object *Player_object = &Objects[0];
object_info Object_info[MAX_OBJECTS];

// ==================== Terrain ====================
int Terrain_checksum = 0;
uint8_t Terrain_dynamic_table[16] = {};
terrain_segment Terrain_seg[(TERRAIN_WIDTH+1) * (TERRAIN_DEPTH+1)] = {};
terrain_tex_segment Terrain_tex_seg[TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH] = {};
terrain_sky Terrain_sky = {};
terrain_normals *TerrainNormals[MAX_TERRAIN_LOD] = {};
terrain_sound_band Terrain_sound_bands[256] = {};
float Terrain_texture_distance = 0;
int16_t Terrain_seg_render_objs[(TERRAIN_WIDTH+1) * (TERRAIN_DEPTH+1)] = {};
uint8_t Fast_terrain = 0;
uint8_t Flat_terrain = 0;
uint8_t Show_invisible_terrain = 0;

int GetTerrainRoomFromPos(vector *pos) { return -1; }
void ComputeTerrainSegmentCenter(vector *center, int seg) { *center = {}; }
float GetTerrainGroundPoint(vector *in, vector *out) { *out = *in; return 0; }
void BuildMinMaxTerrain() {}
void BuildTerrainNormals() {}
void ResetTerrain(int terrain_size) {}
void SetupSky(float t, int tmap, unsigned char layer) {}
void UpdateTerrainLightmaps() {}
int TERRAIN_REGION(int x) { return 0; }
void ClearTerrainSound() {}

// ==================== BNode ====================
bool BNode_allocated = false;
bool BNode_verified = false;
bn_list *BNode_GetBNListPtr(int roomnum, bool f_in_load_level) { return nullptr; }
bool BNode_FindPath(int start_room, int i, int j, float rad) { return false; }
void BNode_ClearBNodeInfo() {}

// ==================== BOA ====================
int BOA_num_terrain_regions = 0;
connect_data BOA_connect[MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS] = {};
int BOA_num_connect[MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_AABB_checksum = 0;
int BOA_AABB_ROOM_checksum[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_mine_checksum = 0;
int BOA_vis_checksum = 0;
bool BOA_vis_valid = false;
void MakeBOA() {}
bool BOA_IsVisible(int start_room, int end_room) { return false; }

// ==================== BSP ====================
void InitBSP() {}
int BSPRayOccluded(vector *a, vector *b, bspnode *n) { return 0; }
bsptree MineBSP = {};
uint8_t UseBSP = 0;
void BuildBSPTree() {}
void BuildSingleBSPTree(int n) {}

// ==================== Game globals ====================
int Num_textures = 0;
int Num_sounds = 0;
int Num_weapons = 0;
int Num_doors = 0;
int Num_ships = 0;
int Num_megacells = 0;
int Num_triggers = 0;
int Num_game_paths = 0;
int Num_matcens = 0;
int Num_object_ids[1500] = {};
int Network_up = 0;
float Gravity_strength = 9.8f;
int Cinematics_enabled = 0;
renderer_type PreferredRenderer = {};
int Detail_settings = 0;
float force_field_bounce_multiplier[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {};
int force_field_bounce_texture[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {};
float Ceiling_height = 100.0f;
float Frametime = 0;
renderer_preferred_state Render_preferred_state = {};
int sound_override_force_field = -1;
int sound_override_glass_breaking = -1;
bool Level_powerups_ignore_wind = false;

char InfoString[INFO_STRING_LEN] = {};
std::string Default_pilot;
std::filesystem::path LocalMiscDir;
std::filesystem::path LocalModelsDir;
std::filesystem::path LocalSoundsDir;
std::filesystem::path NetMiscDir;
std::filesystem::path NetModelsDir;
char LocalLevelsDir[] = "";
char LocalScriptDir[] = "";
char TableUser[] = "";
char ErrorString[INFO_STRING_LEN] = "";

// ==================== Game data arrays ====================
bms_bitmap GameBitmaps[MAX_BITMAPS];
bms_lightmap GameLightmaps[MAX_LIGHTMAPS];
texture GameTextures[MAX_TEXTURES];
vclip GameVClips[MAX_VCLIPS];
poly_model Poly_models[500];
game_path GamePaths[MAX_GAME_PATHS];
trigger Triggers[500];
special_face SpecialFaces[MAX_SPECIAL_FACES];

// ==================== Sound ====================
hlsSystem Sound_system;
void hlsSystem::BeginSoundFrame(bool f_force) {}
void hlsSystem::EndSoundFrame() {}
void hlsSystem::StopAllSounds() {}
sound_info Sounds[MAX_SOUNDS] = {};
sound_file_info SoundFiles[MAX_SOUND_FILES] = {};

// ==================== Lighting ====================
lightmap_info LightmapInfoStore[MAX_LIGHTMAP_INFOS] = {};
lightmap_info *LightmapInfo = LightmapInfoStore;
int Num_of_lightmap_info = 0;
int Num_lightmap_infos_read = 0;
int Outline_lightmaps = 0;
void InitLightmapInfo() { memset(LightmapInfoStore, 0, sizeof(LightmapInfoStore)); Num_of_lightmap_info = 0; }
void FreeLightmapMemory() {}
void ClearAllObjectLightmaps(int n) {}
void ClearObjectLightmaps(object *obj) {}
void BlurLightmapInfos(int n) {}
void ShadeLightmapInfoEdges(int n) {}
void EnableLightmapGen() {}
int lm_AllocLightmap(int w, int h) { return -1; }
void lm_FreeLightmap(int n) {}
uint16_t *lm_data(int handle) { return nullptr; }
int lm_w(int n) { return 0; }
int lm_h(int n) { return 0; }
int lmi_h(int n) { return 0; }
int lmi_w(int n) { return 0; }
void SetupObjectLightmapMemory(object *obj) {}
void ClearAllVolumeLights() {}

// ==================== Player/ship ====================
player Players[MAX_PLAYERS] = {};
int Num_player_shapes = 0;
ship Ships[MAX_SHIPS] = {};

// ==================== Door ====================
door Doors[MAX_DOORS];
doorway *DoorwayAdd(room *rp, int doornum) { return nullptr; }

// ==================== Weapon ====================
weapon Weapons[MAX_WEAPONS] = {};
// Sentinel-terminated list (empty) so callers strcmp("", name[i]) cleanly.
const char *const Static_weapon_names[] = {""};
const int Static_weapon_names_msg[] = {0};
void WBClearInfo(otype_wb_info *wb) {}

// ==================== Megacell ====================
megacell Megacells[MAX_MEGACELLS] = {};

// ==================== Matcen ====================
matcen *Matcen[MAX_MATCENS] = {};

// ==================== Level ====================
levelgoals Level_goals;
level_info Level_info;
void levelgoals::CleanupAfterLevel() {}

// ==================== Ambient ====================
ambient_life a_life;
void ambient_life::ALReset() {}

// ==================== Manage ====================
mngs_track_lock GlobalTrackLocks[] = {};
int mng_AllocTrackLock(char *a, int b) { return -1; }
void mng_FreeTrackLock(int n) {}
int mng_FindTrackLock(char *a, int b) { return -1; }
int mng_CheckIfPageLocked(mngs_Pagelock *p) { return 0; }
int mng_CheckIfPageOwned(mngs_Pagelock *p, char *a) { return 0; }
int mng_DeletePage(char *a, int b, int c) { return 0; }
int mng_DeletePagelock(char *a, int b) { return 0; }
void mng_EraseLocker() {}
int mng_MakeLocker() { return 0; }
void mng_OverrideToUnlocked(mngs_Pagelock *p) {}
int mng_RenamePage(char *a, char *b, int c) { return 0; }
int mng_ReplacePage(char *a, char *b, int c, int d, int e) { return 0; }
int mng_ReplacePagelock(char *a, mngs_Pagelock *b) { return 0; }
int mng_FindSpecificDoorPage(char *a, mngs_door_page *b, int c) { return 0; }
int mng_FindSpecificGenericPage(char *a, mngs_generic_page *b, int c) { return 0; }
int mng_FindSpecificShipPage(char *a, mngs_ship_page *b, int c) { return 0; }
int mng_FindSpecificSoundPage(char *a, mngs_sound_page *b, int c) { return 0; }
int mng_FindSpecificTexPage(char *a, mngs_texture_page *b, int c) { return 0; }
int mng_FindSpecificWeaponPage(char *a, mngs_weapon_page *b, int c) { return 0; }
int mng_AssignDoorPageToDoor(mngs_door_page *a, int b) { return 0; }
int mng_AssignGenericPageToObjInfo(mngs_generic_page *a, int b, CFILE *c) { return 0; }
int mng_AssignShipPageToShip(mngs_ship_page *a, int b, CFILE *c) { return 0; }
int mng_AssignSoundPageToSound(mngs_sound_page *a, int b) { return 0; }
int mng_AssignTexPageToTexture(mngs_texture_page *a, int b, CFILE *c) { return 0; }
int mng_AssignWeaponPageToWeapon(mngs_weapon_page *a, int b, CFILE *c) { return 0; }

// ==================== CFILE ====================
bool cf_CopyFile(const std::filesystem::path &dst, const std::filesystem::path &src, int b) { return false; }
std::filesystem::path cf_GetWritableBaseDirectory() { return "/tmp"; }

// ==================== Mem ====================
void *mem_malloc_sub(int size, const char *fn, int line) {
  void *p = malloc(size);
  if (p == nullptr)
    throw std::bad_alloc();
  return p;
}
void mem_free_sub(void *ptr) { free(ptr); }
void *mem_realloc_sub(void *ptr, int size) {
  void *p = realloc(ptr, size);
  if (p == nullptr)
    throw std::bad_alloc();
  return p;
}
char *mem_strdup_sub(const char *s, const char *fn, int line) {
  if (s == nullptr)
    return nullptr;
  const std::size_t n = std::strlen(s) + 1;
  char *d = static_cast<char *>(mem_malloc(n));
  std::memcpy(d, s, n);
  return d;
}

// ==================== Room ====================
void InitRoom(room *rp, int nverts, int nfaces, int nportals) {
  rp->flags = 0;
  rp->objects = -1;
  rp->vis_effects = -1;
  rp->volume_lights = nullptr;
  rp->mirror_face = -1;
  rp->num_mirror_faces = 0;
  rp->mirror_faces_list = nullptr;
  rp->room_change_flags = 0;
  rp->wind = vector{};
  rp->num_faces = nfaces;
  rp->num_verts = nverts;
  rp->num_portals = nportals;
  rp->last_render_time = 0;
  rp->fog_depth = 100.0f;
  rp->fog_r = rp->fog_g = rp->fog_b = 1.0f;
  rp->faces = nfaces ? static_cast<face *>(mem_malloc(sizeof(face) * nfaces)) : nullptr;
  rp->verts = nverts ? static_cast<vector *>(mem_malloc(sizeof(vector) * nverts)) : nullptr;
  rp->verts4 = nullptr;
  rp->portals = nportals ? static_cast<portal *>(mem_malloc(sizeof(portal) * nportals)) : nullptr;
  rp->pulse_time = 0;
  rp->pulse_offset = 0;
  rp->ambient_sound = -1;
  rp->name = nullptr;
  rp->doorway_data = nullptr;
  rp->env_reverb = 0;
  rp->damage = 0.0f;
  rp->damage_type = 0;
  rp->bn_info.num_nodes = 0;
  rp->bn_info.nodes = nullptr;
  rp->used = 1;
}

void FreeRoom(room *rp) {
  if (rp == nullptr)
    return;
  if (rp->faces) {
    for (int i = 0; i < rp->num_faces; i++)
      FreeRoomFace(&rp->faces[i]);
    mem_free(rp->faces);
    rp->faces = nullptr;
  }
  if (rp->verts) {
    mem_free(rp->verts);
    rp->verts = nullptr;
  }
  if (rp->verts4) {
    mem_free(rp->verts4);
    rp->verts4 = nullptr;
  }
  if (rp->portals) {
    mem_free(rp->portals);
    rp->portals = nullptr;
  }
  if (rp->mirror_faces_list) {
    mem_free(rp->mirror_faces_list);
    rp->mirror_faces_list = nullptr;
  }
  if (rp->bn_info.nodes) {
    mem_free(rp->bn_info.nodes);
    rp->bn_info.nodes = nullptr;
  }
  rp->used = 0;
}

void InitRoomFace(face *fp, int nverts) {
  fp->flags = 0;
  fp->num_verts = nverts;
  fp->portal_num = -1;
  fp->tmap = 0;
  fp->lmi_handle = 0;
  fp->special_handle = 0;
  fp->light_multiple = 4;
  fp->normal = vector{};
  fp->min_xyz = vector{};
  fp->max_xyz = vector{};
  fp->renderframe = 0;
  fp->face_verts = nverts ? static_cast<int16_t *>(mem_malloc(sizeof(int16_t) * nverts)) : nullptr;
  fp->face_uvls = nverts ? static_cast<roomUVL *>(mem_malloc(sizeof(roomUVL) * nverts)) : nullptr;
  for (int i = 0; i < nverts; i++)
    fp->face_uvls[i].alpha = 255;
}

void FreeRoomFace(face *fp) {
  if (fp == nullptr)
    return;
  if (fp->face_verts) {
    mem_free(fp->face_verts);
    fp->face_verts = nullptr;
  }
  if (fp->face_uvls) {
    mem_free(fp->face_uvls);
    fp->face_uvls = nullptr;
  }
  fp->num_verts = 0;
}

// Approximate face normal from the first non-degenerate triangle winding.
bool ComputeFaceNormal(room *rp, int facenum) {
  if (facenum < 0 || facenum >= rp->num_faces)
    return false;
  face *fp = &rp->faces[facenum];
  if (fp->num_verts < 3)
    return false;
  const vector *v0 = &rp->verts[fp->face_verts[0]];
  const vector *v1 = &rp->verts[fp->face_verts[1]];
  const vector *v2 = &rp->verts[fp->face_verts[2]];
  vector a = *v1 - *v0;
  vector b = *v2 - *v0;
  fp->normal = vector{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
  const scalar mag = fp->normal.mag();
  if (mag < 0.0001f)
    return false;
  fp->normal = fp->normal * (1.0f / mag);
  return true;
}

bool ComputeNormal(vector *normal, int num_verts, short *vertnum_list, vector *verts) {
  if (num_verts < 3 || verts == nullptr || vertnum_list == nullptr)
    return false;
  const vector *v0 = &verts[vertnum_list[0]];
  const vector *v1 = &verts[vertnum_list[1]];
  const vector *v2 = &verts[vertnum_list[2]];
  vector a = *v1 - *v0;
  vector b = *v2 - *v0;
  *normal = vector{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
  return true;
}

void ComputePortalCenter(vector *center, room *rp, int portal) { *center = {}; }
void ComputeCenterPointOnFace(vector *center, room *rp, int facenum) { *center = {}; }
void GetIJ(const vector *normal, int *ii, int *jj) {
  if (normal == nullptr || ii == nullptr || jj == nullptr) {
    if (ii)
      *ii = 0;
    if (jj)
      *jj = 1;
    return;
  }
  // Project onto 2d: find the dominant axis of the surface normal.
  if (fabs(normal->x()) > fabs(normal->y()))
    if (fabs(normal->x()) > fabs(normal->z())) {
      if (normal->x() > 0) {
        *ii = 2;
        *jj = 1;
      } else {
        *ii = 1;
        *jj = 2;
      }
    } else {
      if (normal->z() > 0) {
        *ii = 1;
        *jj = 0;
      } else {
        *ii = 0;
        *jj = 1;
      }
    }
  else if (fabs(normal->y()) > fabs(normal->z())) {
    if (normal->y() > 0) {
      *ii = 0;
      *jj = 2;
    } else {
      *ii = 2;
      *jj = 0;
    }
  } else {
    if (normal->z() > 0) {
      *ii = 1;
      *jj = 0;
    } else {
      *ii = 0;
      *jj = 1;
    }
  }
}
void GetVolumeSizeOfRoom(room *rp, int *w, int *h, int *d) { if (w) *w = 0; if (h) *h = 0; if (d) *d = 0; }
void FreeAllRooms() {
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (Rooms[i].used)
      FreeRoom(&Rooms[i]);
  }
}
void ClearAllRoomLightmaps(int n) {}
void ClearRoomLightmaps(int n) {}
void ClearAllRoomSpecmaps(int n) {}

room Rooms[MAX_ROOMS] = {};
int Highest_room_index = -1;

// ==================== DDIO ====================
void ddio_SplitPath(const char *path, char *dir, char *fname, char *ext) { if (dir) dir[0] = 0; if (fname) fname[0] = 0; if (ext) ext[0] = 0; }
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *l, int *r) { if (x) *x = 0; if (y) *y = 0; if (dx) *dx = 0; if (dy) *dy = 0; if (l) *l = 0; if (r) *r = 0; return 0; }
float ddio_KeyDownTime(int key) { return 0; }
void joy_GetPos(tJoystick joy, tJoyPos *pos) { if (pos) { pos->x = 0; pos->y = 0; pos->z = 0; } }
bool joy_IsValid(tJoystick id) { return false; }

// ==================== ChronoTimer ====================
float D3::ChronoTimer::GetTime() { return 0; }
void D3::ChronoTimer::Initialize() {}

// ==================== Init ====================
void GatherArgs(char **argv) {}
void PreInitD3Systems() {}
void InitD3Systems1(bool b) {}
void InitD3Systems2(bool b) {}
void InitGamePaths() {}
void ProgramVersion(int a, unsigned char b, unsigned char c, unsigned char d) {}

// ==================== Application ====================
static tLnxAppInfo _default_app_info;
oeLnxApplication::oeLnxApplication(tLnxAppInfo *info) {}
oeLnxApplication::~oeLnxApplication() {}
void oeLnxApplication::init() {}
void oeLnxApplication::get_info(void *appinfo) {}
unsigned oeLnxApplication::defer() { return 0; }
const char *oeLnxApplication::get_window_name() { return "D3Editor"; }
void oeLnxApplication::clear_window() {}
void oeLnxApplication::set_defer_handler(void (*func)(bool isactive)) {}
void oeLnxApplication::delay(float secs) {}
int oeLnxApplication::flags() const { return 0; }

oeLnxAppDatabase::oeLnxAppDatabase() {}
oeLnxAppDatabase::~oeLnxAppDatabase() {}
bool oeLnxAppDatabase::create_record(const char *pathname) { return false; }
bool oeLnxAppDatabase::lookup_record(const char *pathname) { return false; }
bool oeLnxAppDatabase::read(const char *label, char *entry, int *entrylen) { return false; }
bool oeLnxAppDatabase::read(const char *label, void *entry, int wordsize) { return false; }
bool oeLnxAppDatabase::read(const char *label, bool *entry) { return false; }
bool oeLnxAppDatabase::write(const char *label, const char *entry, int entrylen) { return false; }
bool oeLnxAppDatabase::write(const char *label, int entry) { return false; }
void oeLnxAppDatabase::get_user_name(char *buffer, size_t *size) { if (buffer) buffer[0] = 0; }

oeApplication *Descent = nullptr;
oeAppDatabase *Database = nullptr;

// ==================== Find* ====================
int FindDoorName(const char *name) { return -1; }
int FindGamePathName(const char *name) { return -1; }
int FindObjectIDName(const char *name) { return -1; }
int FindShipName(const char *name) { return -1; }
int FindSoundName(const char *name) { return -1; }
int FindTextureName(const char *name) { return -1; }
int FindWeaponName(const char *name) { return -1; }

// ==================== Alloc/Free ====================
int AllocDoor() { return -1; }
void FreeDoor(int n) {}
int AllocObjectID(int id, bool a, bool b, bool c) { return -1; }
void FreeObjectID(int n) {}
int GetObjectID(int n) { return -1; }
int AllocShip() { return -1; }
void FreeShip(int n) {}
int AllocSound() { return -1; }
void FreeSound(int n) {}
int AllocSpecialFace(int a, int b, bool c, int d) { return -1; }
void FreeSpecialFace(int n) {}
int AllocTexture() { return -1; }
void FreeTexture(int n) {}
int AllocWeapon() { return -1; }
void FreeWeapon(int n) {}
int AllocLightmapInfo(int a, int b, int c, bool d) { return -1; }

// ==================== GetNext/Prev ====================
int GetNextDoor(int n) { return -1; }
int GetPrevDoor(int n) { return -1; }
int GetNextMegacell(int n) { return -1; }
int GetPrevMegacell(int n) { return -1; }
int GetNextShip(int n) { return -1; }
int GetPrevShip(int n) { return -1; }
int GetNextSound(int n) { return -1; }
int GetPrevSound(int n) { return -1; }
int GetNextTexture(int n) { return -1; }
int GetPreviousTexture(int n) { return -1; }
int GetNextWeapon(int n) { return -1; }
int GetPrevWeapon(int n) { return -1; }

// ==================== Remap ====================
void RemapShips() {}
void RemapSounds() {}
void RemapStaticIDs() {}
void RemapWeapons() {}

// ==================== Load ====================
int LoadDoorImage(const char *name, int n) { return -1; }
void LoadLevel(char *name, void (*cb)(const char *, int, int)) {}
int LoadPolyModel(const std::filesystem::path &name, int f_module) { return -1; }
int LoadShipImage(const char *name) { return -1; }
int LoadSoundFile(const char *name, float vol, bool b) { return -1; }
int LoadTextureImage(const char *name, int *handle, int a, int b, int c, int d) { return -1; }
int GetDoorImage(int n) { return -1; }

// ==================== Polymodel ====================
poly_model *GetPolymodelPointer(int n) { return nullptr; }
int CountFacesInPolymodel(poly_model *pm) { return 0; }
float ComputeDefaultSize(int type, int handle, float *size) { return 0; }
void PageInPolymodel(int model_num, int f_damage, float *size) {}
void SetModelAnglesAndPos(poly_model *pm, float *anim, unsigned int flags) {}
int IsNonRenderableSubmodel(poly_model *pm, int index) { return 0; }
void ChangeOldModelsForObjects(int a, int b) {}
std::filesystem::path ChangePolyModelName(const std::filesystem::path &name) { return name; }
void GenerateLODDeltas() {}
int FindPolyModelName(const std::filesystem::path &name) { return -1; }
void FreePolyModel(int n) {}

// ==================== Misc ====================
int CreateMatcen(const char *name, bool *flag) { return -1; }
void DestroyAllMatcens() {}
void FreeAllGamePaths() {}
void FreeGamePath(int n) {}
int FindPointRoom(vector *pnt) { return -1; }
int GetTerrainRoomFromPos_ret(vector *pos) { return -1; }
int AIMakeNextRoomList(int roomnum, int *next_rooms, int max_rooms) { return 0; }
bool PhysCalcGround(vector *ground_point, vector *ground_normal, object *obj, int ground_num) { return false; }
void ClearAllEvents() {}

// ==================== FVI ====================
int fvi_FindIntersection(fvi_query *q, fvi_info *info, bool b) { memset(info, 0, sizeof(fvi_info)); return 0; }
int fvi_QuickDistFaceList(int init_room_index, vector *pos, float rad, fvi_face_room_list *quick_fr_list, int max_elements) { return 0; }
bool FVI_always_check_ceiling = false;

// ==================== OSIRIS ====================
int osipf_FindObjectName(const char *name) { return -1; }
int osipf_FindRoomName(const char *name) { return -1; }
int osipf_FindTriggerName(const char *name) { return -1; }

// ==================== Pilot ====================
void PltClearList() {}
std::vector<std::string> PltGetPilots(std::string ignore, int display) { return {}; }

// ==================== OutrageMessageBox ====================
void OutrageMessageBox(const char *fmt, ...) {}

// ==================== Bitmap ====================
uint16_t *bm_data(int handle, int miplevel) { return nullptr; }
int bm_w(int handle, int miplevel) { return 0; }
int bm_h(int handle, int miplevel) { return 0; }
int bm_mipped(int handle) { return 0; }

// ==================== Inventory ====================
Inventory::Inventory() {}
Inventory::~Inventory() {}

// ==================== grFont ====================
grFont::grFont() {}
grFont::~grFont() {}

// ==================== hlsSystem ====================
hlsSystem::hlsSystem() {}
void hlsSystem::KillSoundLib(bool f_kill_sound_list) {}
int hlsSystem::Play2dSound(int sound_index, float volume, float pan, unsigned short flags) { return 0; }
bool hlsSystem::CheckAndForceSoundDataAlloc(int sound_file_index) { return false; }

// ==================== matcen methods ====================
void matcen::GetName(char *buf) { if (buf) buf[0] = 0; }
bool matcen::SetName(const char *name) { return false; }
