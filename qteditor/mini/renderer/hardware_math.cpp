/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Qt port: hardware-renderer math layer (points / clipper / instance stack /
 * transforms / frame setup) ported from the engine renderer/Hardware*.cpp
 * files.  These are the pure-math leaves the poly-model render path depends on
 * (RenderSubmodel / RenderPolygonModel / DrawPolygonModel).  Rendering state
 * globals live in mini/renderer/HardwareGlobalVars.cpp.
 */

#include "3d.h"
#include "HardwareInternal.h"
#include "renderer.h"
#include "utils.h"
#include <QtGlobal>

#include <cstring>

// ===========================================================================
// Free-point pool (clipper temp points) -- HardwareClipper.cpp
// ===========================================================================

#define MAX_POINTS_IN_POLY 100

static int free_point_num = -1;
static g3Point temp_points[MAX_POINTS_IN_POLY];
static g3Point *free_points[MAX_POINTS_IN_POLY];
vector3 Clip_plane_point;

void InitFreePoints(void) {
  int i;
  for (i = 0; i < MAX_POINTS_IN_POLY; i++)
    free_points[i] = &temp_points[i];
  free_point_num = 0;
}

g3Point *GetTempPoint(void) {
  Q_ASSERT(free_point_num < MAX_POINTS_IN_POLY);
  g3Point *p = free_points[free_point_num++];
  p->p3_flags = g3point_flags_t{.temp_point = true};
  return p;
}

void FreeTempPoint(g3Point *p) {
  if (free_point_num < 1) {
    Q_ASSERT(false);
    free_point_num = 1;
  }
  Q_ASSERT(p->p3_flags.temp_point);
  free_points[--free_point_num] = p;
  p->p3_flags.temp_point = false;
}

// Verify that all the temp points are free, and free them if they are not.
static void checkTempPointsInternal(void) {
  if (free_point_num) {
    Q_ASSERT(false);
    InitFreePoints();
  }
}

// Free up any temp points (created by the clipper) in the given pointlist
void g3_FreeTempPoints(g3Point **pointlist, int nv) {
  int i;
  for (i = 0; i < nv; i++) {
    if (pointlist[i]->p3_flags.temp_point)
      FreeTempPoint(pointlist[i]);
  }
  checkTempPointsInternal();
}

// Clips an edge against the far plane
g3Point *ClipFarEdge(g3Point *on_pnt, g3Point *off_pnt) {
  float z_on = on_pnt->p3_z;
  float z_off = off_pnt->p3_z;
  float k = 1.0f - ((z_off - Far_clip_z) / (z_off - z_on));

  g3Point *tmp = GetTempPoint();
  tmp->p3_z = on_pnt->p3_z + ((off_pnt->p3_z - on_pnt->p3_z) * k);
  tmp->p3_x = on_pnt->p3_x + ((off_pnt->p3_x - on_pnt->p3_x) * k);
  tmp->p3_y = on_pnt->p3_y + ((off_pnt->p3_y - on_pnt->p3_y) * k);

  if (on_pnt->p3_flags.uv) {
    tmp->p3_u = on_pnt->p3_u + ((off_pnt->p3_u - on_pnt->p3_u) * k);
    tmp->p3_v = on_pnt->p3_v + ((off_pnt->p3_v - on_pnt->p3_v) * k);
    tmp->p3_flags.uv = true;
  }
  if (on_pnt->p3_flags.uv2) {
    tmp->p3_u2 = on_pnt->p3_u2 + ((off_pnt->p3_u2 - on_pnt->p3_u2) * k);
    tmp->p3_v2 = on_pnt->p3_v2 + ((off_pnt->p3_v2 - on_pnt->p3_v2) * k);
    tmp->p3_flags.uv2 = true;
  }
  if (on_pnt->p3_flags.lighting) {
    tmp->p3_l = on_pnt->p3_l + ((off_pnt->p3_l - on_pnt->p3_l) * k);
    tmp->p3_flags.lighting = true;
  }
  if (on_pnt->p3_flags.rgba) {
    tmp->p3_r = on_pnt->p3_r + ((off_pnt->p3_r - on_pnt->p3_r) * k);
    tmp->p3_g = on_pnt->p3_g + ((off_pnt->p3_g - on_pnt->p3_g) * k);
    tmp->p3_b = on_pnt->p3_b + ((off_pnt->p3_b - on_pnt->p3_b) * k);
    tmp->p3_a = on_pnt->p3_a + ((off_pnt->p3_a - on_pnt->p3_a) * k);
    tmp->p3_flags.rgba = true;
  }

  g3_CodePoint(tmp);
  return tmp;
}

// Clips an edge against the custom plane
g3Point *ClipCustomEdge(g3Point *on_pnt, g3Point *off_pnt) {
  g3Point *tmp = GetTempPoint();

  vector3 ray_direction = off_pnt->p3_vec - on_pnt->p3_vec;
  ray_direction /= Matrix_scale;

  vector3 w = on_pnt->p3_vec - Clip_plane_point;
  w /= Matrix_scale;

  scalar k, den = -vm_Dot3Product(Clip_plane, ray_direction);
  if (den == 0.0f)
    k = 1.0f;
  else {
    scalar num = vm_Dot3Product(Clip_plane, w);
    k = num / den;
  }

  tmp->p3_vec = on_pnt->p3_vec + ((off_pnt->p3_vec - on_pnt->p3_vec) * k);

  if (on_pnt->p3_flags.uv) {
    tmp->p3_u = on_pnt->p3_u + ((off_pnt->p3_u - on_pnt->p3_u) * k);
    tmp->p3_v = on_pnt->p3_v + ((off_pnt->p3_v - on_pnt->p3_v) * k);
    tmp->p3_flags.uv = true;
  }
  if (on_pnt->p3_flags.uv2) {
    tmp->p3_u2 = on_pnt->p3_u2 + ((off_pnt->p3_u2 - on_pnt->p3_u2) * k);
    tmp->p3_v2 = on_pnt->p3_v2 + ((off_pnt->p3_v2 - on_pnt->p3_v2) * k);
    tmp->p3_flags.uv2 = true;
  }
  if (on_pnt->p3_flags.lighting) {
    tmp->p3_l = on_pnt->p3_l + ((off_pnt->p3_l - on_pnt->p3_l) * k);
    tmp->p3_flags.lighting = true;
  }
  if (on_pnt->p3_flags.rgba) {
    tmp->p3_r = on_pnt->p3_r + ((off_pnt->p3_r - on_pnt->p3_r) * k);
    tmp->p3_g = on_pnt->p3_g + ((off_pnt->p3_g - on_pnt->p3_g) * k);
    tmp->p3_b = on_pnt->p3_b + ((off_pnt->p3_b - on_pnt->p3_b) * k);
    tmp->p3_a = on_pnt->p3_a + ((off_pnt->p3_a - on_pnt->p3_a) * k);
    tmp->p3_flags.rgba = true;
  }

  g3_CodePoint(tmp);
  return tmp;
}

// clips an edge against one plane
g3Point *ClipEdge(g3_clip_codes_t plane_flag, g3Point *on_pnt, g3Point *off_pnt) {
  float a, b, k;
  g3Point *tmp;

  if (plane_flag.off_far)
    return ClipFarEdge(on_pnt, off_pnt);

  if (plane_flag.off_custom && Clip_custom)
    return ClipCustomEdge(on_pnt, off_pnt);

  if (plane_flag.off_right || plane_flag.off_left) {
    a = on_pnt->p3_x;
    b = off_pnt->p3_x;
  } else {
    a = on_pnt->p3_y;
    b = off_pnt->p3_y;
  }

  if (plane_flag.off_left || plane_flag.off_bot) {
    a = -a;
    b = -b;
  }

  k = (a - on_pnt->p3_z) / ((a - on_pnt->p3_z) - b + off_pnt->p3_z);

  tmp = GetTempPoint();

  tmp->p3_x = on_pnt->p3_x + ((off_pnt->p3_x - on_pnt->p3_x) * k);
  tmp->p3_y = on_pnt->p3_y + ((off_pnt->p3_y - on_pnt->p3_y) * k);

  if (plane_flag.off_top || plane_flag.off_bot)
    tmp->p3_z = tmp->p3_y;
  else
    tmp->p3_z = tmp->p3_x;

  if (plane_flag.off_left || plane_flag.off_bot)
    tmp->p3_z = -tmp->p3_z;

  if (on_pnt->p3_flags.uv) {
    tmp->p3_u = on_pnt->p3_u + ((off_pnt->p3_u - on_pnt->p3_u) * k);
    tmp->p3_v = on_pnt->p3_v + ((off_pnt->p3_v - on_pnt->p3_v) * k);
    tmp->p3_flags.uv = true;
  }
  if (on_pnt->p3_flags.uv2) {
    tmp->p3_u2 = on_pnt->p3_u2 + ((off_pnt->p3_u2 - on_pnt->p3_u2) * k);
    tmp->p3_v2 = on_pnt->p3_v2 + ((off_pnt->p3_v2 - on_pnt->p3_v2) * k);
    tmp->p3_flags.uv2 = true;
  }
  if (on_pnt->p3_flags.lighting) {
    tmp->p3_l = on_pnt->p3_l + ((off_pnt->p3_l - on_pnt->p3_l) * k);
    tmp->p3_flags.lighting = true;
  }
  if (on_pnt->p3_flags.rgba) {
    tmp->p3_r = on_pnt->p3_r + ((off_pnt->p3_r - on_pnt->p3_r) * k);
    tmp->p3_g = on_pnt->p3_g + ((off_pnt->p3_g - on_pnt->p3_g) * k);
    tmp->p3_b = on_pnt->p3_b + ((off_pnt->p3_b - on_pnt->p3_b) * k);
    tmp->p3_a = on_pnt->p3_a + ((off_pnt->p3_a - on_pnt->p3_a) * k);
    tmp->p3_flags.rgba = true;
  }

  g3_CodePoint(tmp);
  return tmp;
}

// clips a line to the viewing pyramid
void ClipLine(g3Point **p0, g3Point **p1, g3_clip_codes_t codes_or) {
  g3Point *old_p1;
  uint8_t codes_byte = clip_code_byte(codes_or);

  for (uint8_t plane_flag = 1; plane_flag <= 32; plane_flag <<= 1) {
    if (codes_byte & plane_flag) {
      if (clip_code_byte((*p0)->p3_codes) & plane_flag) {
        g3Point *t = *p0;
        *p0 = *p1;
        *p1 = t;
      }

      old_p1 = *p1;
      *p1 = ClipEdge(clip_codes_from_byte(plane_flag), *p0, *p1);
      codes_or = clip_codes_or((*p0)->p3_codes, (*p1)->p3_codes);
      codes_byte = clip_code_byte(codes_or);

      if (old_p1->p3_flags.temp_point)
        FreeTempPoint(old_p1);
    }
  }
}

// Clips a polygon against one plane
int ClipPlane(g3_clip_codes_t plane_flag, g3Point **src, g3Point **dest, int *nv, g3Codes *cc) {
  int i, prev, next;
  g3Point **save_dest = dest;
  const uint8_t flag_byte = clip_code_byte(plane_flag);

  cc->cc_and = clip_codes_from_byte(0xff);
  cc->cc_or = {};

  for (i = 0, prev = *nv - 1, next = 1; i < *nv; i++) {
    if (clip_code_byte(src[i]->p3_codes) & flag_byte) {
      if (!(clip_code_byte(src[prev]->p3_codes) & flag_byte)) {
        *dest = ClipEdge(plane_flag, src[prev], src[i]);
        cc->cc_or = clip_codes_or(cc->cc_or, (*dest)->p3_codes);
        cc->cc_and = clip_codes_and(cc->cc_and, (*dest)->p3_codes);
        dest++;
      }
      if (!(clip_code_byte(src[next]->p3_codes) & flag_byte)) {
        *dest = ClipEdge(plane_flag, src[next], src[i]);
        cc->cc_or = clip_codes_or(cc->cc_or, (*dest)->p3_codes);
        cc->cc_and = clip_codes_and(cc->cc_and, (*dest)->p3_codes);
        dest++;
      }
      if (src[i]->p3_flags.temp_point)
        FreeTempPoint(src[i]);
    } else {
      *dest++ = src[i];
      cc->cc_or = clip_codes_or(cc->cc_or, src[i]->p3_codes);
      cc->cc_and = clip_codes_and(cc->cc_and, src[i]->p3_codes);
    }

    prev = i;
    if (++next == *nv)
      next = 0;
  }

  return (dest - save_dest);
}

static g3Point *Vbuf0[MAX_POINTS_IN_POLY];
static g3Point *Vbuf1[MAX_POINTS_IN_POLY];

// Clips a polygon.  Caller MUST call g3_FreeTempPoints() when done.
g3Point **g3_ClipPolygon(g3Point **pointlist, int *nv, g3Codes *cc) {
  g3Point **dest = Vbuf0;

  if (free_point_num != 0) {
    Q_ASSERT(false);
    free_point_num = 0;
  }

  uint8_t codes = clip_code_byte(cc->cc_or);
  for (uint8_t plane_flag = 1; plane_flag <= 32; plane_flag <<= 1) {
    if (codes & plane_flag) {
      *nv = ClipPlane(clip_codes_from_byte(plane_flag), pointlist, dest, nv, cc);
      if (clip_code_byte(cc->cc_and) != 0) // clipped away
        return dest;

      pointlist = dest;
      dest = (pointlist == Vbuf0) ? Vbuf1 : Vbuf0;
      codes = clip_code_byte(cc->cc_or);
    }
  }
  return pointlist;
}

void g3_SetFarClipZ(float z) { Far_clip_z = z; }

void g3_SetCustomClipPlane(uint8_t state, vector3 *pnt, vector3 *normal) {
  Clip_custom = state;
  if (state) {
    vector3 tempv;
    vector3 norm = *normal;
    tempv = *pnt - View_position;
    Clip_plane_point = tempv * View_matrix;
    Clip_plane = norm * Unscaled_matrix;
    vm_NormalizeVector(&Clip_plane);
  }
}

// ===========================================================================
// Point coding / rotation / projection -- HardwarePoints.cpp
// ===========================================================================

// code a point; fills in the p3_codes field and returns the codes
uint8_t g3_CodePoint(g3Point *p) {
  g3_clip_codes_t cc{};

  if (p->p3_x > p->p3_z)
    cc.off_right = true;
  if (p->p3_y > p->p3_z)
    cc.off_top = true;
  if (p->p3_x < -p->p3_z)
    cc.off_left = true;
  if (p->p3_y < -p->p3_z)
    cc.off_bot = true;
  if (p->p3_z < 0)
    cc.behind = true;
  if (p->p3_z > Far_clip_z)
    cc.off_far = true;

  if (Clip_custom) {
    vector3 vec = p->p3_vec - Clip_plane_point;
    vec /= Matrix_scale;
    scalar dp = vm_Dot3Product(vec, Clip_plane);
    if (dp < -0.005f)
      cc.off_custom = true;
  }

  p->p3_codes = cc;
  return clip_code_byte(cc);
}

// rotates a point; returns codes
uint8_t g3_RotatePoint(g3Point& dest, const vector3& src) {
  dest.p3_vecPreRot = src;
  vector3 tempv = src - View_position;
  dest.p3_vec = tempv * View_matrix;
  dest.p3_flags = g3point_flags_t{.origpoint = true};
  return g3_CodePoint(&dest);
}

// projects a point
void g3_ProjectPoint(g3Point *p) {
  if (p->p3_flags.projected || p->p3_codes.behind)
    return;

  float one_over_z = 1.0 / p->p3_z;
  p->p3_sx = Window_w2 + (p->p3_x * (Window_w2 * one_over_z));
  p->p3_sy = Window_h2 - (p->p3_y * (Window_h2 * one_over_z));
  p->p3_flags.projected = true;
}

// from a 2d point, compute the vector through that point
void g3_Point2Vec(vector3 *v, int16_t sx, int16_t sy) {
  vector3 tempv;
  matrix tempm;

  tempv.x() = (((sx - Window_w2) / Window_w2) * Matrix_scale.z() / Matrix_scale.x());
  tempv.y() = -(((sy - Window_h2) / Window_h2) * Matrix_scale.z() / Matrix_scale.y());
  tempv.z() = 1.0f;

  vm_NormalizeVector(&tempv);

  tempm = ~Unscaled_matrix;
  *v = tempv * tempm;
}

// calculate the depth of a point - returns the z coord of the rotated point
float g3_CalcPointDepth(vector3 *pnt) {
  return ((pnt->x() - View_position.x()) * View_matrix.fvec.x()) + ((pnt->y() - View_position.y()) * View_matrix.fvec.y()) +
         ((pnt->z() - View_position.z()) * View_matrix.fvec.z());
}

// ===========================================================================
// Instance matrix stack -- HardwareInstance.cpp
// ===========================================================================

struct InstanceContext {
  matrix m_viewMatrix;
  matrix m_unscaledMatrix;
  vector3 m_viewPosition;
  float m_modelView[4][4];
};

#define MAX_INSTANCE_DEPTH 30
static InstanceContext sInstanceStack[MAX_INSTANCE_DEPTH];
static int sInstanceDepth = 0;

// instance at specified point with specified orientation
void g3_StartInstanceMatrix(vector3 &pos, const matrix &orient) {
  Q_ASSERT(sInstanceDepth < MAX_INSTANCE_DEPTH);

  sInstanceStack[sInstanceDepth].m_viewMatrix = View_matrix;
  sInstanceStack[sInstanceDepth].m_viewPosition = View_position;
  sInstanceStack[sInstanceDepth].m_unscaledMatrix = Unscaled_matrix;
  memcpy(sInstanceStack[sInstanceDepth].m_modelView, gTransformModelView, sizeof(gTransformModelView));
  ++sInstanceDepth;

  vector3 tempv = View_position - pos;
  View_position = tempv * orient;

  matrix tempm, tempm2 = ~orient;
  tempm = tempm2 * View_matrix;
  View_matrix = tempm;
  tempm = tempm2 * Unscaled_matrix;
  Unscaled_matrix = tempm;

  g3_GetModelViewMatrix(&View_position, &Unscaled_matrix, (float *)gTransformModelView);
  g3_UpdateFullTransform();
}

// instance at specified point with specified angles
void g3_StartInstanceAngles(vector3 &pos, optref<angvec> angles) {
  if (!angles.has_value()) {
    matrix ident;
    vm_MakeIdentity(&ident);
    g3_StartInstanceMatrix(pos, ident);
    return;
  }
  matrix tm;
  vm_AnglesToMatrix(&tm, angles->p(), angles->h(), angles->b());
  g3_StartInstanceMatrix(pos, tm);
}

// pops the old context
void g3_DoneInstance() {
  --sInstanceDepth;
  Q_ASSERT(sInstanceDepth >= 0);
  View_position = sInstanceStack[sInstanceDepth].m_viewPosition;
  View_matrix = sInstanceStack[sInstanceDepth].m_viewMatrix;
  Unscaled_matrix = sInstanceStack[sInstanceDepth].m_unscaledMatrix;
  memcpy(gTransformModelView, sInstanceStack[sInstanceDepth].m_modelView, sizeof(gTransformModelView));
  g3_UpdateFullTransform();
}

// ===========================================================================
// Transforms -- HardwareTransforms.cpp
// ===========================================================================

static int sUseTransformPassthru = -1;

extern float Z_bias;

void g3_GetModelViewMatrix(const vector3 *viewPos, const matrix *viewMatrix, float *mvMat) {
  matrix localOrient = (*viewMatrix);
  vector3 localPos = -((vector3)*viewPos);
  mvMat[0] = localOrient.rvec.x();
  mvMat[1] = localOrient.uvec.x();
  mvMat[2] = localOrient.fvec.x();
  mvMat[3] = 0.0f;
  mvMat[4] = localOrient.rvec.y();
  mvMat[5] = localOrient.uvec.y();
  mvMat[6] = localOrient.fvec.y();
  mvMat[7] = 0.0f;
  mvMat[8] = localOrient.rvec.z();
  mvMat[9] = localOrient.uvec.z();
  mvMat[10] = localOrient.fvec.z();
  mvMat[11] = 0.0f;
  mvMat[12] = vm_Dot3Product(localPos, localOrient.rvec);
  mvMat[13] = vm_Dot3Product(localPos, localOrient.uvec);
  mvMat[14] = vm_Dot3Product(localPos, localOrient.fvec) + Z_bias;
  mvMat[15] = 1.0f;
}

void g3_TransformMult(float res[4][4], float a[4][4], float b[4][4]) {
  float temp[4][4];
  int x, y;
  for (y = 0; y < 4; ++y) {
    for (x = 0; x < 4; ++x) {
      temp[y][x] = (a[y][0] * b[0][x]) + (a[y][1] * b[1][x]) + (a[y][2] * b[2][x]) + (a[y][3] * b[3][x]);
    }
  }
  memcpy(res, temp, 16 * sizeof(float));
}

void g3_TransformTrans(float res[4][4], float t[4][4]) {
  float temp[4][4];
  int y, x;
  for (y = 0; y < 4; ++y) {
    for (x = 0; x < 4; ++x) {
      temp[x][y] = t[y][x];
    }
  }
  memcpy(res, temp, 16 * sizeof(float));
}

void g3_UpdateFullTransform() {
  g3_TransformMult(gTransformFull, gTransformModelView, gTransformProjection);
  g3_TransformMult(gTransformFull, gTransformFull, gTransformViewPort);
}

void g3_ForceTransformRefresh(void) { sUseTransformPassthru = -1; }

void g3_RefreshTransforms(bool usePassthru) {
  if (sUseTransformPassthru == 1 && usePassthru)
    return;

  if (usePassthru) {
    rend_TransformSetToPassthru();
  } else {
    int viewportWidth, viewportHeight, viewportX, viewportY;
    rend_GetProjectionScreenParameters(viewportX, viewportY, viewportWidth, viewportHeight);
    rend_TransformSetViewport(viewportX, viewportY, viewportWidth, viewportHeight);
    rend_TransformSetProjection(gTransformProjection);
    rend_TransformSetModelView(gTransformModelView);
  }

  sUseTransformPassthru = (usePassthru) ? 1 : 0;
}

// ===========================================================================
// Frame setup -- HardwareSetup.cpp
// ===========================================================================

static float sAspect = 0.0f;

void g3_SetAspectRatio(float aspect) { sAspect = aspect; }
float g3_GetAspectRatio() { return sAspect; }

void g3_GetViewPortMatrix(float *viewMat) {
  int viewportWidth, viewportHeight;
  int viewportX, viewportY;
  rend_GetProjectionScreenParameters(viewportX, viewportY, viewportWidth, viewportHeight);

  float viewportWidthOverTwo = ((float)viewportWidth) * 0.5f;
  float viewportHeightOverTwo = ((float)viewportHeight) * 0.5f;

  memset(viewMat, 0, sizeof(float) * 16);
  viewMat[0] = viewportWidthOverTwo;
  viewMat[5] = -viewportHeightOverTwo;
  viewMat[12] = viewportWidthOverTwo + (float)viewportX;
  viewMat[13] = viewportHeightOverTwo + (float)viewportY;
  viewMat[10] = viewMat[15] = 1.0f;
}

void g3_GetProjectionMatrix(float zoom, float *projMat) {
  int viewportWidth, viewportHeight;
  rend_GetProjectionParameters(&viewportWidth, &viewportHeight);

  float s = ((float)viewportWidth) / ((float)viewportHeight);
  float vertical_fov = zoom * 3.0f / 4.0f;

  memset(projMat, 0, sizeof(float) * 16);

  float oOT = 1.0f / vertical_fov;

  if (s <= 1.0f) {
    projMat[0] = oOT;
    projMat[5] = oOT * s;
  } else {
    projMat[0] = oOT / s;
    projMat[5] = oOT;
  }

  projMat[10] = 1.0f;
  projMat[11] = 1.0f;
  projMat[14] = -1.0f;
}

// start the frame
void g3_StartFrame(vector3 *view_pos, matrix *view_matrix, float zoom) {
  g3_GetViewPortMatrix((float *)gTransformViewPort);
  g3_GetProjectionMatrix(zoom, (float *)gTransformProjection);
  g3_GetModelViewMatrix(view_pos, view_matrix, (float *)gTransformModelView);
  g3_UpdateFullTransform();

  rend_GetProjectionParameters(&Window_width, &Window_height);

  Window_w2 = 0.5f * Window_width;
  Window_h2 = 0.5f * Window_height;

  scalar s = scalar(Window_height) / Window_width;
  Matrix_scale = vector3 { (s <= 1.0f ? s : 1.0f / s), 1.0f, 1.0f};

  zoom *= 3.f / 4.f;

  View_position = *view_pos;
  View_zoom = zoom;
  Unscaled_matrix = *view_matrix;

  float oOZ = 1.0f / View_zoom;
  Matrix_scale.x() = Matrix_scale.x() * oOZ;
  Matrix_scale.y() = Matrix_scale.y() * oOZ;

  View_matrix.rvec = Unscaled_matrix.rvec * Matrix_scale.x();
  View_matrix.uvec = Unscaled_matrix.uvec * Matrix_scale.y();
  View_matrix.fvec = Unscaled_matrix.fvec * Matrix_scale.z();

  InitFreePoints();
  g3_ResetFarClipZ();
}

void g3_EndFrame(void) { checkTempPointsInternal(); }

void g3_GetViewPosition(vector3 *vp) { *vp = View_position; }
void g3_GetViewMatrix(matrix *mat) { *mat = View_matrix; }
void g3_GetUnscaledMatrix(matrix *mat) { *mat = Unscaled_matrix; }
void g3_GetMatrixScale(vector3 *matrix_scale) { *matrix_scale = Matrix_scale; }

// ===========================================================================
// Line helper used by debug / box drawing (g3_DrawLine path).  Emits via the
// immediate-mode renderer.
// ===========================================================================

void rend_GetProjectionParameters(int *width, int *height) {
  if (width)
    *width = Window_width ? Window_width : 640;
  if (height)
    *height = Window_height ? Window_height : 480;
}

void rend_GetProjectionScreenParameters(int &screenLX, int &screenTY, int &screenW, int &screenH) {
  screenLX = 0;
  screenTY = 0;
  rend_GetProjectionParameters(&screenW, &screenH);
}
