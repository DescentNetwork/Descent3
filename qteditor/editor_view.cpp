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
#include "room.h"
#include "terrain.h"


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
  float dummy;
  return projectVertexDepth(world, sx, sy, &dummy);
}

bool EditorView::projectVertexDepth(const vector &world, float *sx, float *sy, float *depth) const {
  vector d = world - m_eye;
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
  // VM_MINE: All rooms. VM_ROOM: Single palette room. VM_TERRAIN: None.
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

  for (int r = renderStart; r <= renderEnd; r++) {
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

  // ---- Selection highlights ----
  // Draw wireframe overlays for the current room/face/edge and marked
  // room/face so the user gets visual feedback after clicking.
  if (Curroomp != nullptr && Curroomp->used) {
    const int curRoom = ROOMNUM(Curroomp);

    // Only highlight if this room is in the rendered set.
    if (curRoom >= renderStart && curRoom <= renderEnd) {
      // Draw all edges of the current room in white.
      glColor3f(1.0f, 1.0f, 1.0f);
      glLineWidth(1.0f);
      for (int i = 0; i < Curroomp->num_faces; i++) {
        face *fp = &Curroomp->faces[i];
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

      // Draw the current face in yellow (thicker).
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
          glColor3f(1.0f, 1.0f, 0.0f);
          glLineWidth(2.0f);
          glBegin(GL_LINE_LOOP);
          for (int v = 0; v < nv; v++)
            glVertex2f(sx[v], sy[v]);
          glEnd();

          // Draw the current edge in green.
          if (Curedge >= 0 && Curedge < nv) {
            int next = (Curedge + 1) % nv;
            glColor3f(0.0f, 1.0f, 0.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glVertex2f(sx[Curedge], sy[Curedge]);
            glVertex2f(sx[next], sy[next]);
            glEnd();
          }
          glLineWidth(1.0f);
        }
      }
    }
  }

  // Draw the marked room/face in cyan if different from current.
  if (Markedroomp != nullptr && Markedroomp->used) {
    const int markedRoom = ROOMNUM(Markedroomp);
    if (markedRoom >= renderStart && markedRoom <= renderEnd) {
      // Marked face outline.
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
          glColor3f(0.0f, 1.0f, 1.0f);
          glLineWidth(2.0f);
          glBegin(GL_LINE_LOOP);
          for (int v = 0; v < nv; v++)
            glVertex2f(sx[v], sy[v]);
          glEnd();
          glLineWidth(1.0f);
        }
      }
    }
  }
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
      const int idx00 = z * (TERRAIN_WIDTH + 1) + x;
      const int idx10 = z * (TERRAIN_WIDTH + 1) + (x + 1);
      const int idx01 = (z + 1) * (TERRAIN_WIDTH + 1) + x;
      const int idx11 = (z + 1) * (TERRAIN_WIDTH + 1) + (x + 1);

      const vector v00{float(x * TERRAIN_SIZE), Terrain_seg[idx00].y,
                       float(z * TERRAIN_SIZE)};
      const vector v10{float((x + 1) * TERRAIN_SIZE), Terrain_seg[idx10].y,
                       float(z * TERRAIN_SIZE)};
      const vector v01{float(x * TERRAIN_SIZE), Terrain_seg[idx01].y,
                       float((z + 1) * TERRAIN_SIZE)};
      const vector v11{float((x + 1) * TERRAIN_SIZE), Terrain_seg[idx11].y,
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
}

void EditorView::renderObjects() {
  if (Editor_view_mode == VM_TERRAIN)
    return;

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
    if (screenRadius < 4.0f)
      screenRadius = 4.0f;

    // Color by object type.
    switch (obj->type) {
    case OBJ_PLAYER:
      glColor3f(0.0f, 1.0f, 0.0f);
      break; // green
    case OBJ_ROBOT:
      glColor3f(1.0f, 0.2f, 0.2f);
      break; // red
    case OBJ_WEAPON:
      glColor3f(1.0f, 1.0f, 0.0f);
      break; // yellow
    case OBJ_POWERUP:
      glColor3f(0.3f, 0.5f, 1.0f);
      break; // blue
    case OBJ_VIEWER:
      glColor3f(1.0f, 0.0f, 1.0f);
      break; // magenta
    case OBJ_CAMERA:
      glColor3f(1.0f, 1.0f, 0.5f);
      break; // yellow-green
    case OBJ_BUILDING:
      glColor3f(1.0f, 0.6f, 0.0f);
      break; // orange
    case OBJ_DOOR:
      glColor3f(0.0f, 1.0f, 1.0f);
      break; // cyan
    default:
      glColor3f(0.7f, 0.7f, 0.7f);
      break; // grey
    }

    // Highlight the currently selected object.
    if (i == Cur_object_index) {
      glLineWidth(2.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
    }

    // Draw a diamond marker around the object.
    const float r = screenRadius;
    glBegin(GL_LINE_LOOP);
    glVertex2f(ox, oy - r);
    glVertex2f(ox + r, oy);
    glVertex2f(ox, oy + r);
    glVertex2f(ox - r, oy);
    glEnd();

    // Cross inside the diamond.
    glBegin(GL_LINES);
    glVertex2f(ox - r * 0.5f, oy);
    glVertex2f(ox + r * 0.5f, oy);
    glVertex2f(ox, oy - r * 0.5f);
    glVertex2f(ox, oy + r * 0.5f);
    glEnd();

    if (i == Cur_object_index)
      glLineWidth(1.0f);
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
  if (Editor_view_mode == VM_TERRAIN) {
    min = vector{0, 0, 0};
    max = vector{float(TERRAIN_WIDTH * TERRAIN_SIZE), float(MAX_TERRAIN_HEIGHT),
                 float(TERRAIN_DEPTH * TERRAIN_SIZE)};
  } else {
    computeMineBounds(&min, &max);
  }
  m_target = (min + max) * 0.5f;
  if (!m_targetInitialized) {
    vector extent = max - min;
    float radius = extent.x() > extent.y() ? extent.x() : extent.y();
    if (extent.z() > radius)
      radius = extent.z();
    if (radius < 1.0f)
      radius = 1.0f;
    m_dist = radius * 2.5f;
    m_targetInitialized = true;
  }

  vm_AnglesToMatrix(&m_orient, 0, m_yaw * 65536.0f / 360.0f, m_pitch * 65536.0f / 360.0f);
  m_eye = m_target - m_orient.fvec * m_dist;
  m_cameraValid = true;
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

  renderObjects();
  ++m_frameCount;
}

void EditorView::mousePressEvent(QMouseEvent *event) {
  m_lastMouse = event->pos();
  if (event->button() == Qt::LeftButton) {
    m_mouseDown = true;
    m_dragged = false;
    m_pressPos = event->pos();
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
  if (!m_dragged && Viewer_object != nullptr)
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

void EditorView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && m_mouseDown && !m_dragged) {
    updateCamera();
    PickResult pick = pickAt(event->pos().x(), event->pos().y());
    if (pick.objectIndex >= 0) {
      emit objectSelected(pick.objectIndex);
    } else if (pick.roomIndex >= 0 && pick.faceIndex >= 0) {
      emit faceSelected(pick.roomIndex, pick.faceIndex);
    } else {
      emit selectionCleared();
    }
  }
  if (event->button() == Qt::LeftButton) {
    m_mouseDown = false;
    m_dragged = false;
  }
}

void EditorView::wheelEvent(QWheelEvent *event) {
  if (Viewer_object != nullptr)
    return;
  m_dist *= (event->angleDelta().y() > 0) ? 0.9f : 1.1f;
  if (m_dist < 1.0f)
    m_dist = 1.0f;
  update();
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
  PickResult best;

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

  for (int r = pickStart; r <= pickEnd; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int f = 0; f < rp->num_faces; f++) {
      face *fp = &rp->faces[f];
      int nv = fp->num_verts;
      if (nv < 3 || nv > MAX_VERTS_PER_FACE)
        continue;

      float sx[64], sy[64], sz[64];
      bool allInFront = true;
      for (int v = 0; v < nv; v++) {
        if (!projectVertexDepth(rp->verts[fp->face_verts[v]], &sx[v], &sy[v], &sz[v])) {
          allInFront = false;
          break;
        }
      }
      if (!allInFront)
        continue;

      if (!pointInPolygon(static_cast<float>(screenX), static_cast<float>(screenY), sx, sy, nv))
        continue;

      float avgDepth = 0.0f;
      for (int v = 0; v < nv; v++)
        avgDepth += sz[v];
      avgDepth /= nv;

      if (avgDepth < best.depth) {
        best.roomIndex = r;
        best.faceIndex = f;
        best.depth = avgDepth;
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

