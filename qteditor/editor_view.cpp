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

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <GL/gl.h>
#include <QSurfaceFormat>

#include <cmath>

#include "bitmap.h"
#include "d3edit.h"
#include "gametexture.h"
#include "object.h"
#include "pserror.h"
#include "room_external.h"

namespace QtEditor {

namespace {

// Vertical field of view in radians (matches the editor's default zoom).
constexpr float kFovY = 0.5445f;

void computeMineBounds(vector *min, vector *max) {
  min->x() = min->y() = min->z() = 1e30f;
  max->x() = max->y() = max->z() = -1e30f;
  bool any = false;
  for (int r = 0; r <= Highest_room_index; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int v = 0; v < rp->num_verts; v++) {
      vector *p = &rp->verts[v];
      if (p->x() < min->x())
        min->x() = p->x();
      if (p->y() < min->y())
        min->y() = p->y();
      if (p->z() < min->z())
        min->z() = p->z();
      if (p->x() > max->x())
        max->x() = p->x();
      if (p->y() > max->y())
        max->y() = p->y();
      if (p->z() > max->z())
        max->z() = p->z();
      any = true;
    }
  }
  if (!any) {
    *min = vector{-100, -100, -100};
    *max = vector{100, 100, 100};
  }
}

} // namespace

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

bool EditorView::projectVertex(const vector &world, float *sx, float *sy) const {
  vector d = world - m_eye;
  // Camera space: dot with the camera basis vectors.
  vector c;
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
  return true;
}

void EditorView::projectMine(QVector<QVector<ProjectedVertex>> *outFaces) const {
  if (outFaces == nullptr)
    return;
  const_cast<EditorView *>(this)->updateCamera();
  outFaces->clear();
  for (int r = 0; r <= Highest_room_index; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int f = 0; f < rp->num_faces; f++) {
      face *fp = &rp->faces[f];
      QVector<ProjectedVertex> face;
      bool ok = true;
      for (int v = 0; v < fp->num_verts; v++) {
        const vector &world = rp->verts[fp->face_verts[v]];
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

  QVector<uchar> rgb(w * h * 3);
  const uint16_t *data = bm_data(bmHandle, 0);
  if (data == nullptr) {
    // 8-bit palettized bitmaps are read through bm_data only when the
    // palette is applied; fall back to a magenta placeholder.
    memset(rgb.data(), 255, rgb.size());
    for (int i = 0; i < w * h; i++)
      rgb[3 * i] = 0;
  } else {
    for (int i = 0; i < w * h; i++) {
      const uint16_t p = data[i];
      const int r5 = (p >> 11) & 0x1F;
      const int g6 = (p >> 5) & 0x3F;
      const int b5 = p & 0x1F;
      rgb[3 * i + 0] = (r5 << 3) | (r5 >> 2);
      rgb[3 * i + 1] = (g6 << 2) | (g6 >> 4);
      rgb[3 * i + 2] = (b5 << 3) | (b5 >> 2);
    }
  }

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.constData());
  m_textures.insert(bmHandle, tex);
}

void EditorView::renderRooms() {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;

  for (int r = 0; r <= Highest_room_index; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int i = 0; i < rp->num_faces; i++) {
      face *fp = &rp->faces[i];

      float sx[16], sy[16];
      int nv = fp->num_verts;
      if (nv > 16)
        nv = 16;
      bool behind = false;
      for (int v = 0; v < nv; v++) {
        const vector &world = rp->verts[fp->face_verts[v]];
        if (!projectVertex(world, &sx[v], &sy[v])) {
          behind = true;
          break;
        }
      }
      if (behind)
        continue;

      // Face shading from a fixed world light direction.
      vector ld{0.4f, 0.7f, 0.6f};
      float len = vm_NormalizeVector(&ld);
      if (len < 0.001f)
        ld = vector{0, 1, 0};
      float diff = (fp->normal.x() * ld.x() + fp->normal.y() * ld.y() + fp->normal.z() * ld.z());
      float shade = 0.35f + 0.65f * (diff < 0 ? -diff : diff);
      if (shade > 1.0f)
        shade = 1.0f;

      if (m_wireframe) {
        glColor3f(shade, shade, shade);
        glBegin(GL_LINE_LOOP);
        for (int v = 0; v < nv; v++)
          glVertex2f(sx[v], sy[v]);
        glEnd();
      } else {
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
              glVertex2f(sx[v], sy[v]);
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
          glVertex2f(sx[v], sy[v]);
        glEnd();
      }
    }
  }
}

void EditorView::updateCamera() {
  m_cameraValid = false;
  if (Viewer_object != nullptr) {
    m_eye = Viewer_object->pos;
    m_orient = Viewer_object->orient;
    m_cameraValid = true;
    return;
  }
  vector min, max;
  computeMineBounds(&min, &max);
  m_target = (min + max) * 0.5f;
  vector extent = max - min;
  float radius = extent.x() > extent.y() ? extent.x() : extent.y();
  if (extent.z() > radius)
    radius = extent.z();
  if (radius < 1.0f)
    radius = 1.0f;
  m_dist = radius * 2.5f;

  vm_AnglesToMatrix(&m_orient, 0, m_yaw * 65536.0f / 360.0f, m_pitch * 65536.0f / 360.0f);
  m_eye = m_target - m_orient.fvec * m_dist;
  m_cameraValid = true;
}

void EditorView::paintGL() {
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;

  glViewport(0, 0, width(), height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width(), height(), 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  updateCamera();

  renderRooms();
  ++m_frameCount;
}

void EditorView::mousePressEvent(QMouseEvent *event) {
  m_lastMouse = event->pos();
}

void EditorView::mouseMoveEvent(QMouseEvent *event) {
  if (Viewer_object != nullptr)
    return;
  const QPoint delta = event->pos() - m_lastMouse;
  m_lastMouse = event->pos();
  m_yaw += delta.x() * 0.5f;
  m_pitch += delta.y() * 0.5f;
  if (m_pitch > 1.5f)
    m_pitch = 1.5f;
  if (m_pitch < -1.5f)
    m_pitch = -1.5f;
  update();
}

void EditorView::wheelEvent(QWheelEvent *event) {
  if (Viewer_object != nullptr)
    return;
  m_dist *= (event->angleDelta().y() > 0) ? 0.9f : 1.1f;
  if (m_dist < 1.0f)
    m_dist = 1.0f;
  update();
}

} // namespace QtEditor
