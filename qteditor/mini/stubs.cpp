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
#include "grtext.h"
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
#include "args.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <filesystem>
#include <cstdarg>
#include <cmath>

#include <QDebug>
#include <QtGlobal>
#include <cstdio>
#include <cstdlib>

//#include "default_base_directories.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PRINT_STUB(x) qDebug() << "Function \"" << __FUNCTION__ << "\" is a stub!"

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
uint8_t g3_RotatePoint(g3Point *dest, vector *src) { PRINT_STUB(__FUNCTION__); return 0; }
void g3_ProjectPoint(g3Point *point) { PRINT_STUB(__FUNCTION__); }
bool g3_CheckNormalFacing(vector *v, vector *norm) { PRINT_STUB(__FUNCTION__); return false; }
g3Point **g3_ClipPolygon(g3Point **pointlist, int *nv, g3Codes *cc) { PRINT_STUB(__FUNCTION__); return pointlist; }
void g3_FreeTempPoints(g3Point **pointlist, int nv) { PRINT_STUB(__FUNCTION__); }

// ==================== grSurface / grViewport / grHardwareSurface ====================
grSurface::grSurface() { PRINT_STUB(__FUNCTION__); }
grSurface::~grSurface() { PRINT_STUB(__FUNCTION__); }
void grSurface::free() { PRINT_STUB(__FUNCTION__); }
grViewport::grViewport(grSurface *s) { PRINT_STUB(__FUNCTION__); }
grViewport::~grViewport() { PRINT_STUB(__FUNCTION__); }
// grHardwareSurface::create returns bool, takes (int, int, int, unsigned, const char*)
bool grHardwareSurface::create(int w, int h, int bpp, unsigned flags, const std::string& name) { PRINT_STUB(__FUNCTION__); return false; }
float Float_to_ubyte(float f) { PRINT_STUB(__FUNCTION__); return f; }

// ==================== Object ====================
void ObjSetPos(object *obj, vector *pos, int roomnum, matrix *orient, bool f_relink) { PRINT_STUB(__FUNCTION__); }
void ObjDelete(int objnum) { PRINT_STUB(__FUNCTION__); }
int ObjCreate(uint8_t type, uint16_t id, int roomnum, vector *pos, const matrix *orient, int parent_handle) { PRINT_STUB(__FUNCTION__); return -1; }
void ObjSetOrient(object *obj, const matrix *orient) { PRINT_STUB(__FUNCTION__); }
object *ObjGet(int objnum) { PRINT_STUB(__FUNCTION__); return nullptr; }
void ObjReInitAll() { PRINT_STUB(__FUNCTION__); }
void FreeAllObjects() { PRINT_STUB(__FUNCTION__); }
void ResetObjectList() { PRINT_STUB(__FUNCTION__); }
void GetObjectPointInWorld(vector *dest, object *obj, int gun_num, int animated_frame) { PRINT_STUB(__FUNCTION__); *dest = {}; }

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

int GetTerrainRoomFromPos(vector *pos) { PRINT_STUB(__FUNCTION__); return -1; }
void ComputeTerrainSegmentCenter(vector *center, int seg) { PRINT_STUB(__FUNCTION__); *center = {}; }
float GetTerrainGroundPoint(vector *in, vector *out) { PRINT_STUB(__FUNCTION__); *out = *in; return 0; }
void BuildMinMaxTerrain() { PRINT_STUB(__FUNCTION__); }
void BuildTerrainNormals() { PRINT_STUB(__FUNCTION__); }
void ResetTerrain(int terrain_size) { PRINT_STUB(__FUNCTION__); }
void SetupSky(float t, int tmap, unsigned char layer) { PRINT_STUB(__FUNCTION__); }
void UpdateTerrainLightmaps() { PRINT_STUB(__FUNCTION__); }
int TERRAIN_REGION(int x) { PRINT_STUB(__FUNCTION__); return 0; }
void ClearTerrainSound() { PRINT_STUB(__FUNCTION__); }

// ==================== BNode ====================
bool BNode_allocated = false;
bool BNode_verified = false;
bn_list *BNode_GetBNListPtr(int roomnum, bool f_in_load_level) { PRINT_STUB(__FUNCTION__); return nullptr; }
bool BNode_FindPath(int start_room, int i, int j, float rad) { PRINT_STUB(__FUNCTION__);return false; }
void BNode_ClearBNodeInfo() { PRINT_STUB(__FUNCTION__); }

// ==================== BOA ====================
int BOA_num_terrain_regions = 0;
connect_data BOA_connect[MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS] = {};
int BOA_num_connect[MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_AABB_checksum = 0;
int BOA_AABB_ROOM_checksum[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_mine_checksum = 0;
int BOA_vis_checksum = 0;
bool BOA_vis_valid = false;
void MakeBOA() { PRINT_STUB(__FUNCTION__); }
bool BOA_IsVisible(int start_room, int end_room) { PRINT_STUB(__FUNCTION__); return false; }

// ==================== BSP ====================
void InitBSP() { PRINT_STUB(__FUNCTION__); }
int BSPRayOccluded(vector *a, vector *b, bspnode *n) { PRINT_STUB(__FUNCTION__); return 0; }
bsptree MineBSP = {};
uint8_t UseBSP = 0;
void BuildBSPTree() { PRINT_STUB(__FUNCTION__); }
void BuildSingleBSPTree(int n) { PRINT_STUB(__FUNCTION__); }

// ==================== Game globals ====================
int Num_textures = 0;
int Num_sounds = 0;
int Num_weapons = 0;
int Num_doors = 0;
int Num_ships = 0;
int Num_megacells = 0;
int Num_objects = 0;
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

QString InfoString;
QString ErrorString;
QString TableUser;
std::string Default_pilot;
std::filesystem::path LocalMiscDir;
std::filesystem::path LocalModelsDir;
std::filesystem::path LocalSoundsDir;
std::filesystem::path NetMiscDir;
std::filesystem::path NetModelsDir;
std::filesystem::path LocalLevelsDir;
std::filesystem::path LocalScriptDir;

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
void hlsSystem::BeginSoundFrame(bool f_force) { PRINT_STUB(__FUNCTION__); }
void hlsSystem::EndSoundFrame() { PRINT_STUB(__FUNCTION__); }
void hlsSystem::StopAllSounds() { PRINT_STUB(__FUNCTION__); }
sound_info Sounds[MAX_SOUNDS] = {};
sound_file_info SoundFiles[MAX_SOUND_FILES] = {};

// ==================== Lighting ====================
lightmap_info LightmapInfoStore[MAX_LIGHTMAP_INFOS] = {};
lightmap_info *LightmapInfo = LightmapInfoStore;
int Num_of_lightmap_info = 0;
int Num_lightmap_infos_read = 0;
int Outline_lightmaps = 0;
void InitLightmapInfo(int nummaps) {
  if (nummaps > 0 && nummaps < MAX_LIGHTMAP_INFOS) {
    memset(LightmapInfoStore, 0, sizeof(lightmap_info) * nummaps);
  } else {
    memset(LightmapInfoStore, 0, sizeof(LightmapInfoStore));
  }
  Num_of_lightmap_info = 0;
}
void FreeLightmapMemory() { PRINT_STUB(__FUNCTION__); }
void ClearAllObjectLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
void ClearObjectLightmaps(object *obj) { PRINT_STUB(__FUNCTION__); }
void BlurLightmapInfos(int n) { PRINT_STUB(__FUNCTION__); }
void ShadeLightmapInfoEdges(int n) { PRINT_STUB(__FUNCTION__); }
void EnableLightmapGen() { PRINT_STUB(__FUNCTION__); }
int lm_AllocLightmap(int w, int h) { PRINT_STUB(__FUNCTION__); return -1; }
void lm_FreeLightmap(int n) { PRINT_STUB(__FUNCTION__); }
uint16_t *lm_data(int handle) { PRINT_STUB(__FUNCTION__); return nullptr; }
int lm_w(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lm_h(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lmi_h(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lmi_w(int n) { PRINT_STUB(__FUNCTION__); return 0; }
void SetupObjectLightmapMemory(object *obj) { PRINT_STUB(__FUNCTION__); }
void ClearAllVolumeLights() { PRINT_STUB(__FUNCTION__); }

// ==================== Player/ship ====================
player Players[MAX_PLAYERS] = {};
int Num_player_shapes = 0;
ship Ships[MAX_SHIPS] = {};

// ==================== Door ====================
door Doors[MAX_DOORS];
doorway *DoorwayAdd(room *rp, int doornum) { PRINT_STUB(__FUNCTION__); return nullptr; }

// ==================== Weapon ====================
weapon Weapons[MAX_WEAPONS] = {};
// Sentinel-terminated list (empty) so callers strcmp("", name[i]) cleanly.
const char *const Static_weapon_names[] = {""};
const int Static_weapon_names_msg[] = {0};
void WBClearInfo(otype_wb_info *wb) { PRINT_STUB(__FUNCTION__); }

// ==================== Megacell ====================
megacell Megacells[MAX_MEGACELLS] = {};

// ==================== Matcen ====================
matcen *Matcen[MAX_MATCENS] = {};

// ==================== Level ====================
levelgoals Level_goals;
level_info Level_info;
void levelgoals::CleanupAfterLevel() { PRINT_STUB(__FUNCTION__); }

// ==================== Ambient ====================
ambient_life a_life;
void ambient_life::ALReset() { PRINT_STUB(__FUNCTION__); }

// ==================== Manage ====================
mngs_track_lock GlobalTrackLocks[MAX_TRACKLOCKS] = {};
int mng_AllocTrackLock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return -1; }
void mng_FreeTrackLock(int n) { PRINT_STUB(__FUNCTION__); }
int mng_FindTrackLock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return -1; }
int mng_CheckIfPageLocked(mngs_Pagelock *p) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_CheckIfPageOwned(mngs_Pagelock *p, const std::string &a) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_DeletePage(const std::string &a, int b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_DeletePagelock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
void mng_EraseLocker() { PRINT_STUB(__FUNCTION__); }
int mng_MakeLocker() { PRINT_STUB(__FUNCTION__); return 0; }
void mng_OverrideToUnlocked(mngs_Pagelock *p) { PRINT_STUB(__FUNCTION__); }
int mng_RenamePage(const std::string &a, const std::string &b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_ReplacePage(const std::string &a, const std::string &b, int c, int d, int e) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_ReplacePagelock(const std::string &a, mngs_Pagelock *b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificDoorPage(const std::string &a, mngs_door_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificGenericPage(const std::string &a, mngs_generic_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificShipPage(const std::string &a, mngs_ship_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificSoundPage(const std::string &a, mngs_sound_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificTexPage(const std::string &a, mngs_texture_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificWeaponPage(const std::string &a, mngs_weapon_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignDoorPageToDoor(mngs_door_page *a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignGenericPageToObjInfo(mngs_generic_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignShipPageToShip(mngs_ship_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignSoundPageToSound(mngs_sound_page *a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignTexPageToTexture(mngs_texture_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignWeaponPageToWeapon(mngs_weapon_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }

// ==================== CFILE (implemented in cfile/cfile.cpp) ====================

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
  memset(&rp->flags, 0, sizeof(uint32_t));
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
  rp->name.clear();
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
void ClearAllRoomLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
void ClearRoomLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
void ClearAllRoomSpecmaps(int n) { PRINT_STUB(__FUNCTION__); }

room Rooms[MAX_ROOMS] = {};
int Highest_room_index = -1;

// Zeroes the whole room table and selection globals used by the editor.
void InitRooms() {
  for (int i = 0; i < MAX_ROOMS; i++) {
    if (Rooms[i].used) {
      FreeRoom(&Rooms[i]);
    }
  }
  Highest_room_index = -1;
  Curroomp = nullptr;
  Markedroomp = nullptr;
  Markedface = -1;
  Curface = -1;
  Curvert = -1;
  Curedge = -1;
  Curportal = -1;
}

// ==================== DDIO ====================
void ddio_SplitPath(const char *path, char *dir, char *fname, char *ext) { PRINT_STUB(__FUNCTION__); if (dir) dir[0] = 0; if (fname) fname[0] = 0; if (ext) ext[0] = 0; }
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *l, int *r) { PRINT_STUB(__FUNCTION__); if (x) *x = 0; if (y) *y = 0; if (dx) *dx = 0; if (dy) *dy = 0; if (l) *l = 0; if (r) *r = 0; return 0; }
float ddio_KeyDownTime(int key) { PRINT_STUB(__FUNCTION__); return 0; }
int ddio_GetFileLength(FILE *filePtr) {
  if (filePtr == nullptr)
    return 0;
  long cur = ftell(filePtr);
  fseek(filePtr, 0, SEEK_END);
  long len = ftell(filePtr);
  fseek(filePtr, cur, SEEK_SET);
  return (int)len;
}
void joy_GetPos(tJoystick joy, tJoyPos *pos) { PRINT_STUB(__FUNCTION__); if (pos) { pos->x = 0; pos->y = 0; pos->z = 0; } }
bool joy_IsValid(tJoystick id) { PRINT_STUB(__FUNCTION__); return false; }
bool ddio_FileDiff(const std::filesystem::path& a, const std::filesystem::path& b) {
  if (a == b)
    return false;
  if (!std::filesystem::exists(a) || !std::filesystem::exists(b))
    return true;
  return std::filesystem::file_size(a) != std::filesystem::file_size(b);
}
void ddio_CopyFileTime(const std::filesystem::path& dest, const std::filesystem::path& src) { PRINT_STUB(__FUNCTION__); (void)dest; (void)src; }


namespace D3 {
const std::vector<std::filesystem::path> Default_read_only_base_directories;
}

void Error(const char *fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  qCritical("%s", buf);
  std::abort();
}

// ==================== ChronoTimer ====================
float D3::ChronoTimer::GetTime() { PRINT_STUB(__FUNCTION__); return 0; }
void D3::ChronoTimer::Initialize() { PRINT_STUB(__FUNCTION__); }

// ==================== Init ====================
// Command-line argument store. GatherArgs collects the argv tokens (including
// argv[0] as index 0) so FindArg/GetArg can be used to pass options such as
// "-datadir <path>" to locate the game data files.
char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG] = {};

void GatherArgs(char **argv) {
  if (argv == nullptr)
    return;
  int n = 0;
  for (int i = 0; argv[i] && i < MAX_ARGS; i++) {
    std::strncpy(GameArgs[n++], argv[i], MAX_CHARS_PER_ARG - 1);
    GameArgs[n - 1][MAX_CHARS_PER_ARG - 1] = '\0';
  }
}

void GatherArgs(const char *str) {
  if (str == nullptr)
    return;
  int n = 0;
  const char *p = str;
  while (*p && n < MAX_ARGS) {
    while (*p == ' ')
      p++;
    if (!*p)
      break;
    int len = 0;
    while (p[len] && p[len] != ' ' && len < MAX_CHARS_PER_ARG - 1)
      len++;
    std::memcpy(GameArgs[n], p, len);
    GameArgs[n][len] = '\0';
    n++;
    p += len;
  }
}

int FindArg(const char *which, int start) {
  if (which == nullptr)
    return 0;
  for (int i = start; i < MAX_ARGS; i++) {
    if (GameArgs[i][0] && strcasecmp(GameArgs[i], which) == 0)
      return i;
  }
  return 0;
}

int FindArgChar(const char *which, char singleCharArg) { return FindArg(which); }

const char *GetArg(int index) {
  if (index < 0 || index >= MAX_ARGS)
    return "";
  return GameArgs[index];
}

// Minimal pre-init: establish the memory and error subsystems that the
// ported engine code relies on before anything else runs.
void PreInitD3Systems() {
  if (FindArg("-lowmem") || FindArg("-dedicated"))
    Mem_low_memory_mode = true;
  if (FindArg("-superlowmem")) {
    Mem_low_memory_mode = true;
    Mem_superlow_memory_mode = true;
  }
  if (FindArg("-himem")) {
    Mem_low_memory_mode = false;
    Mem_superlow_memory_mode = false;
  }
}

// First-phase system init. The decoupled editor replaces the full game's I/O,
// graphics, sound and network stacks with Qt + OpenGL and a HOG-capable
// cfile; here we initialise the data structures the editor core uses.
void InitD3Systems1(bool /*editor*/) {
  InitLightmapInfo();
  InitRooms();
  ResetObjectList();
}

// Second-phase system init. In the Win32 editor this is where the gamedata
// table files are loaded (mng_LoadTableFiles) so levels opened later can
// reference object/ship/weapon/sound/texture data. The decoupled editor does
// the same in initD3Core via loadGameDataTable() (the data dir must be
// resolved there first); this hook preserves the original two-phase shape.
void InitD3Systems2(bool /*editor*/) { PRINT_STUB(__FUNCTION__); }
void InitGamePaths() { PRINT_STUB(__FUNCTION__); }
void ProgramVersion(int a, unsigned char b, unsigned char c, unsigned char d) { PRINT_STUB(__FUNCTION__); }

// ==================== Application ====================
static tLnxAppInfo _default_app_info;

// ==================== Mem ====================
bool Mem_low_memory_mode = false;
bool Mem_superlow_memory_mode = false;
void mem_Init() { PRINT_STUB(__FUNCTION__); }
void mem_Shutdown() { PRINT_STUB(__FUNCTION__); }
int mem_GetTotalMemoryUsed() { PRINT_STUB(__FUNCTION__); return 0; }



// ==================== grtext ====================
void grtext_Init() { PRINT_STUB(__FUNCTION__); }

// ==================== Application ====================
oeLnxApplication::oeLnxApplication(tLnxAppInfo *info) { PRINT_STUB(__FUNCTION__); }
oeLnxApplication::~oeLnxApplication() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::init() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::get_info(void *appinfo) { PRINT_STUB(__FUNCTION__); }
unsigned oeLnxApplication::defer() { PRINT_STUB(__FUNCTION__); return 0; }
const char *oeLnxApplication::get_window_name() { PRINT_STUB(__FUNCTION__); return "D3Editor"; }
void oeLnxApplication::clear_window() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::set_defer_handler(void (*func)(bool isactive)) { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::delay(float secs) { PRINT_STUB(__FUNCTION__); }
int oeLnxApplication::flags() const { PRINT_STUB(__FUNCTION__); return 0; }

oeLnxAppDatabase::oeLnxAppDatabase() { PRINT_STUB(__FUNCTION__); }
oeLnxAppDatabase::~oeLnxAppDatabase() { PRINT_STUB(__FUNCTION__); }
bool oeLnxAppDatabase::create_record(const char *pathname) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::lookup_record(const char *pathname) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, char *entry, int *entrylen) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, void *entry, int wordsize) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, bool *entry) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::write(const char *label, const char *entry, int entrylen) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::write(const char *label, int entry) { PRINT_STUB(__FUNCTION__); return false; }
void oeLnxAppDatabase::get_user_name(char *buffer, size_t *size) { PRINT_STUB(__FUNCTION__); if (buffer) buffer[0] = 0; }

oeApplication *Descent = nullptr;
oeAppDatabase *Database = nullptr;

// ==================== Find* ====================

int FindDoorName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int FindGamePathName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int FindObjectIDName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int FindShipName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int FindSoundName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
// FindTextureName: look up a texture by name in the loaded GameTextures table.
// Returns the global texture index or -1 if absent.  Used by the level loader's
// TXNM texture remap and by gamedata cross-references.
int FindTextureName(const std::string &name) {
  for (int i = 0; i < Num_textures; i++) {
    if (!GameTextures[i].name.empty() && name == GameTextures[i].name)
      return i;
  }
  std::runtime_error("texture not found!");
  return -1;
}
int FindWeaponName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Alloc/Free ====================
int AllocDoor() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeDoor(int n) { PRINT_STUB(__FUNCTION__);}
int AllocObjectID(int id, bool a, bool b, bool c) { PRINT_STUB(__FUNCTION__); return -1; }
void FreeObjectID(int n) { PRINT_STUB(__FUNCTION__); }
int GetObjectID(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int AllocShip() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeShip(int n) { PRINT_STUB(__FUNCTION__); }
int AllocSound() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeSound(int n) { PRINT_STUB(__FUNCTION__); }
int AllocSpecialFace(int a, int b, bool c, int d) { PRINT_STUB(__FUNCTION__); return -1; }
void FreeSpecialFace(int n) { PRINT_STUB(__FUNCTION__); }
int AllocTexture() {  PRINT_STUB(__FUNCTION__); return -1; }
void FreeTexture(int n) { PRINT_STUB(__FUNCTION__); }
int AllocWeapon() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeWeapon(int n) { PRINT_STUB(__FUNCTION__); }
int AllocLightmapInfo(int a, int b, int c, bool d) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== GetNext/Prev ====================
int GetNextDoor(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevDoor(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextMegacell(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevMegacell(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextShip(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevShip(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextSound(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevSound(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextTexture(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPreviousTexture(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextWeapon(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevWeapon(int n) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Remap ====================
void RemapShips() { PRINT_STUB(__FUNCTION__); }
void RemapSounds() { PRINT_STUB(__FUNCTION__); }
void RemapStaticIDs() { PRINT_STUB(__FUNCTION__); }
void RemapWeapons() { PRINT_STUB(__FUNCTION__); }

// ==================== Load ====================
int LoadDoorImage(const std::filesystem::path& name, int n) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadPolyModel(const std::filesystem::path& name, int f_module) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadShipImage(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadSoundFile(const char *name, float vol, bool b) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadTextureImage(const std::filesystem::path& name, int *handle, int a, int b, int c, int d) { PRINT_STUB(__FUNCTION__); return -1; }
int GetDoorImage(int n) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Polymodel ====================
poly_model *GetPolymodelPointer(int n) { PRINT_STUB(__FUNCTION__); return nullptr; }
int CountFacesInPolymodel(poly_model *pm) { PRINT_STUB(__FUNCTION__); return 0; }
float ComputeDefaultSize(int type, int handle, float *size) { PRINT_STUB(__FUNCTION__); return 0; }
void PageInPolymodel(int model_num, int f_damage, float *size) { PRINT_STUB(__FUNCTION__); }
void SetModelAnglesAndPos(poly_model *pm, float *anim, unsigned int flags) { PRINT_STUB(__FUNCTION__); }
int IsNonRenderableSubmodel(poly_model *pm, int index) { PRINT_STUB(__FUNCTION__); return 0; }
void ChangeOldModelsForObjects(int a, int b) { PRINT_STUB(__FUNCTION__); }
std::filesystem::path ChangePolyModelName(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return name; }
void GenerateLODDeltas() { PRINT_STUB(__FUNCTION__); }
int FindPolyModelName(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return -1; }
void FreePolyModel(int n) { PRINT_STUB(__FUNCTION__); }

// ==================== Misc ====================
int CreateMatcen(const char *name, bool *flag) { PRINT_STUB(__FUNCTION__); return -1; }
void DestroyAllMatcens() { PRINT_STUB(__FUNCTION__); }
void FreeAllGamePaths() { PRINT_STUB(__FUNCTION__); }
void FreeGamePath(int n) { PRINT_STUB(__FUNCTION__); }
int FindPointRoom(vector *pnt) { PRINT_STUB(__FUNCTION__); return -1; }
int GetTerrainRoomFromPos_ret(vector *pos) { PRINT_STUB(__FUNCTION__); return -1; }
int AIMakeNextRoomList(int roomnum, int *next_rooms, int max_rooms) { PRINT_STUB(__FUNCTION__); return 0; }
bool PhysCalcGround(vector *ground_point, vector *ground_normal, object *obj, int ground_num) { PRINT_STUB(__FUNCTION__); return false; }
void ClearAllEvents() { PRINT_STUB(__FUNCTION__); }

// ==================== FVI ====================
int fvi_FindIntersection(fvi_query *q, fvi_info *info, bool b) { memset(info, 0, sizeof(fvi_info)); return 0; }
int fvi_QuickDistFaceList(int init_room_index, vector *pos, float rad, fvi_face_room_list *quick_fr_list, int max_elements) { PRINT_STUB(__FUNCTION__); return 0; }
bool FVI_always_check_ceiling = false;

// ==================== OSIRIS ====================
int osipf_FindObjectName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int osipf_FindRoomName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int osipf_FindTriggerName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Pilot ====================
void PltClearList() { PRINT_STUB(__FUNCTION__); }
std::vector<std::string> PltGetPilots(std::string ignore, int display) { return {}; }


// ==================== Bitmap ====================
// The mini tree has no InitBitmaps()/Bitmaps_initted global; GameBitmaps is a
// zero-initialised static array so allocation is always considered ready.

// Allocs a bitmap of w x h size and sets the data16 buffer (16-bit pixels).
// Returns bitmap handle if successful, -1 otherwise.
int bm_AllocBitmap(int w, int h, int add_mem) {
  int n = -1;
  for (int i = 0; i < MAX_BITMAPS; i++) {
    if (GameBitmaps[i].used == 0) {
      n = i;
      break;
    }
  }
  if (n == -1)
    return -1;

  memset(&GameBitmaps[n], 0, sizeof(bms_bitmap));
  GameBitmaps[n].width = (uint16_t)w;
  GameBitmaps[n].height = (uint16_t)h;
  GameBitmaps[n].format = BITMAP_FORMAT_STANDARD;
  GameBitmaps[n].flags = BF_CHANGED | BF_BRAND_NEW;
  GameBitmaps[n].cache_slot = -1;

  if (w > 0 && h > 0) {
    // add_mem is the extra space for mip levels (approx (w*h*2)/3 each when
    // mipped). Allocate the base level plus that padding so the mipped OGF
    // decoder can write level 0 and each half-size level into consecutive
    // memory, matching how bm_data(handle, m) computes its offset.
    size_t base = (size_t)w * (size_t)h * 2;
    size_t extra = (add_mem > 0) ? (size_t)add_mem : 0;
    GameBitmaps[n].data16 = (uint16_t *)mem_malloc(base + extra);
    if (!GameBitmaps[n].data16) {
      GameBitmaps[n].used = 0;
      return -1;
    }
    memset(GameBitmaps[n].data16, 0, base + extra);
    if (add_mem > 0)
      GameBitmaps[n].mip_levels = 1; // marked mipped; actual count set by caller
  } else {
    GameBitmaps[n].data16 = nullptr;
  }

  GameBitmaps[n].used = 1;
  return n;
}

uint16_t *bm_data(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return nullptr;
  const bms_bitmap &b = GameBitmaps[handle];
  if (b.data16 == nullptr)
    return nullptr;
  int offset = 0, w = b.width, h = b.height;
  for (int m = 0; m < miplevel; m++) {
    offset += w * h;
    if (w > 1) w >>= 1;
    if (h > 1) h >>= 1;
  }
  return b.data16 + offset;
}
int bm_w(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  int w = GameBitmaps[handle].width;
  for (int m = 0; m < miplevel; m++) {
    if (w > 1) w >>= 1;
  }
  return w;
}
int bm_h(int handle, int miplevel) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  int h = GameBitmaps[handle].height;
  for (int m = 0; m < miplevel; m++) {
    if (h > 1) h >>= 1;
  }
  return h;
}
int bm_mipped(int handle) {
  if (handle < 0 || handle >= MAX_BITMAPS)
    return 0;
  return GameBitmaps[handle].mip_levels > 1 ? 1 : 0;
}

// ==================== Inventory ====================
Inventory::Inventory() { PRINT_STUB(__FUNCTION__); }
Inventory::~Inventory() { PRINT_STUB(__FUNCTION__); }

// ==================== grFont ====================
grFont::grFont() { PRINT_STUB(__FUNCTION__); }
grFont::~grFont() { PRINT_STUB(__FUNCTION__); }

// ==================== hlsSystem ====================
hlsSystem::hlsSystem() { PRINT_STUB(__FUNCTION__); }
void hlsSystem::KillSoundLib(bool f_kill_sound_list) { PRINT_STUB(__FUNCTION__); }
int hlsSystem::Play2dSound(int sound_index, float volume, float pan, unsigned short flags) { PRINT_STUB(__FUNCTION__); return 0; }
bool hlsSystem::CheckAndForceSoundDataAlloc(int sound_file_index) { PRINT_STUB(__FUNCTION__); return false; }

// ==================== matcen methods ====================
std::string matcen::GetName(void) { PRINT_STUB(__FUNCTION__); return std::string(); }
bool matcen::SetName(const std::string& name) { PRINT_STUB(__FUNCTION__); return false; }
