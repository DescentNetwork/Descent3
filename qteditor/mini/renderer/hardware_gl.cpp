/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Qt port: immediate-mode OpenGL emitter for the hardware renderer.  Implements
 * the polygon/bitmap/line render functions and the rend_* state setters that the
 * poly-model path needs (RenderSubmodel / RenderPolygonModel / DrawPolygonModel),
 * using classic immediate-mode GL (glBegin/glVertex/glColor/glTexCoord).  It
 * renders into whatever GL context is current (a QOpenGLWidget's paintGL), so no
 * SDL window/context management is required.
 */

#include <GL/gl.h>
#include <cmath>

#include "3d.h"
#include "HardwareInternal.h"
#include "renderer.h"
#include "bitmap.h"
#include <QtGlobal>
#include "grdefs.h"

// ===========================================================================
// z bias (declared extern in renderer.h)
// ===========================================================================
float Z_bias = 0.0f;

// ===========================================================================
// Renderer state backing the rend_* setters
// ===========================================================================
static rendering_state s_state;

static texture_type s_textureType = TT_FLAT;
static color_model s_colorModel = CM_MONO;
static light_state s_lightState = LS_NONE;
static int8_t s_alphaType = AT_ALWAYS;
static wrap_type s_wrapType = WT_WRAP;
static int s_alpha = 255;
static ddgr_color s_flatColor = 0;
static float s_alphaFactor = 1.0f;
static int8_t s_textureQuality = 0;

void rend_SetTextureType(texture_type t) {
  s_textureType = t;
  s_state.cur_texture_type = t;
  s_textureQuality = (t == TT_FLAT) ? 0 : 1;
}
void rend_SetColorModel(color_model m) {
  s_colorModel = m;
  s_state.cur_color_model = m;
}
void rend_SetLighting(light_state l) {
  s_lightState = l;
  s_state.cur_light_state = l;
}
void rend_SetAlphaType(int8_t a) {
  s_alphaType = a;
  s_state.cur_alpha_type = a;
}
void rend_SetWrapType(wrap_type w) { s_wrapType = w; }
void rend_SetZBufferWriteMask(int) {}
void rend_SetCoplanarPolygonOffset(float) {}
void rend_SetBumpmapReadyState(int, int) {}
void rend_SetZBias(float) {}
void rend_SetOverlayMap(int) {}
void rend_SetFogState(int8_t) {}
void rend_SetFogBorders(float, float) {}
void rend_SetFogColor(ddgr_color) {}
void rend_SetZBufferState(int8_t) {}
void rend_SetAlphaValue(uint8_t a) { s_alpha = a; }
void rend_SetAlphaFactor(float f) { s_alphaFactor = f; }
float rend_GetAlphaFactor() { return s_alphaFactor; }
void rend_SetOverlayType(uint8_t) {}
void rend_SetFlatColor(ddgr_color c) { s_flatColor = c; }

void rend_TransformSetToPassthru(void) {}
void rend_TransformSetViewport(int, int, int, int) {}
void rend_TransformSetProjection(float[4][4]) {}
void rend_TransformSetModelView(float[4][4]) {}

int rend_ReInit() { return 1; }
float rend_GetAlphaMultiplier() { return 1.0f; }

// ===========================================================================
// Facing test -- HardwareDraw.cpp
// ===========================================================================
bool g3_CheckNormalFacing(vector3 *v, vector3 *norm) {
  vector3 tempv = View_position - *v;
  return (vm_Dot3Product(tempv, *norm) > 0);
}

int Triangulate_test = 0;
void g3_SetTriangulationTest(int state) { Triangulate_test = state; }

// ===========================================================================
// 16-bit pixel (555) -> RGBA upload
// ===========================================================================
static void convertBitmapToRGBA(int bm, std::vector<uint8_t> &rgba) {
  int w = bm_w(bm, 0);
  int h = bm_h(bm, 0);
  uint16_t *data = bm_data(bm, 0);
  rgba.resize((size_t)w * h * 4);
  for (int i = 0; i < w * h; i++) {
    uint16_t px = data[i];
    uint8_t r = (uint8_t)(((px >> 10) & 0x1f) << 3);
    uint8_t g = (uint8_t)(((px >> 5) & 0x1f) << 3);
    uint8_t b = (uint8_t)((px & 0x1f) << 3);
    rgba[i * 4 + 0] = r;
    rgba[i * 4 + 1] = g;
    rgba[i * 4 + 2] = b;
    rgba[i * 4 + 3] = 255;
  }
}

// uploads a bitmap into a GL texture; returns the GL texture handle
static GLuint uploadTexture(int bm) {
  std::vector<uint8_t> rgba;
  convertBitmapToRGBA(bm, rgba);
  int w = bm_w(bm, 0);
  int h = bm_h(bm, 0);
  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return tex;
}

// ===========================================================================
// Flat polygon (no texture) -- legacy opengl_DrawFlatPolygon
// ===========================================================================
void gpu_DrawFlatPolygon3D(g3Point **p, int nv) {
  float fr = (float)GR_COLOR_RED(s_flatColor) / 255.0f;
  float fg = (float)GR_COLOR_GREEN(s_flatColor) / 255.0f;
  float fb = (float)GR_COLOR_BLUE(s_flatColor) / 255.0f;

  glBegin(GL_POLYGON);
  for (int i = 0; i < nv; i++) {
    g3Point *pnt = p[i];
    if (s_alphaType & ATF_VERTEX)
      glColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, pnt->p3_a * s_alphaFactor);
    else if (s_lightState == LS_NONE)
      glColor4f(fr, fg, fb, s_alphaFactor);
    else if (s_colorModel == CM_MONO)
      glColor4f(pnt->p3_l, pnt->p3_l, pnt->p3_l, s_alphaFactor);
    else
      glColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, s_alphaFactor);

    float z = std::max(0.0f, std::min(1.0f, 1.0f - (1.0f / (pnt->p3_z + Z_bias))));
    glVertex3f(pnt->p3_sx, pnt->p3_sy, -z);
  }
  glEnd();
}

// ===========================================================================
// color_array Determination -- HardwareBaseGPU.cpp DeterminePointColor
// ===========================================================================
color_array DeterminePointColor(g3Point const *pnt, bool disableGouraud, bool checkTextureQuality,
                                bool flatColorForNoLight) {
  color_array col;

  if (s_lightState == LS_NONE) {
    if (flatColorForNoLight) {
      col.r = (float)GR_COLOR_RED(s_flatColor) / 255.0f;
      col.g = (float)GR_COLOR_GREEN(s_flatColor) / 255.0f;
      col.b = (float)GR_COLOR_BLUE(s_flatColor) / 255.0f;
    } else {
      col.r = pnt->p3_r;
      col.g = pnt->p3_g;
      col.b = pnt->p3_b;
    }
  } else {
    col.r = pnt->p3_r;
    col.g = pnt->p3_g;
    col.b = pnt->p3_b;
  }
  col.a = pnt->p3_a * s_alphaFactor;
  return col;
}

void gpu_SetMultitextureBlendMode(bool) {}
void gpu_BindTexture(int, int, int) {}
void gpu_RenderPolygon(PosColorUVVertex *vData, uint32_t nv) {
  glBegin(GL_TRIANGLE_FAN);
  for (uint32_t i = 0; i < nv; i++) {
    glColor4f(vData[i].color.r, vData[i].color.g, vData[i].color.b, vData[i].color.a);
    glTexCoord2f(vData[i].uv.s, vData[i].uv.t);
    glVertex3f(vData[i].pos.x(), vData[i].pos.y(), vData[i].pos.z());
  }
  glEnd();
}
void gpu_RenderPolygonUV2(PosColorUV2Vertex *vData, uint32_t nv) {
  glBegin(GL_TRIANGLE_FAN);
  for (uint32_t i = 0; i < nv; i++) {
    glColor4f(vData[i].color.r, vData[i].color.g, vData[i].color.b, vData[i].color.a);
    glTexCoord2f(vData[i].uv0.s, vData[i].uv0.t);
    glVertex3f(vData[i].pos.x(), vData[i].pos.y(), vData[i].pos.z());
  }
  glEnd();
}

void rend_DrawMultitexturePolygon3D(int, g3Point **, int, int) {}

// ===========================================================================
// Polygon -- legacy opengl_DrawPolygon (immediate mode)
// ===========================================================================
static PosColorUVVertex s_vArray[100];

void rend_DrawPolygon3D(int handle, g3Point **p, int nv, int map_type) {
  Q_ASSERT(nv < 100);
  if (nv < 3)
    return;

  if (s_textureQuality == 0) {
    gpu_DrawFlatPolygon3D(p, nv);
    return;
  }

  if (handle < 0) {
    gpu_DrawFlatPolygon3D(p, nv);
    return;
  }

  GLuint tex = uploadTexture(handle);
  glEnable(GL_TEXTURE_2D);

  for (int i = 0; i < nv; i++) {
    g3Point *pnt = p[i];
    s_vArray[i].pos = vector3{pnt->p3_sx, pnt->p3_sy, -(pnt->p3_z * 1.0f)};
    color_array c = DeterminePointColor(pnt, false, false, false);
    s_vArray[i].color = c;
    s_vArray[i].uv.s = pnt->p3_u;
    s_vArray[i].uv.t = pnt->p3_v;
  }
  gpu_RenderPolygon(s_vArray, nv);

  glDisable(GL_TEXTURE_2D);
  glDeleteTextures(1, &tex);
}

void rend_DrawPolygon2D(int handle, g3Point **p, int nv) { rend_DrawPolygon3D(handle, p, nv, MAP_TYPE_BITMAP); }

// ===========================================================================
// g3_DrawPoly / g3_DrawBitmap -- HardwareDraw.cpp
// ===========================================================================
int g3_DrawPoly(int nv, g3Point **pointlist, int bm, int map_type, g3Codes *) {
  rend_DrawPolygon3D(bm, pointlist, nv, map_type);
  return 1;
}

void g3_DrawBitmap(vector3 *pos, float width, float height, int bm, int color) {
  matrix viewOrient;
  g3_GetUnscaledMatrix(&viewOrient);

  float r = 1.0f, g = 1.0f, b = 1.0f;
  if (color != -1) {
    float scale = 1.0f / 255.0f;
    r = GR_COLOR_RED(color) * scale;
    g = GR_COLOR_GREEN(color) * scale;
    b = GR_COLOR_BLUE(color) * scale;
  }

  g3Point corners[4], *pts[4];
  for (int i = 0; i < 4; ++i) {
    pts[i] = &corners[i];
    float cornerScaleU = ((i & 1) ^ ((i & 2) >> 1)) ? 1.0f : -1.0f;
    float cornerScaleV = (i & 2) ? 1.0f : -1.0f;
    vector3 cornerPos = *pos + (viewOrient.uvec * (height * -cornerScaleV)) + (viewOrient.rvec * (width * cornerScaleU));
    corners[i].p3_codes = 0;
    g3_RotatePoint(pts[i], &cornerPos);
    corners[i].p3_flags |= PF_UV;
    corners[i].p3_uvl.u = (cornerScaleU * 0.5f) + 0.5f;
    corners[i].p3_uvl.v = (cornerScaleV * 0.5f) + 0.5f;
    if (color == -1) {
      corners[i].p3_flags |= PF_L;
      corners[i].p3_uvl.l = 1.0f;
    } else {
      corners[i].p3_flags |= PF_RGBA;
      corners[i].p3_uvl.r = r;
      corners[i].p3_uvl.g = g;
      corners[i].p3_uvl.b = b;
    }
    corners[i].p3_uvl.a = 1.0f;
  }
  rend_SetTextureType(TT_LINEAR);
  rend_DrawPolygon3D(bm, pts, 4);
}

void g3_DrawSpecialLine(g3Point *, g3Point *) {}
void g3_DrawLine(ddgr_color, g3Point *, g3Point *) {}
void g3_DrawSphere(ddgr_color, g3Point *, float) {}
void g3_DrawBox(ddgr_color, g3Point *, float) {}
void rend_DrawLine(int, int, int, int) {}
void rend_DrawSpecialLine(g3Point *, g3Point *) {}
void rend_FillCircle(ddgr_color, float, float, float) {}

// ===========================================================================
// Frame handling for the immediate-mode renderer
// ===========================================================================
void rend_StartFrame(int x1, int y1, int x2, int y2, int clear_flags) {
  s_state.clip_x1 = x1;
  s_state.clip_y1 = y1;
  s_state.clip_x2 = x2;
  s_state.clip_y2 = y2;
}

void rend_EndFrame() {
}

void rend_ClearZBuffer() {}
void rend_ClearScreen(ddgr_color) {}
void rend_SetPixel(ddgr_color, int, int) {}
ddgr_color rend_GetPixel(int, int) { return 0; }