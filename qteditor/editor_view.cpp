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

#include "editor_view.h"

#include <QDebug>

#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <GL/gl.h>
#include <QSurfaceFormat>

#include <cmath>
#include <algorithm>

#include "bitmap.h"

#include "chrono_timer.h"
#include "ebnode.h"
#include "d3edit.h"
#include "editor_room_state.h"
#include "gametexture.h"
#include "gamepath.h"
#include "obj_move_manager.h"
#include "object.h"

#include "room.h"
#include "terrain.h"

uint8_t Show_invisible_terrain = 0;
uint8_t Fast_terrain = 1;
float Terrain_texture_distance = DEFAULT_TEXTURE_DISTANCE;



namespace {

// Wireframe color palette matching the Win32 editor (editor/drawworld.cpp).
const float kWfFacingColor[3] = {198.0f / 255, 198.0f / 255, 198.0f / 255};
const float kWfNotFacingColor[3] = {125.0f / 255, 125.0f / 255, 125.0f / 255};
const float kWfSelectedColor[3] = {1.0f, 166.0f / 255, 0.0f};
const float kWfPortalColor[3] = {0.0f, 0.0f, 1.0f}; // TERR_PORTAL_COLOR GR_RGB(0,0,255)
const float kWfCurRoomColor[3] = {1.0f, 1.0f, 1.0f};
const float kWfCurFaceColor[3] = {1.0f, 1.0f, 0.0f};
const float kWfCurEdgeColor[3] = {0.0f, 1.0f, 0.0f};
const float kWfMarkedFaceColor[3] = {0.0f, 1.0f, 1.0f};
const float kWfMarkedEdgeColor[3] = {0.0f, 150.0f / 255, 150.0f / 255};
const float kWfCurPortalColor[3] = {200.0f / 255, 150.0f / 255, 1.0f};
const float kWfFloatTrigColor[3] = {1.0f, 100.0f / 255, 100.0f / 255};
// Object marker colors matching DrawRoomObjects (editor/drawworld.cpp:246-251).
const float kObjRobotColor[3] = {1.0f, 0.0f, 0.0f};
const float kObjPlayerColor[3] = {0.0f, 1.0f, 0.0f};
const float kObjViewerColor[3] = {100.0f / 255, 0.0f, 100.0f / 255};
const float kObjPowerupColor[3] = {0.0f, 0.0f, 1.0f};
const float kObjMiscColor[3] = {0.0f, 100.0f / 255, 100.0f / 255};
const float kObjCameraColor[3] = {1.0f, 1.0f, 0.0f};

// Returns true when `m` is a usable camera basis (a right-handed orthonormal
// rotation).  Degenerate orientation data in a saved level (e.g. level1.d3l's
// shipped viewer has an all-zero matrix) must not be followed as the camera:
// with a zero forward vector every world point projects behind the eye and the
// view renders blank.  updateCamera() falls back to the orbit view instead.
bool isUsableCameraOrient(const matrix &m) {
  const auto mag2 = [](const vector3 &v) {
    return v.x() * v.x() + v.y() * v.y() + v.z() * v.z();
  };
  const float fr = mag2(m.fvec);
  const float ur = mag2(m.uvec);
  const float rr = mag2(m.rvec);
  constexpr float kLo = 0.6f, kHi = 1.4f;
  if (fr < kLo || fr > kHi || ur < kLo || ur > kHi || rr < kLo || rr > kHi)
    return false;
  constexpr float kPerpSlack = 0.3f;
  if (std::fabs(m.fvec.x() * m.uvec.x() + m.fvec.y() * m.uvec.y() + m.fvec.z() * m.uvec.z()) > kPerpSlack)
    return false;
  if (std::fabs(m.fvec.x() * m.rvec.x() + m.fvec.y() * m.rvec.y() + m.fvec.z() * m.rvec.z()) > kPerpSlack)
    return false;
  if (std::fabs(m.rvec.x() * m.uvec.x() + m.rvec.y() * m.uvec.y() + m.rvec.z() * m.uvec.z()) > kPerpSlack)
    return false;
  return true;
}

// Edge types for the deduplication hash table.
enum EdgeType { ET_FACING = 0, ET_NOTFACING = 1, ET_PORTAL = 2, ET_EMPTY = 255 };

struct WireEdge {
  int16_t v0, v1; // normalized so v0 < v1
  uint8_t type;   // EdgeType
  float sx0, sy0, sx1, sy1; // screen coords for both endpoints
};

constexpr int kMaxEdges = 8192;
WireEdge s_edgeTable[kMaxEdges];
int s_nUsed = 0;

void resetEdgeTable() {
  for (int i = 0; i < kMaxEdges; i++)
    s_edgeTable[i].type = ET_EMPTY;
  s_nUsed = 0;
}

void addEdge(int v0, int v1, uint8_t type,
             float sx0, float sy0, float sx1, float sy1) {
  if (v0 > v1) {
    int t = v0; v0 = v1; v1 = t;
    float st = sx0; sx0 = sx1; sx1 = st;
    st = sy0; sy0 = sy1; sy1 = st;
  }
  int key = ((v0 * 7 + v1 * 13) & 0x7FFF) % kMaxEdges;
  for (int probe = 0; probe < kMaxEdges; probe++) {
    int idx = (key + probe) % kMaxEdges;
    if (s_edgeTable[idx].type == ET_EMPTY) {
      s_edgeTable[idx] = {static_cast<int16_t>(v0), static_cast<int16_t>(v1), type,
                          sx0, sy0, sx1, sy1};
      s_nUsed++;
      return;
    }
    if (s_edgeTable[idx].v0 == v0 && s_edgeTable[idx].v1 == v1) {
      if (type < s_edgeTable[idx].type)
        s_edgeTable[idx].type = type;
      return;
    }
  }
}

void drawEdgesFromTable(const float *color, uint8_t type) {
  glColor3fv(color);
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  for (int i = 0; i < kMaxEdges; i++) {
    WireEdge &e = s_edgeTable[i];
    if (e.type == ET_EMPTY || e.type != type)
      continue;
    glVertex2f(e.sx0, e.sy0);
    glVertex2f(e.sx1, e.sy1);
  }
  glEnd();
}

// Draws the vertex marker the Win32 editor uses (DrawVertBox,
// editor/drawworld.cpp:764-776): a diamond cross of four segments.
// CROSS_WIDTH / CROSS_HEIGHT are 8.0, so the half-arm length is 8 px.
void drawVertCross(float sx, float sy, const float *color, float half = 8.0f) {
  glColor3fv(color);
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  glVertex2f(sx - half, sy);
  glVertex2f(sx, sy - half);
  glVertex2f(sx, sy - half);
  glVertex2f(sx + half, sy);
  glVertex2f(sx + half, sy);
  glVertex2f(sx, sy + half);
  glVertex2f(sx, sy + half);
  glVertex2f(sx - half, sy);
  glEnd();
}

// Filled disk marker (the legacy g3_DrawSphere -> rend_FillCircle renders a
// flat projected circle of the object's world radius).
void drawFilledCircle(float cx, float cy, float radius, const float *color) {
  glColor3fv(color);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  constexpr int kSegments = 32;
  for (int i = 0; i <= kSegments; i++) {
    const float a = 2.0f * 3.14159265f * static_cast<float>(i) / kSegments;
    glVertex2f(cx + radius * std::cos(a), cy + radius * std::sin(a));
  }
  glEnd();
}

// ---- Turntable (orbit) camera: port of editor/moveworld.cpp ----
// The Win32 wireframe view is a trackball camera.  The mouse controls are:
//   Ctrl+drag            -> rotate the view around the target
//   Ctrl+Shift+drag      -> pan (move the target)
//   Z+drag               -> zoom (move dist)
//   Z+Shift+drag         -> change the render radius rad
// These all live in editor/moveworld.cpp MoveWorld(); the definitions below
// mirror the constants and helpers exactly (MOVE_SCALE 3.0, ZOOM_SCALE /
// RAD_SCALE 10.0, DEFAULT_VIEW_DIST 500, DEFAULT_VIEW_RAD 5000).

constexpr float kDefaultViewDist = 500.0f; // DEFAULT_VIEW_DIST
constexpr float kDefaultViewRad = 5000.0f; // DEFAULT_VIEW_RAD
constexpr float kMoveScale = 3.0f;         // MOVE_SCALE
constexpr float kZoomScale = 10.0f;        // ZOOM_SCALE
constexpr float kRadScale = 10.0f;         // RAD_SCALE

// Mine_origin (editor/HFile.cpp:402): the fixed world point that
// ResetWireframeView aims the view at (center of the terrain grid).
const vector3 kMineOrigin = {float(TERRAIN_WIDTH * (TERRAIN_SIZE / 2)), -100.0f,
                             float(TERRAIN_DEPTH * (TERRAIN_SIZE / 2))};

// Port of the dist_2d macro (editor/moveworld.cpp:66).
float dist2d(float x, float y) { return std::sqrt(x * x + y * y); }

// Port of GetMouseRotation (editor/moveworld.cpp:68-102): builds the
// incremental trackball rotation matrix for a (dx, dy) mouse motion (idy is
// negated because the Win32 origin is the bottom-left).
void getMouseRotation(int idx, int idy, matrix *rotMat) {
  const float Radius = 100.0f;
  idy *= -1;

  const float dx = static_cast<float>(idx);
  const float dy = static_cast<float>(idy);

  const float dr = dist2d(dx, dy);
  const float denom = dist2d(Radius, dr);

  const float cos_theta = Radius / denom;
  const float sin_theta = dr / denom;
  const float cos_theta1 = 1.0f - cos_theta;

  const float dxdr = dx / dr;
  const float dydr = dy / dr;

  rotMat->rvec.x() = cos_theta + ((dydr * dydr) * cos_theta1);
  rotMat->uvec.x() = -((dxdr * dydr) * cos_theta1);
  rotMat->fvec.x() = (dxdr * sin_theta);

  rotMat->rvec.y() = rotMat->uvec.x();
  rotMat->uvec.y() = cos_theta + ((dxdr * dxdr) * cos_theta1);
  rotMat->fvec.y() = (dydr * sin_theta);

  rotMat->rvec.z() = -rotMat->fvec.x();
  rotMat->uvec.z() = -rotMat->fvec.y();
  rotMat->fvec.z() = cos_theta;
}

} // anonymous namespace

// Vertical field of view in radians (matches the editor's default zoom).
constexpr float kFovY = 0.5445f;

EditorView::EditorView(QWidget *parent) : QOpenGLWidget(parent) {
  QSurfaceFormat fmt;
  fmt.setRenderableType(QSurfaceFormat::OpenGL);
  fmt.setRedBufferSize(8);
  fmt.setGreenBufferSize(8);
  fmt.setBlueBufferSize(8);
  fmt.setDepthBufferSize(24);
  fmt.setSamples(0);
  setFormat(fmt);
  setMinimumSize(320, 240);
  setFocusPolicy(Qt::StrongFocus);
  // The Win32 editor orbits from plain mouse motion with a key held (its
  // OnIdle polls the mouse every frame), so the view must see hover moves.
  setMouseTracking(true);
}

EditorView::~EditorView() {
  if (context() != nullptr) {
    makeCurrent();
    for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
      QOpenGLFunctions(context()).glDeleteTextures(1, &it.value());
    m_textures.clear();
  }
}

void EditorView::requestRedraw() { update(); }

QSize EditorView::renderSize() const { return size(); }

void EditorView::setWireframe(bool wireframe) {
  if (m_wireframe == wireframe)
    return;
  m_wireframe = wireframe;
  update();
}

void EditorView::initializeGL() {
  m_frameCount = 0;
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  f->glCullFace(GL_BACK);
  f->glFrontFace(GL_CCW);
  glClearColor(0.10f, 0.12f, 0.18f, 1.0f);
}

void EditorView::resizeGL(int w, int h) {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f != nullptr)
    glViewport(0, 0, w, h);
  requestRedraw();
}

float EditorView::getFocal() const {
  const float h = height() > 0 ? static_cast<float>(height()) : 480.0f;
  return (h * 0.5f) / std::tan(kFovY * 0.5f);
}

bool EditorView::projectVertex(const vector3 &world, float *sx, float *sy) const {
  float dummy;
  return projectVertexDepth(world, sx, sy, &dummy);
}

bool EditorView::projectVertexDepth(const vector3 &world, float *sx, float *sy, float *depth) const {
  vector3 d = world - m_eye;
  vector3 c;
  c.x() = d.x() * m_orient.rvec.x() + d.y() * m_orient.rvec.y() + d.z() * m_orient.rvec.z();
  c.y() = d.x() * m_orient.uvec.x() + d.y() * m_orient.uvec.y() + d.z() * m_orient.uvec.z();
  c.z() = d.x() * m_orient.fvec.x() + d.y() * m_orient.fvec.y() + d.z() * m_orient.fvec.z();
  if (c.z() < 0.1f)
    return false;

  const float w = width() > 0 ? width() : 1;
  const float h = height() > 0 ? height() : 1;
  const float focal = (h * 0.5f) / std::tan(kFovY * 0.5f);
  *sx = w * 0.5f + (c.x() / c.z()) * focal;
  *sy = h * 0.5f - (c.y() / c.z()) * focal;
  if (depth != nullptr)
    *depth = c.z();
  return true;
}

void EditorView::projectMine(QVector<QVector<ProjectedVertex>> *outFaces) const {
  if (outFaces == nullptr)
    return;
  const_cast<EditorView *>(this)->updateCamera();
  outFaces->clear();

  int projStart = 0;
  int projEnd = Highest_room_index;
  if (Editor_view_mode == VM_ROOM) {
    if (D3EditState.current_room >= 0 && D3EditState.current_room <= Highest_room_index) {
      projStart = D3EditState.current_room;
      projEnd = D3EditState.current_room;
    }
  }
  if (Editor_view_mode == VM_TERRAIN)
    return;

  for (int r = projStart; r <= projEnd; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int f = 0; f < rp->num_faces; f++) {
      face *fp = &rp->faces[f];
      QVector<ProjectedVertex> face;
      bool ok = true;
      for (int v = 0; v < fp->num_verts; v++) {
        const vector3 &world = rp->verts[fp->face_verts[v]];
        float sx, sy;
        if (!projectVertex(world, &sx, &sy)) {
          ok = false;
          break;
        }
        face.append({sx / width(), sy / height(), 0});
      }
      if (ok)
        outFaces->append(face);
    }
  }
}

bool EditorView::projectWorldToScreen(const vector3 &world, float *sx, float *sy, float *depth) const {
  // The camera is kept valid by paint/pick; recompute only when needed so
  // per-vertex calls don't re-scan the whole mine every time.
  if (!m_cameraValid)
    const_cast<EditorView *>(this)->updateCamera();
  return projectVertexDepth(world, sx, sy, depth);
}

void EditorView::ensureTexture(int bmHandle) {
  if (bmHandle < 0 || m_textures.contains(bmHandle))
    return;

  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;
  const int w = bm_w(bmHandle, 0);
  const int h = bm_h(bmHandle, 0);
  if (w <= 0 || h <= 0)
    return;

  QVector<uchar> rgba(w * h * 4);
  const uint16_t *data = bm_data(bmHandle, 0);
  if (data == nullptr) {
    // 8-bit palettized bitmaps are read through bm_data only when the
    // palette is applied; fall back to a magenta placeholder.
    memset(rgba.data(), 0, rgba.size());
    for (int i = 0; i < w * h; i++) {
      rgba[4 * i + 0] = 255; // R
      rgba[4 * i + 1] = 0;   // G
      rgba[4 * i + 2] = 255; // B
      rgba[4 * i + 3] = 255; // A
    }
  } else {
    // D3 bitmaps are 1555: bit 15 = opaque flag, bits 14-10 = R,
    // bits 9-5 = G, bits 4-0 = B.  Expand to 8-bit RGBA to match
    // the legacy renderer's non-packed-pixel path
    // (HardwareOpenGL.cpp opengl_Translate_table).
    for (int i = 0; i < w * h; i++) {
      const uint16_t p = data[i];
      if (!(p & 0x8000)) {
        // Transparent pixel.
        rgba[4 * i + 0] = 0;
        rgba[4 * i + 1] = 0;
        rgba[4 * i + 2] = 0;
        rgba[4 * i + 3] = 0;
      } else {
        const int r5 = (p >> 10) & 0x1F;
        const int g5 = (p >> 5) & 0x1F;
        const int b5 = p & 0x1F;
        rgba[4 * i + 0] = (r5 * 255) / 31; // R
        rgba[4 * i + 1] = (g5 * 255) / 31; // G
        rgba[4 * i + 2] = (b5 * 255) / 31; // B
        rgba[4 * i + 3] = 255;              // A (opaque)
      }
    }
  }

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.constData());
  m_textures.insert(bmHandle, tex);
}

void EditorView::renderRooms() {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;

  // Determine which rooms to render based on view mode.
  if (Editor_view_mode == VM_TERRAIN)
    return;

  int renderStart = 0;
  int renderEnd = Highest_room_index;
  if (Editor_view_mode == VM_ROOM) {
    if (D3EditState.current_room < 0 || D3EditState.current_room > Highest_room_index)
      return;
    if (!Rooms[D3EditState.current_room].used)
      return;
    renderStart = D3EditState.current_room;
    renderEnd = D3EditState.current_room;
  }

  // In the Win32 mine view the terrain dots are drawn before all the rooms
  // (DrawTerrainPoints before DrawAllRooms, editor/drawworld.cpp:811-819),
  // gated on the wireframe (DrawWorld) pass only.
  if (Editor_view_mode != VM_ROOM && m_wireframe && D3EditState.terrain_dots)
    drawTerrainDots();

  // For solid (textured) mode, occlusion is resolved the way the Win32
  // renderer did it: with a Z-buffer rather than a painter's sort.  The GPU
  // depth buffer is used, and GL_DEPTH_TEST (GL_LESS) keeps the nearest
  // fragment per pixel regardless of draw order.  The depth buffer is cleared
  // once per frame and shared across every room here, so the view-distance ->
  // window-z depth scale must be IDENTICAL for all rooms; otherwise a near
  // face in one room would not correctly occlude a far face in another.
  // Precompute the global maximum view distance across the rendered rooms.
  float solidMaxDepth = 10.0f;
  if (!m_wireframe) {
    for (int rr = renderStart; rr <= renderEnd; rr++) {
      room *rpc = &Rooms[rr];
      if (!rpc->used)
        continue;
      for (int i = 0; i < rpc->num_faces; i++) {
        face *fp = &rpc->faces[i];
        int fnv = fp->num_verts;
        if (fnv > 16)
          fnv = 16;
        if (fnv < 3)
          continue;
        for (int v = 0; v < fnv; v++) {
          float tsx, tsy, td;
          if (projectVertexDepth(rpc->verts[fp->face_verts[v]], &tsx, &tsy, &td)) {
            if (td > solidMaxDepth)
              solidMaxDepth = td;
          }
        }
      }
    }
  }

  // ---- Pass 1: render room geometry (textured or wireframe) ----
  // Solid mode resolves occlusion with the shared depth buffer; enable it for
  // the whole room pass so faces in different rooms compare against the same
  // Z-buffer (near faces win with GL_LESS regardless of draw order).
  if (!m_wireframe) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Back faces are culled per-face below (geometric, matching the Win32
    // DoFacingCheck test and pickAtImpl) so the displayed surface matches what
    // picking can select.  GL winding culling is left disabled; the Z-buffer
    // resolves occlusion between the remaining front-facing faces.
    glDisable(GL_CULL_FACE);
  }
  for (int r = renderStart; r <= renderEnd; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;

    if (m_wireframe) {
      // The Win32 wireframe view only draws rooms within the view radius of
      // the target (DrawAllRooms, editor/drawworld.cpp:745-759).
      if (Editor_view_mode != VM_ROOM && rp->num_verts > 0 &&
          vm_VectorDistance(&rp->verts[0], &m_target) >= m_rad)
        continue;

      // Wireframe: use edge deduplication with backface-aware coloring,
      // matching the Win32 editor's drawworld.cpp DrawRoom().
      resetEdgeTable();

      // Build screen-space coords for every vertex in this room.
      const int nv = rp->num_verts;
      QVector<float> sxv(nv), syv(nv);
      for (int v = 0; v < nv; v++) {
        if (!projectVertex(rp->verts[v], &sxv[v], &syv[v])) {
          sxv[v] = syv[v] = -9999.0f;
        }
      }

      // Determine the room's overlay color (selected = orange, else default).
      const bool selected = IsRoomSelected(r);
      const bool isCurRoom = (rp == Curroomp);

      for (int i = 0; i < rp->num_faces; i++) {
        face *fp = &rp->faces[i];
        int fnv = fp->num_verts;
        if (fnv > 16)
          fnv = 16;

        // Skip faces with any behind-camera vertex.
        bool behind = false;
        for (int v = 0; v < fnv; v++) {
          if (sxv[fp->face_verts[v]] < -9000.0f) {
            behind = true;
            break;
          }
        }
        if (behind)
          continue;

        // Floating trigger: draw edges directly in red, including the two
        // diagonals the Win32 editor adds (DrawRoom, drawworld.cpp:618-622).
        if (fp->flags & FF_FLOATING_TRIG) {
          glColor3fv(kWfFloatTrigColor);
          glLineWidth(1.0f);
          glBegin(GL_LINE_LOOP);
          for (int v = 0; v < fnv; v++) {
            int vi = fp->face_verts[v];
            glVertex2f(sxv[vi], syv[vi]);
          }
          glEnd();
          if (fnv >= 3) {
            glBegin(GL_LINES);
            glVertex2f(sxv[fp->face_verts[0]], syv[fp->face_verts[0]]);
            glVertex2f(sxv[fp->face_verts[2]], syv[fp->face_verts[2]]);
            if (fnv >= 4) {
              glVertex2f(sxv[fp->face_verts[1]], syv[fp->face_verts[1]]);
              glVertex2f(sxv[fp->face_verts[3]], syv[fp->face_verts[3]]);
            }
            glEnd();
          }
          continue;
        }

        // Room-to-room portal faces are skipped entirely in the wireframe
        // view (DrawRoom gates on fp->portal_num == -1); only *terrain*
        // portals are drawn, in blue, after all the edges.
        if (fp->portal_num != -1)
          continue;

        // Determine facing as the Win32 editor does: the tested point is the
        // face's first vertex (g3_CheckNormalFacing against verts[0]).
        // The classification is intentionally inverted in the legacy code so
        // facing edges come out dark (first batch) and non-facing light
        // (second batch, on top).
        vector3 toCamera = m_eye - rp->verts[fp->face_verts[0]];
        const bool facing = (fp->normal.x() * toCamera.x() +
                             fp->normal.y() * toCamera.y() +
                             fp->normal.z() * toCamera.z()) > 0.0f;

        uint8_t edgeType = facing ? ET_NOTFACING : ET_FACING;

        for (int v = 0; v < fnv; v++) {
          int v0 = fp->face_verts[v];
          int v1 = fp->face_verts[(v + 1) % fnv];
          addEdge(v0, v1, edgeType, sxv[v0], syv[v0], sxv[v1], syv[v1]);
        }
      }

      // Draw edges: non-facing first (dark), then facing (light).
      drawEdgesFromTable(kWfNotFacingColor, ET_NOTFACING);
      drawEdgesFromTable(kWfFacingColor, ET_FACING);

      // Override all edges with orange if this is a selected room.
      // (Floating triggers and room portals are not re-drawn: triggers keep
      // their red, terrain portals stay blue below.)
      if (selected && !isCurRoom) {
        resetEdgeTable();
        for (int i = 0; i < rp->num_faces; i++) {
          face *fp = &rp->faces[i];
          if ((fp->flags & FF_FLOATING_TRIG) || fp->portal_num != -1)
            continue;
          int fnv = fp->num_verts;
          if (fnv > 16)
            fnv = 16;
          for (int v = 0; v < fnv; v++) {
            int v0 = fp->face_verts[v];
            int v1 = fp->face_verts[(v + 1) % fnv];
            if (sxv[v0] > -9000.0f && sxv[v1] > -9000.0f)
              addEdge(v0, v1, ET_FACING, sxv[v0], syv[v0], sxv[v1], syv[v1]);
          }
        }
        drawEdgesFromTable(kWfSelectedColor, ET_FACING);
      }

      // Now draw the terrain portals in blue (DrawRoom, drawworld.cpp:638-646).
      for (int p = 0; p < rp->num_portals; p++) {
        portal *pp = &rp->portals[p];
        if (pp->croom != -1)
          continue;
        face *fp = &rp->faces[pp->portal_face];
        int pnv = fp->num_verts;
        if (pnv > 16)
          pnv = 16;
        bool behind = false;
        for (int v = 0; v < pnv; v++) {
          if (sxv[fp->face_verts[v]] < -9000.0f) {
            behind = true;
            break;
          }
        }
        if (behind)
          continue;
        glColor3fv(kWfPortalColor);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        for (int v = 0; v < pnv; v++) {
          int vi = fp->face_verts[v];
          glVertex2f(sxv[vi], syv[vi]);
        }
        glEnd();
      }
    } else {
      // Textured / solid mode.
      for (int i = 0; i < rp->num_faces; i++) {
        face *fp = &rp->faces[i];

        float sx[16], sy[16], d[16];
        int nv = fp->num_verts;
        if (nv > 16)
          nv = 16;
        bool behind = false;
        for (int v = 0; v < nv; v++) {
          if (!projectVertexDepth(rp->verts[fp->face_verts[v]], &sx[v], &sy[v], &d[v])) {
            behind = true;
            break;
          }
        }
        if (behind)
          continue;

        // Front-face cull matching Win32 DoFacingCheck (and pickAtImpl): only
        // front-facing faces are drawn, so the displayed surface agrees with
        // what picking can select.  Uses D3's row-vector view space
        // (p3_vec = d * View_matrix): dot((v1-v0)x(v2-v0), v1) < 0 => front.
        {
          auto toViewScratch = [this](const vector3 &w, vector3 &c) {
            const vector3 d = w - m_eye;
            c = vector3{d.x() * m_orient.rvec.x() + d.y() * m_orient.uvec.x() +
                            d.z() * m_orient.fvec.x(),
                        d.x() * m_orient.rvec.y() + d.y() * m_orient.uvec.y() +
                            d.z() * m_orient.fvec.y(),
                        d.x() * m_orient.rvec.z() + d.y() * m_orient.uvec.z() +
                            d.z() * m_orient.fvec.z()};
          };
          vector3 v0, v1, v2;
          toViewScratch(rp->verts[fp->face_verts[0]], v0);
          toViewScratch(rp->verts[fp->face_verts[1]], v1);
          toViewScratch(rp->verts[fp->face_verts[2]], v2);
          const vector3 n = vm_Cross3Product(v1 - v0, v2 - v0);
          if (!(vm_Dot3Product(n, v1) < 0.0f))
            continue; // back-facing -> not drawn (matches pick)
        }

        // Face shading from a fixed world light direction.
        vector3 ld{0.4f, 0.7f, 0.6f};
        float len = vm_NormalizeVector(&ld);
        if (len < 0.001f)
          ld = vector3{0, 1, 0};
        float diff = (fp->normal.x() * ld.x() + fp->normal.y() * ld.y() + fp->normal.z() * ld.z());
        float shade = 0.35f + 0.65f * (diff < 0 ? -diff : diff);
        if (shade > 1.0f)
          shade = 1.0f;

        // Pack each vertex's view distance into window z using the GLOBAL
        // depth scale so this face agrees with faces in every other room.
        // Near -> z=-1 (smallest depth value), far -> z=+1.
        static const float kDepthNear = 0.1f;
        const float zScale = 2.0f / (solidMaxDepth - kDepthNear);
        const float zBase = -1.0f - kDepthNear * zScale;
        for (int v = 0; v < nv; v++)
          d[v] = zBase + d[v] * zScale;

        // Texture the face if possible.
        int bm = -1;
        if (fp->tmap >= 0 && fp->tmap < MAX_TEXTURES && GameTextures[fp->tmap].used)
          bm = GameTextures[fp->tmap].bm_handle;
        if (bm >= 0) {
          ensureTexture(bm);
          auto it = m_textures.constFind(bm);
          if (it != m_textures.constEnd()) {
            const int tw = bm_w(bm, 0);
            const int th = bm_h(bm, 0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, it.value());
            glColor3f(shade, shade, shade);
            glBegin(GL_TRIANGLE_FAN);
            for (int v = 0; v < nv; v++) {
              float u = 0, vv = 0;
              if (fp->face_uvls != nullptr && tw > 0 && th > 0) {
                u = fp->face_uvls[v].u / tw;
                vv = fp->face_uvls[v].v / th;
              }
              glTexCoord2f(u, vv);
              glVertex3f(sx[v], sy[v], d[v]);
            }
            glEnd();
            glDisable(GL_TEXTURE_2D);
            continue;
          }
        }
        // No texture: flat shade.
        glColor3f(shade * 0.7f, shade * 0.75f, shade * 0.85f);
        glBegin(GL_TRIANGLE_FAN);
        for (int v = 0; v < nv; v++)
          glVertex3f(sx[v], sy[v], d[v]);
        glEnd();
      }
    }
  }
}

void EditorView::renderOverlays() {
  if (Editor_view_mode == VM_TERRAIN)
    return;
  // ---- Pass 2: selection highlights (always on top) ----
  // In VM_ROOM the Win32 wireframe view draws only the palette room plus its
  // yellow current face; there are no current-room / marked-room overlays
  // (DrawWorld VM_ROOM branch, editor/drawworld.cpp:800-810).  The Qt port
  // keeps the current-face selection in Curface.
  if (Editor_view_mode == VM_ROOM) {
    // (The yellow current-face highlight is drawn below — the Win32 room view
    // has no current-room or marked-room overlays.)
    if (Curroomp != nullptr && Curroomp->used &&
        Curroomp == &Rooms[D3EditState.current_room] && Curface >= 0 &&
        Curface < Curroomp->num_faces) {
      face *fp = &Curroomp->faces[Curface];
      float sx[16], sy[16];
      int nv = fp->num_verts;
      if (nv > 16)
        nv = 16;
      bool ok = true;
      for (int v = 0; v < nv; v++) {
        if (!projectVertex(Curroomp->verts[fp->face_verts[v]], &sx[v], &sy[v])) {
          ok = false;
          break;
        }
      }
      if (ok) {
        glColor3fv(kWfCurFaceColor);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int v = 0; v < nv; v++)
          glVertex2f(sx[v], sy[v]);
        glEnd();
        glLineWidth(1.0f);
      }
    }
    return;
  }

  // Marked room/face/edge/vert in the Win32 order: the marked elements are
  // drawn BEFORE the current room so the white current-room wireframe draws
  // over them where they overlap (DrawWorld, editor/drawworld.cpp:851-863).
  if (Markedroomp != nullptr && Markedroomp->used) {
    if (Markedface >= 0 && Markedface < Markedroomp->num_faces) {
      face *fp = &Markedroomp->faces[Markedface];
      float sx[16], sy[16];
      int nv = fp->num_verts;
      if (nv > 16)
        nv = 16;
      bool ok = true;
      for (int v = 0; v < nv; v++) {
        if (!projectVertex(Markedroomp->verts[fp->face_verts[v]], &sx[v], &sy[v])) {
          ok = false;
          break;
        }
      }
      if (ok) {
        // Marked face in cyan.
        glColor3fv(kWfMarkedFaceColor);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int v = 0; v < nv; v++)
          glVertex2f(sx[v], sy[v]);
        glEnd();
        glLineWidth(1.0f);

        // Marked edge in teal.
        if (Markededge >= 0 && Markededge < nv) {
          int next = (Markededge + 1) % nv;
          glColor3fv(kWfMarkedEdgeColor);
          glLineWidth(1.0f);
          glBegin(GL_LINES);
          glVertex2f(sx[Markededge], sy[Markededge]);
          glVertex2f(sx[next], sy[next]);
          glEnd();
        }

        // Marked vertex cross in teal.
        if (Markedvert >= 0 && Markedvert < fp->num_verts) {
          int vi = fp->face_verts[Markedvert];
          float vx, vy;
          if (projectVertex(Markedroomp->verts[vi], &vx, &vy))
            drawVertCross(vx, vy, kWfMarkedEdgeColor);
        }
      }
    }
  }

  if (Curroomp != nullptr && Curroomp->used) {
    // Current room wireframe in white (DrawRoom(Curroomp, CURROOM_COLOR)).
    // Like the legacy DrawRoom edge table, floating-trigger faces (drawn
    // red) and room portal faces (terrain portals drawn blue) are skipped so
    // the white override does not cover them.
    glColor3fv(kWfCurRoomColor);
    glLineWidth(1.0f);
    for (int i = 0; i < Curroomp->num_faces; i++) {
      face *fp = &Curroomp->faces[i];
      if ((fp->flags & FF_FLOATING_TRIG) || fp->portal_num != -1)
        continue;
      float sx[16], sy[16];
      int nv = fp->num_verts;
      if (nv > 16)
        nv = 16;
      bool ok = true;
      for (int v = 0; v < nv; v++) {
        if (!projectVertex(Curroomp->verts[fp->face_verts[v]], &sx[v], &sy[v])) {
          ok = false;
          break;
        }
      }
      if (!ok)
        continue;
      glBegin(GL_LINE_LOOP);
      for (int v = 0; v < nv; v++)
        glVertex2f(sx[v], sy[v]);
      glEnd();
    }

    // Current portal face in purple (DrawRoomFace, CURPORTAL_COLOR).
    if (Curportal >= 0 && Curportal < Curroomp->num_portals) {
      int faceIdx = Curroomp->portals[Curportal].portal_face;
      if (faceIdx >= 0 && faceIdx < Curroomp->num_faces) {
        face *fp = &Curroomp->faces[faceIdx];
        float sx[16], sy[16];
        int nv = fp->num_verts;
        if (nv > 16)
          nv = 16;
        bool ok = true;
        for (int v = 0; v < nv; v++) {
          if (!projectVertex(Curroomp->verts[fp->face_verts[v]], &sx[v], &sy[v])) {
            ok = false;
            break;
          }
        }
        if (ok) {
          glColor3fv(kWfCurPortalColor);
          glLineWidth(2.0f);
          glBegin(GL_LINE_LOOP);
          for (int v = 0; v < nv; v++)
            glVertex2f(sx[v], sy[v]);
          glEnd();
          glLineWidth(1.0f);
        }
      }
    }

    // Current face in yellow.
    if (Curface >= 0 && Curface < Curroomp->num_faces) {
      face *fp = &Curroomp->faces[Curface];
      float sx[16], sy[16];
      int nv = fp->num_verts;
      if (nv > 16)
        nv = 16;
      bool ok = true;
      for (int v = 0; v < nv; v++) {
        if (!projectVertex(Curroomp->verts[fp->face_verts[v]], &sx[v], &sy[v])) {
          ok = false;
          break;
        }
      }
      if (ok) {
        glColor3fv(kWfCurFaceColor);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int v = 0; v < nv; v++)
          glVertex2f(sx[v], sy[v]);
        glEnd();

        // Current edge in green (DrawFaceEdge, CUREDGE_COLOR).
        if (Curedge >= 0 && Curedge < nv) {
          int next = (Curedge + 1) % nv;
          glColor3fv(kWfCurEdgeColor);
          glLineWidth(3.0f);
          glBegin(GL_LINES);
          glVertex2f(sx[Curedge], sy[Curedge]);
          glVertex2f(sx[next], sy[next]);
          glEnd();
        }

        // Current vertex cross in green (DrawVertBox at
        // faces[Curface].face_verts[Curvert], CUREDGE_COLOR).
        if (Curvert >= 0 && Curvert < fp->num_verts) {
          int vi = fp->face_verts[Curvert];
          float vx, vy;
          if (projectVertex(Curroomp->verts[vi], &vx, &vy))
            drawVertCross(vx, vy, kWfCurEdgeColor);
        }

        glLineWidth(1.0f);
      }
    }
  }
}

// Draws a white pixel at every terrain grid node, as the Win32 mine-wireframe
// view does (DrawTerrainPoints, editor/drawworld.cpp:279-294).  The grid has
// TERRAIN_WIDTH * TERRAIN_DEPTH nodes stored compactly; each node sits at
// (x*TERRAIN_SIZE, Terrain_seg[i].y, z*TERRAIN_SIZE).
void EditorView::drawTerrainDots() {
  glPointSize(1.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_POINTS);
  for (int i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    const int x = i % TERRAIN_WIDTH;
    const int z = i / TERRAIN_WIDTH;
    const vector3 wp{static_cast<float>(x * TERRAIN_SIZE), Terrain_seg[i].y,
                     static_cast<float>(z * TERRAIN_SIZE)};
    float sx, sy;
    if (projectVertex(wp, &sx, &sy))
      glVertex2f(sx, sy);
  }
  glEnd();
}

void EditorView::renderTerrain() {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;

  // Compute visible cell range based on camera position.
  const int camCellX = static_cast<int>(std::round(m_eye.x() / TERRAIN_SIZE));
  const int camCellZ = static_cast<int>(std::round(m_eye.z() / TERRAIN_SIZE));

  constexpr int kViewRadius = 48;
  const int startX = std::max(0, camCellX - kViewRadius);
  const int endX = std::min(TERRAIN_WIDTH - 1, camCellX + kViewRadius);
  const int startZ = std::max(0, camCellZ - kViewRadius);
  const int endZ = std::min(TERRAIN_DEPTH - 1, camCellZ + kViewRadius);

  for (int z = startZ; z <= endZ; z++) {
    for (int x = startX; x <= endX; x++) {
      // Terrain_seg is stored in a compact z*TERRAIN_WIDTH + x grid (see
      // mini/game/terrain.cpp ResetTerrain); the extra room in the declared
      // (TERRAIN_WIDTH+1)*(TERRAIN_DEPTH+1) array only absorbs edge reads,
      // so the +1 stride here would shift every row after the first.
      const int idx00 = z * TERRAIN_WIDTH + x;
      const int idx10 = z * TERRAIN_WIDTH + (x + 1);
      const int idx01 = (z + 1) * TERRAIN_WIDTH + x;
      const int idx11 = (z + 1) * TERRAIN_WIDTH + (x + 1);

      const vector3 v00{float(x * TERRAIN_SIZE), Terrain_seg[idx00].y,
                       float(z * TERRAIN_SIZE)};
      const vector3 v10{float((x + 1) * TERRAIN_SIZE), Terrain_seg[idx10].y,
                       float(z * TERRAIN_SIZE)};
      const vector3 v01{float(x * TERRAIN_SIZE), Terrain_seg[idx01].y,
                       float((z + 1) * TERRAIN_SIZE)};
      const vector3 v11{float((x + 1) * TERRAIN_SIZE), Terrain_seg[idx11].y,
                       float((z + 1) * TERRAIN_SIZE)};

      float sx[4], sy[4];
      if (!projectVertex(v00, &sx[0], &sy[0]))
        continue;
      if (!projectVertex(v10, &sx[1], &sy[1]))
        continue;
      if (!projectVertex(v01, &sx[2], &sy[2]))
        continue;
      if (!projectVertex(v11, &sx[3], &sy[3]))
        continue;

      if (m_wireframe) {
        const float height = Terrain_seg[idx00].y / MAX_TERRAIN_HEIGHT;
        glColor3f(0.3f + height * 0.5f, 0.5f + height * 0.4f,
                  0.3f + height * 0.5f);
        glBegin(GL_LINES);
        // Cell edges.
        glVertex2f(sx[0], sy[0]);
        glVertex2f(sx[1], sy[1]);
        glVertex2f(sx[1], sy[1]);
        glVertex2f(sx[3], sy[3]);
        glVertex2f(sx[3], sy[3]);
        glVertex2f(sx[2], sy[2]);
        glVertex2f(sx[2], sy[2]);
        glVertex2f(sx[0], sy[0]);
        // Diagonals.
        glVertex2f(sx[0], sy[0]);
        glVertex2f(sx[3], sy[3]);
        glVertex2f(sx[1], sy[1]);
        glVertex2f(sx[2], sy[2]);
        glEnd();
        continue;
      }

      // Textured / flat-shaded mode.
      const int texsegIdx = Terrain_seg[idx00].texseg_index;
      int bm = -1;
      if (texsegIdx >= 0 && texsegIdx < TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH) {
        const int texIdx = Terrain_tex_seg[texsegIdx].tex_index;
        if (texIdx >= 0 && texIdx < MAX_TEXTURES && GameTextures[texIdx].used)
          bm = GameTextures[texIdx].bm_handle;
      }

      // Cell lighting.
      const float light =
          Terrain_seg[idx00].l / 255.0f;
      const float shade = 0.35f + 0.65f * (light < 0 ? -light : light);

      if (bm >= 0) {
        ensureTexture(bm);
        auto it = m_textures.constFind(bm);
        if (it != m_textures.constEnd()) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, it.value());
          glColor3f(shade, shade, shade);

          // Triangle 1: top-left, top-right, bottom-left.
          glBegin(GL_TRIANGLES);
          glTexCoord2f(0.0f, 0.0f);
          glVertex2f(sx[0], sy[0]);
          glTexCoord2f(1.0f, 0.0f);
          glVertex2f(sx[1], sy[1]);
          glTexCoord2f(0.0f, 1.0f);
          glVertex2f(sx[2], sy[2]);
          glEnd();

          // Triangle 2: top-right, bottom-right, bottom-left.
          glBegin(GL_TRIANGLES);
          glTexCoord2f(1.0f, 0.0f);
          glVertex2f(sx[1], sy[1]);
          glTexCoord2f(1.0f, 1.0f);
          glVertex2f(sx[3], sy[3]);
          glTexCoord2f(0.0f, 1.0f);
          glVertex2f(sx[2], sy[2]);
          glEnd();

          glDisable(GL_TEXTURE_2D);
          continue;
        }
      }

      // No texture: flat shade with a terrain-tinted green.
      glColor3f(shade * 0.4f, shade * 0.7f, shade * 0.4f);
      glBegin(GL_TRIANGLES);
      glVertex2f(sx[0], sy[0]);
      glVertex2f(sx[1], sy[1]);
      glVertex2f(sx[2], sy[2]);
      glEnd();
      glBegin(GL_TRIANGLES);
      glVertex2f(sx[1], sy[1]);
      glVertex2f(sx[3], sy[3]);
      glVertex2f(sx[2], sy[2]);
      glEnd();
    }
  }
  if (!m_wireframe)
    glDisable(GL_DEPTH_TEST);
}

// Draws the objects in each rendered room as filled disks, matching the
// Win32 DrawRoomObjects (editor/drawworld.cpp:705-743): gated by
// D3EditState.objects_in_wireframe, doors are skipped, and each object is a
// filled circle whose screen radius is size * focal / depth.  In VM_ROOM the
// legacy room view draws no objects (DrawWorld VM_ROOM branch).
void EditorView::renderObjects() {
  if (Editor_view_mode == VM_TERRAIN || Editor_view_mode == VM_ROOM)
    return;
  if (!D3EditState.objects_in_wireframe)
    return;

  const float w = width() > 0 ? static_cast<float>(width()) : 640.0f;
  const float h = height() > 0 ? static_cast<float>(height()) : 480.0f;
  const float halfFovY = kFovY * 0.5f;
  const float halfFovX = std::atan(std::tan(halfFovY) * (w / h));
  const float focalX = (w * 0.5f) / std::tan(halfFovX);

  for (int i = 0; i <= Highest_object_index; i++) {
    object *obj = &Objects[i];
    if (obj->type == OBJ_NONE || obj->type == OBJ_DOOR)
      continue;

    // A room is only drawn when its verts[0] is within the render radius
    // (DrawAllRooms); objects live in rooms and follow the same gate.
    if (obj->roomnum < 0 || obj->roomnum > Highest_room_index)
      continue;
    room *rp = &Rooms[obj->roomnum];
    if (!rp->used || rp->num_verts == 0)
      continue;
    if (vm_VectorDistance(&rp->verts[0], &m_target) >= m_rad)
      continue;

    // Un-clipped projection like the legacy editor: draw even behind camera.
    vector3 d = obj->pos - m_eye;
    const float cz = d.x() * m_orient.fvec.x() + d.y() * m_orient.fvec.y() + d.z() * m_orient.fvec.z();
    if (cz > -0.001f && cz < 0.001f)
      continue;
    const float cx =
        d.x() * m_orient.rvec.x() + d.y() * m_orient.rvec.y() + d.z() * m_orient.rvec.z();
    const float cy = d.x() * m_orient.uvec.x() + d.y() * m_orient.uvec.y() + d.z() * m_orient.uvec.z();
    const float sx = w * 0.5f + (cx / cz) * getFocal();
    const float sy = h * 0.5f - (cy / cz) * getFocal();

    // Color by object type (DrawRoomObjects switch).  Doors were skipped above.
    switch (obj->type) {
    case OBJ_PLAYER:
      glColor3f(0.0f, 1.0f, 0.0f); // PLAYER_COLOR GR_RGB(0,255,0)
      break;
    case OBJ_ROBOT:
      glColor3f(1.0f, 0.0f, 0.0f); // ROBOT_COLOR GR_RGB(255,0,0)
      break;
    case OBJ_POWERUP:
      glColor3f(0.0f, 0.0f, 1.0f); // POWERUP_COLOR GR_RGB(0,0,255)
      break;
    case OBJ_VIEWER:
      glColor3f(100.0f / 255, 0.0f, 100.0f / 255); // VIEWER_COLOR GR_RGB(100,0,100)
      break;
    case OBJ_CAMERA:
      glColor3f(1.0f, 1.0f, 0.0f); // CAMERA_COLOR GR_RGB(255,255,0)
      break;
    default:
      glColor3f(0.0f, 100.0f / 255, 100.0f / 255); // MISCOBJ_COLOR GR_RGB(0,100,100)
      break;
    }

    // g3_DrawSphere: filled disk of radius obj->size * focal / depth.
    const float radius = (obj->size * focalX) / cz;
    constexpr int kSegments = 32;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sx, sy);
    for (int k = 0; k <= kSegments; k++) {
      const float a = (float)k / kSegments * 6.2831853f;
      glVertex2f(sx + std::cos(a) * radius, sy + std::sin(a) * radius);
    }
    glEnd();
  }
}

extern uint8_t Show_paths;
int GetFirstPath();
int GetNextPath(int n);

void EditorView::renderPaths() {
  if (!Show_paths)
    return;
  if (Editor_view_mode == VM_TERRAIN)
    return;

  glLineWidth(1.5f);

  int current_path_index = GetFirstPath();
  if (current_path_index < 0)
    return;

  const float h = height() > 0 ? static_cast<float>(height()) : 1.0f;
  const float focal = (h * 0.5f) / std::tan(kFovY * 0.5f);

  for (int i = 0; i < Num_game_paths; i++, current_path_index = GetNextPath(current_path_index)) {
    game_path *gp = &GamePaths[current_path_index];
    if (!gp->used || gp->num_nodes == 0)
      continue;

    bool isCurrent = (current_path_index == D3EditState.current_path);

    for (int t = 0; t < gp->num_nodes - 1; t++) {
      float ax, ay, az, bx, by, bz;
      if (!projectVertexDepth(gp->pathnodes[t].pos, &ax, &ay, &az))
        continue;
      if (!projectVertexDepth(gp->pathnodes[t + 1].pos, &bx, &by, &bz))
        continue;

      glColor3f(0.14f, 0.39f, 0.93f);
      if (isCurrent)
        glColor3f(1.0f, 1.0f, 1.0f);
      glBegin(GL_LINES);
      glVertex2f(ax, ay);
      glVertex2f(bx, by);
      glEnd();
    }

    for (int t = 0; t < gp->num_nodes; t++) {
      float nx, ny, nz;
      if (!projectVertexDepth(gp->pathnodes[t].pos, &nx, &ny, &nz))
        continue;

      float size = (focal * 1.2f) / nz;
      if (size < 3.0f)
        size = 3.0f;

      bool isNodeCurrent = isCurrent && (t == D3EditState.current_node);
      if (isNodeCurrent)
        glColor3f(0.39f, 0.50f, 1.0f);
      else
        glColor3f(0.0f, 0.50f, 0.63f);

      float hs = size * 0.5f;
      glBegin(GL_LINE_LOOP);
      glVertex2f(nx, ny - hs);
      glVertex2f(nx + hs, ny);
      glVertex2f(nx, ny + hs);
      glVertex2f(nx - hs, ny);
      glEnd();

      float fex = gp->pathnodes[t].pos.x() + gp->pathnodes[t].fvec.x() * (size * 0.4f);
      float fey = gp->pathnodes[t].pos.y() + gp->pathnodes[t].fvec.y() * (size * 0.4f);
      float fez = gp->pathnodes[t].pos.z() + gp->pathnodes[t].fvec.z() * (size * 0.4f);
      vector3 fepos{fex, fey, fez};
      float fx, fy, fdep;
      if (projectVertexDepth(fepos, &fx, &fy, &fdep)) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(nx, ny);
        glVertex2f(fx, fy);
        glEnd();
      }

      float uex = gp->pathnodes[t].pos.x() + gp->pathnodes[t].uvec.x() * (size * 0.4f);
      float uey = gp->pathnodes[t].pos.y() + gp->pathnodes[t].uvec.y() * (size * 0.4f);
      float uez = gp->pathnodes[t].pos.z() + gp->pathnodes[t].uvec.z() * (size * 0.4f);
      vector3 uepos{uex, uey, uez};
      float ux, uy, udep;
      if (projectVertexDepth(uepos, &ux, &uy, &udep)) {
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(nx, ny);
        glVertex2f(ux, uy);
        glEnd();
      }
    }
  }

  glLineWidth(1.0f);
}

extern char EBN_draw_type;

void EditorView::renderBNodes() {
  if (EBN_draw_type == EBDRAW_NONE)
    return;
  if (Editor_view_mode == VM_TERRAIN)
    return;

  glLineWidth(1.5f);

  const float h = height() > 0 ? static_cast<float>(height()) : 1.0f;
  const float focal = (h * 0.5f) / std::tan(kFovY * 0.5f);

  int room_start = 0;
  int room_end = Highest_room_index;

  if (EBN_draw_type == EBDRAW_ROOM || EBDRAW_ROOM_AND_NEXT_ROOMS) {
    if (Viewer_object != nullptr)
      room_start = room_end = Viewer_object->roomnum;
  }

  for (int roomnum = room_start; roomnum <= room_end; roomnum++) {
    if (!Rooms[roomnum].used || (Rooms[roomnum].flags.external))
      continue;
    if (EBN_draw_type == EBDRAW_ROOM && roomnum != room_start)
      continue;

    bn_list *nlist = BNode_GetBNListPtr(roomnum);
    if (!nlist || nlist->num_nodes == 0)
      continue;

    bool is_current_room = (Viewer_object != nullptr && roomnum == Viewer_object->roomnum);

    for (int i = 0; i < nlist->num_nodes; i++) {
      for (int e = 0; e < nlist->nodes[i].num_edges; e++) {
        int eroom = nlist->nodes[i].edges[e].end_room;
        int eidx = nlist->nodes[i].edges[e].end_index;
        bn_list *enlist = BNode_GetBNListPtr(eroom);
        if (!enlist)
          continue;
        if (eidx < 0 || eidx >= enlist->num_nodes)
          continue;

        float ax, ay, az, bx, by, bz;
        if (!projectVertexDepth(nlist->nodes[i].pos, &ax, &ay, &az))
          continue;
        if (!projectVertexDepth(enlist->nodes[eidx].pos, &bx, &by, &bz))
          continue;

        if (is_current_room)
          glColor3f(1.0f, 0.0f, 0.0f); // EBNode GR_RGB(255,0,0) current room
        else
          glColor3f(100.0f / 255, 0.0f, 0.0f); // GR_RGB(100,0,0) other rooms
        glBegin(GL_LINES);
        glVertex2f(ax, ay);
        glVertex2f(bx, by);
        glEnd();
      }
    }

    for (int i = 0; i < nlist->num_nodes; i++) {
      float nx, ny, nz;
      if (!projectVertexDepth(nlist->nodes[i].pos, &nx, &ny, &nz))
        continue;

      float size = (focal * 0.7f) / nz;
      if (size < 2.0f)
        size = 2.0f;

      if (is_current_room)
        glColor3f(0.0f, 0.5f, 0.63f);
      else
        glColor3f(0.0f, 0.25f, 0.31f);

      float hs = size * 0.5f;
      glBegin(GL_LINE_LOOP);
      glVertex2f(nx, ny - hs);
      glVertex2f(nx + hs, ny);
      glVertex2f(nx, ny + hs);
      glVertex2f(nx - hs, ny);
      glEnd();
    }
  }

  glLineWidth(1.0f);
}

EditorView::WireframeViewState *EditorView::activeView() {
  return Editor_view_mode == VM_ROOM ? &m_viewRoom : &m_viewMine;
}

const EditorView::WireframeViewState *EditorView::activeView() const {
  return Editor_view_mode == VM_ROOM ? &m_viewRoom : &m_viewMine;
}

const EditorView::WireframeViewState &EditorView::activeWireframeView() const {
  return *activeView();
}

void EditorView::updateCamera() {
  // The camera is derived from the active wireframe view state (Win32
  // Wireframe_view).  m_target / m_dist / m_rad are the mirrors the projection
  // and render code (and the pick radius gate) read, so they must stay in sync
  // with the orbit state even while the eye/orient follow a Viewer_object.
  const WireframeViewState *v = activeView();
  m_target = v->target;
  m_dist = v->dist;
  m_rad = v->rad;

  m_cameraValid = false;
  if (Viewer_object != nullptr && isUsableCameraOrient(Viewer_object->orient)) {
    m_eye = Viewer_object->pos;
    m_orient = Viewer_object->orient;
    m_cameraValid = true;
    return;
  }

  // Turntable camera: the eye orbits on the sphere of radius dist around the
  // target, so the center of rotation stays wherever the view is aimed
  // (reset: Mine_origin; pan: moved target; SetWireframeView: room/object).
  m_orient = v->orient;
  m_eye = m_target - m_orient.fvec * m_dist;
  m_cameraValid = true;
}

void EditorView::syncViewerToCamera() {
  if (Viewer_object == nullptr)
    return;
  const WireframeViewState *v = activeView();
  Viewer_object->orient = v->orient;
  Viewer_object->pos = v->target - v->orient.fvec * v->dist;
}

void EditorView::resetCamera() { resetWireframeView(); }

void EditorView::resetWireframeView() {
  // Port of ResetWireframeView (editor/moveworld.cpp:164-172).
  WireframeViewState *v = activeView();
  v->dist = kDefaultViewDist;
  v->rad = kDefaultViewRad;
  vm_MakeIdentity(&v->orient);
  v->target = kMineOrigin;
  syncViewerToCamera();
  m_cameraValid = false;
  update();
}

void EditorView::resetWireframeViewRad() {
  // Port of ResetWireframeViewRad (editor/moveworld.cpp:175-178).
  activeView()->rad = kDefaultViewRad;
  m_cameraValid = false;
  update();
}

void EditorView::setWireframeView(const vector3 &pos) {
  // Port of SetWireframeView (editor/moveworld.cpp:182-185): re-aim the active
  // view at a new location without touching distance or orientation.
  activeView()->target = pos;
  syncViewerToCamera();
  m_cameraValid = false;
  update();
}

void EditorView::moveWorld(int dx, int dy, bool ctrlDown, bool shiftDown, bool zDown) {
  // Port of MoveWorld (editor/moveworld.cpp:120-158).  The mouse control keys
  // are mutually exclusive in the Win32 function (rotate / pan / zoom / rad),
  // so the order here matches the original if/if/if/if structure.  The Win32
  // editor performs no mouse-drag selection while these run, so the Qt port
  // applies the same camera motion on plain mouse movement over the view.
  if (dx == 0 && dy == 0)
    return;
  if (Editor_view_mode == VM_TERRAIN)
    return;

  WireframeViewState *v = activeView();

  // Ctrl+drag: rotate the view around its target.
  if (ctrlDown && !shiftDown) {
    matrix rotmat, tempm;
    getMouseRotation(dx, dy, &rotmat);
    tempm = v->orient * rotmat;
    v->orient = tempm;
    m_cameraValid = false;
  }

  // Ctrl+Shift+drag: pan the target along the view's right/up axes.
  if (ctrlDown && shiftDown) {
    v->target += v->orient.rvec * -dx * kMoveScale;
    v->target += v->orient.uvec * dy * kMoveScale;
    m_cameraValid = false;
  }

  // Z+drag: zoom by moving the eye along the target->eye ray.
  if (zDown && !shiftDown) {
    v->dist += dy * kZoomScale;
    if (v->dist < 0)
      v->dist = 0;
    m_cameraValid = false;
  }

  // Z+Shift+drag: change the wireframe render radius.
  if (zDown && shiftDown) {
    v->rad += dy * kRadScale;
    if (v->rad < 0)
      v->rad = 0;
    m_cameraValid = false;
  }

  syncViewerToCamera();
}

void EditorView::setOrbitCamera(float yawDeg, float pitchDeg, float dist) {
  WireframeViewState *v = activeView();
  // Same angle mapping updateCamera used to apply: p=0, h=yaw, b=pitch.
  vm_AnglesToMatrix(&v->orient, 0, yawDeg * 65536.0f / 360.0f, pitchDeg * 65536.0f / 360.0f);
  if (dist > 0.0f)
    v->dist = dist;
  syncViewerToCamera();
  m_cameraValid = false;
  update();
}

void EditorView::setPickRadius(float radius) {
  activeView()->rad = radius;
  m_cameraValid = false;
}

void EditorView::fitToMine() {
  vector3 mn{1e30f, 1e30f, 1e30f}, mx{-1e30f, -1e30f, -1e30f};
  bool any = false;
  for (int r = 0; r <= Highest_room_index; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int v = 0; v < rp->num_verts; v++) {
      const vector3 &p = rp->verts[v];
      mn.x() = std::min(mn.x(), p.x());
      mn.y() = std::min(mn.y(), p.y());
      mn.z() = std::min(mn.z(), p.z());
      mx.x() = std::max(mx.x(), p.x());
      mx.y() = std::max(mx.y(), p.y());
      mx.z() = std::max(mx.z(), p.z());
      any = true;
    }
  }
  if (!any)
    return;

  const vector3 center = (mn + mx) * 0.5f;
  const vector3 e = mx - mn;
  const float radius = vm_GetMagnitude(&e) * 0.5f;

  const float h = height() > 0 ? static_cast<float>(height()) : 480.0f;
  const float w = width() > 0 ? static_cast<float>(width()) : 640.0f;
  const float halfFovY = kFovY * 0.5f;
  const float halfFovX = std::atan(std::tan(halfFovY) * (w / h));
  const float fitY = radius / std::sin(halfFovY);
  const float fitX = radius / std::sin(halfFovX);

  WireframeViewState *v = activeView();
  v->target = center;
  v->dist = std::max(fitX, fitY) * 1.5f;
  vm_MakeIdentity(&v->orient);
  syncViewerToCamera();
  m_cameraValid = false;
  update();
}

void EditorView::paintGL() {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;

  // Distinct background per view mode.
  if (Editor_view_mode == VM_TERRAIN)
    glClearColor(0.05f, 0.10f, 0.25f, 1.0f); // dark blue for terrain
  else if (Editor_view_mode == VM_ROOM)
    glClearColor(0.14f, 0.12f, 0.10f, 1.0f); // warm grey for room
  else
    glClearColor(0.10f, 0.12f, 0.18f, 1.0f); // default mine

  glViewport(0, 0, width(), height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), height(), 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  updateCamera();

  if (Editor_view_mode == VM_TERRAIN)
    renderTerrain();
  else
    renderRooms();

  // Win32 order in DrawWorld: rooms, then object disks, then the marked/
  // current-room selection overlays on top (and paths/bnodes last).
  renderObjects();
  renderOverlays();
  renderPaths();
  renderBNodes();
  ++m_frameCount;

  // Advance the game clock so animated elements (textures, room changes,
  // rotating submodels) progress with wall-clock time.
  d3::chrono::update();
}

void EditorView::mousePressEvent(QMouseEvent *event) {
  m_lastMouse = event->pos();
  const bool ctrl = (event->modifiers() & Qt::ControlModifier) != 0;
  const bool shift = (event->modifiers() & Qt::ShiftModifier) != 0;

  // Camera-motion keys (Win32 MoveWorld): while Ctrl or Z is held, mouse
  // drags orbit/pan/zoom the wireframe view, so they must never start a pick
  // or an object drag.
  if (ctrl || m_zKeyHeld) {
    m_mouseDown = false;
    return;
  }

  if (event->button() == Qt::LeftButton) {
    m_mouseDown = true;
    m_dragged = false;
    m_pressPos = event->pos();

    // Shift+click toggles the room selection on release (Win32
    // ToggleRoomSelectedState); don't pick on press.
    if (shift)
      return;

    updateCamera();
    PickResult pick = pickAt(event->pos().x(), event->pos().y());
    if (pick.objectIndex >= 0) {
      Cur_object_index = pick.objectIndex;
      emit objectSelected(pick.objectIndex);
      ObjMoveManager.Start(width(), height(), &m_eye, &m_orient, event->pos().x(), event->pos().y());
    }
  } else if (event->button() == Qt::RightButton) {
    updateCamera();
    PickResult pick = pickAt(event->pos().x(), event->pos().y());
    if (pick.objectIndex >= 0) {
      emit objectContextMenuRequested(event->globalPos(), pick.objectIndex);
    }
  }
}

void EditorView::mouseMoveEvent(QMouseEvent *event) {
  if (m_mouseDown) {
    if ((event->pos() - m_pressPos).manhattanLength() > 4)
      m_dragged = true;
  }

  const QPoint delta = event->pos() - m_lastMouse;
  m_lastMouse = event->pos();

  if (ObjMoveManager.IsMoving()) {
    const bool leftDown = (event->buttons() & Qt::LeftButton) != 0;
    ObjMoveManager.Defer(delta.x(), delta.y(), leftDown);
    if (ObjMoveManager.IsMoving())
      update();
    return;
  }

  // Win32 MoveWorld (editor editor OnIdle -> MoveWorld): any mouse motion
  // while Ctrl/Z is held orbits/pans/zooms the wireframe view, independent of
  // which mouse button (if any) is pressed.
  const bool ctrl = (event->modifiers() & Qt::ControlModifier) != 0;
  const bool shift = (event->modifiers() & Qt::ShiftModifier) != 0;
  if (ctrl || shift || m_zKeyHeld) {
    moveWorld(delta.x(), delta.y(), ctrl, shift, m_zKeyHeld);
    update();
  }
}

void EditorView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (ObjMoveManager.IsMoving()) {
      ObjMoveManager.End();
    } else if (m_mouseDown && !m_dragged) {
      updateCamera();
      const bool shift = (event->modifiers() & Qt::ShiftModifier) != 0;
      if (shift) {
        // Shift+click toggles the current room selection (Win32
        // ToggleRoomSelectedState).
        PickResult pick = pickAt(event->pos().x(), event->pos().y());
        if (pick.roomIndex >= 0)
          emit roomToggleRequested(pick.roomIndex);
        else
          emit selectionCleared();
      } else {
        PickResult pick = pickAtCycle(event->pos().x(), event->pos().y());
        if (pick.objectIndex >= 0) {
          emit objectSelected(pick.objectIndex);
        } else if (pick.roomIndex >= 0 && pick.faceIndex >= 0) {
          emit faceSelected(pick.roomIndex, pick.faceIndex);
        } else {
          m_pickRoom = -1;
          m_pickFace = -1;
          m_pickCenterDist = 1e30f;
          emit selectionCleared();
        }
      }
    }
    m_mouseDown = false;
    m_dragged = false;
  }
}

void EditorView::keyPressEvent(QKeyEvent *event) {
  // Win32 MoveWorld keys are polled by KEY_STATE(KEY_Z) each idle frame; the
  // Qt equivalent tracks the Z key between move events.  The event is passed
  // on so accelerator/menu handling in the parent is not blocked.
  if (event->key() == Qt::Key_Z) {
    m_zKeyHeld = true;
    event->accept();
    return;
  }
  QOpenGLWidget::keyPressEvent(event);
}

void EditorView::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Z) {
    m_zKeyHeld = false;
    event->accept();
    return;
  }
  QOpenGLWidget::keyReleaseEvent(event);
}

void EditorView::focusOutEvent(QFocusEvent *event) {
  m_zKeyHeld = false;
  QOpenGLWidget::focusOutEvent(event);
}

bool EditorView::pointInPolygon(float px, float py, const float *sx, const float *sy, int n) {
  bool inside = false;
  for (int i = 0, j = n - 1; i < n; j = i++) {
    if (((sy[i] > py) != (sy[j] > py)) &&
        (px < (sx[j] - sx[i]) * (py - sy[i]) / (sy[j] - sy[i]) + sx[i]))
      inside = !inside;
  }
  return inside;
}

EditorView::PickResult EditorView::pickAt(int screenX, int screenY) const {
  PickResult pick = pickAtImpl(screenX, screenY, -1, -1, 1e30f);
  const WireframeViewState &v = activeWireframeView();
  qInfo().noquote()
      << "PICK" << "target=(" << v.target.x() << "," << v.target.y() << "," << v.target.z() << ")"
      << "dist=" << v.dist << "rad=" << v.rad << "zoom=" << m_dist
      << "click=(" << screenX << "," << screenY << ")"
      << "room=" << pick.roomIndex << "face=" << pick.faceIndex
      << "obj=" << pick.objectIndex << "depth=" << pick.depth;
  return pick;
}

EditorView::PickResult EditorView::pickAtCycle(int screenX, int screenY) {
  const bool sameSpot = (m_pickScreen == QPoint(screenX, screenY));
  // FM_NEXT: only faces farther (by eye->face-center distance) than the
  // previously picked face are considered, so seed with that center distance.
  // A brand-new click position uses 0 (no gate: pick the closest front face).
  PickResult best =
      pickAtImpl(screenX, screenY, sameSpot ? m_pickRoom : -1,
                 sameSpot ? m_pickFace : -1, sameSpot ? m_pickCenterDist : 0.0f);
  // Remember the picked surface (and its center distance) for the next cycle.
  if (best.roomIndex >= 0 && best.faceIndex >= 0) {
    m_pickRoom = best.roomIndex;
    m_pickFace = best.faceIndex;
    m_pickCenterDist = best.depth;
    m_pickScreen = QPoint(screenX, screenY);
  }
  return best;
}

// Shared implementation mirroring Win32 WireframeFindRoomFace / CheckRoom.  A
// face is a candidate only if it is FRONT-FACING (Win32 DoFacingCheck: back
// faces culled via the geometric normal) and its polygon covers the click
// pixel.  For a plain pick (prevRoom < 0, FM_CLOSEST) the winner is the
// candidate nearest along the view ray at the pixel (the Z-buffer front-most
// face) -- i.e. minimum per-pixel depth.  For FM_NEXT (prevRoom/prevFace valid,
// e.g. via pickAtCycle) only candidates whose eye->face-center distance is
// strictly greater than prevCenterDist are considered, and the winner is the
// one with the minimum such eye->face-center distance (Win32:
// dist > Search_min_dist with Search_min_dist = Found_dist, then min dist).
EditorView::PickResult EditorView::pickAtImpl(int screenX, int screenY, int prevRoom,
                                               int prevFace, float prevCenterDist) const {
  PickResult best;
  const bool cycle = prevRoom >= 0 && prevFace >= 0;

  const_cast<EditorView *>(this)->updateCamera();
  if (!m_cameraValid)
    return best;

  // In terrain mode, no room picking (terrain not rendered).
  // In room mode, only pick from the current palette room.
  int pickStart = 0;
  int pickEnd = Highest_room_index;
  if (Editor_view_mode == VM_TERRAIN)
    return best;
  if (Editor_view_mode == VM_ROOM) {
    if (D3EditState.current_room < 0 || D3EditState.current_room > Highest_room_index)
      return best;
    pickStart = D3EditState.current_room;
    pickEnd = D3EditState.current_room;
  }

  const float rad2 = m_rad * m_rad;
  for (int r = pickStart; r <= pickEnd; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;

    // Win32 DrawAllRooms gating: only rooms whose first vertex lies within the
    // wireframe render radius of the orbit target are candidates, except the
    // current room, which is always a candidate (VM_ROOM already restricts to
    // a single room, so this only matters in VM_MINE).
    const bool isCurrent = (D3EditState.current_room == r);
    if (!isCurrent && rp->num_verts > 0 &&
        (vm_VectorDistance(&rp->verts[0], &m_target) * vm_VectorDistance(&rp->verts[0], &m_target)) > rad2)
      continue;

    for (int f = 0; f < rp->num_faces; f++) {
      face *fp = &rp->faces[f];
      int nv = fp->num_verts;
      if (nv < 3 || nv > MAX_VERTS_PER_FACE)
        continue;

      float sx[64], sy[64], sz[64];
      int inFront = 0;
      for (int v = 0; v < nv; v++) {
        if (projectVertexDepth(rp->verts[fp->face_verts[v]], &sx[v], &sy[v], &sz[v]))
          inFront++;
      }
      if (inFront < 1)
        continue;

      if (!pointInPolygon(static_cast<float>(screenX), static_cast<float>(screenY), sx, sy, nv))
        continue;

      // Win32 DoFacingCheck: back-face cull by the geometric face normal.  In
      // view space the face is front-facing when dot(n_view, vert_view) < 0,
      // where n_view = vm_GetPerp(v0, v1, v2) = (v1 - v0) x (v2 - v0) computed
      // from the VIEW-SPACE vertex coordinates (as g3_RotatePoint produces).
      // We must use g3_RotatePoint's exact row-vector view transform
      // (p3_vec = d * View_matrix, which is the transpose of projectVertexDepth's
      // c = M * d) rather than camera-relative world-space vectors: the view
      // transform is a reflection (left-handed), so a naive world-space cross
      // product would flip the facing sign.  Back faces are never selectable.
      {
        const vector3 &w0 = rp->verts[fp->face_verts[0]];
        const vector3 &w1 = rp->verts[fp->face_verts[1]];
        const vector3 &w2 = rp->verts[fp->face_verts[2]];
        auto toViewScratch = [](const vector3 &w, const vector3 &eye,
                                const matrix &o, vector3 &c) {
          // D3 g3_RotatePoint computes p3_vec = d * View_matrix (row-vector
          // convention), i.e. the TRANSPOSE of projectVertexDepth's c = M*d.
          // DoFacingCheck must use this exact view space, so replicate the
          // d * View_matrix row-vector form (positive scalings dropped --
          // they cannot change the facing sign).
          const vector3 d = w - eye;
          c.x() = d.x() * o.rvec.x() + d.y() * o.uvec.x() + d.z() * o.fvec.x();
          c.y() = d.x() * o.rvec.y() + d.y() * o.uvec.y() + d.z() * o.fvec.y();
          c.z() = d.x() * o.rvec.z() + d.y() * o.uvec.z() + d.z() * o.fvec.z();
        };
        vector3 v0, v1, v2;
        toViewScratch(w0, m_eye, m_orient, v0);
        toViewScratch(w1, m_eye, m_orient, v1);
        toViewScratch(w2, m_eye, m_orient, v2);
        // n = (v1-v0) x (v2-v0) in view space, replicating Win32 vm_GetPerp
        // (the mini-lib vm_GetPerp is a stub, so compute the cross directly).
        const vector3 m10 = v1 - v0;
        const vector3 m20 = v2 - v0;
        const vector3 n = vm_Cross3Product(m10, m20);
        float dotv1 = vm_Dot3Product(n, v1);
        if (!(dotv1 < 0.0f))
          continue; // back-facing -> not a valid pick candidate (Win32)
      }

      // Perspective-correct depth of the surface exactly at the clicked pixel
      // (used to resolve the nearest front-facing face for a plain pick).  1/z
      // is linear in screen space for a perspective projection, so barycentric
      // interpolation of the inverse vertex depths across the clicked triangle
      // gives the true depth of the surface under the mouse.
      const float px = static_cast<float>(screenX);
      const float py = static_cast<float>(screenY);
      bool found = false;
      float pixelDepth = 0.0f;
      for (int v = 1; v + 1 < nv; v++) {
        const float invB = 1.0f / sz[0];
        const float invC = 1.0f / sz[v];
        const float invD = 1.0f / sz[v + 1];
        const float det = (sx[v] - sx[0]) * (sy[v + 1] - sy[0]) -
                          (sx[v + 1] - sx[0]) * (sy[v] - sy[0]);
        if (std::fabs(det) < 1e-9f)
          continue;
        const float invDet = 1.0f / det;
        const float lamB = ((px - sx[0]) * (sy[v + 1] - sy[0]) -
                            (sx[v + 1] - sx[0]) * (py - sy[0])) * invDet;
        const float lamC = ((sx[v] - sx[0]) * (py - sy[0]) -
                            (px - sx[0]) * (sy[v] - sy[0])) * invDet;
        if (lamB < -1e-4f || lamC < -1e-4f || lamB + lamC > 1.0f + 1e-4f)
          continue;
        const float lamA = 1.0f - lamB - lamC;
        const float invDepth = lamA * invB + lamB * invC + lamC * invD;
        if (invDepth > 0.0f) {
          pixelDepth = 1.0f / invDepth;
          found = true;
          break;
        }
      }
      if (!found)
        continue;

      // Win32 ComputeCenterPointOnFace + vm_VectorDistance: the distance from
      // the viewer's EYE to the (vertex-averaged) face center.  This is the
      // metric Win32 stores in Found_dist and uses to order FM_NEXT.
      vector3 center;
      ComputeCenterPointOnFace(&center, rp, f);
      const float centerDist = vm_VectorDistance(&center, &m_eye);

      if (cycle) {
        // FM_NEXT: strictly farther (by eye->face-center distance) than the
        // previously picked face, choosing the minimum such distance.
        if (centerDist <= prevCenterDist + 1e-4f)
          continue;
        if (centerDist < best.depth) {
          best.roomIndex = r;
          best.faceIndex = f;
          best.depth = centerDist;
        }
      } else {
        // FM_CLOSEST: nearest front-facing face at the pixel (Z-buffer result).
        if (pixelDepth < best.depth) {
          best.roomIndex = r;
          best.faceIndex = f;
          best.depth = pixelDepth;
        }
      }
    }
  }

  for (int i = 0; i <= Highest_object_index; i++) {
    object *obj = &Objects[i];
    if (obj->type == OBJ_NONE)
      continue;

    float ox, oy, oz;
    if (!projectVertexDepth(obj->pos, &ox, &oy, &oz))
      continue;

    const float h = height() > 0 ? static_cast<float>(height()) : 1.0f;
    const float focal = (h * 0.5f) / std::tan(kFovY * 0.5f);
    float screenRadius = (obj->size * focal) / oz;
    if (screenRadius < 6.0f)
      screenRadius = 6.0f;

    float dx = static_cast<float>(screenX) - ox;
    float dy = static_cast<float>(screenY) - oy;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= screenRadius) {
      if (best.objectIndex < 0 || oz < best.depth) {
        best.objectIndex = i;
        best.depth = oz;
      }
    }
  }

  return best;
}

void EditorView::startRectSelection() {
  m_rectSelecting = true;
  m_selectedRect = QRect();
}

void EditorView::endRectSelection() {
  m_rectSelecting = false;
  if (!m_selectedRect.isNull()) {
    m_selectedRect = m_selectedRect.normalized();
    emit rectSelectionFinished(m_selectedRect);
  }
}

void EditorView::getSelectedRect(int *l, int *t, int *r, int *b) const {
  QRect norm = m_selectedRect.normalized();
  *l = norm.left();
  *t = norm.top();
  *r = norm.right();
  *b = norm.bottom();
}

