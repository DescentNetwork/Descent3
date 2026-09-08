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

#include "robot_preview_widget.h"

#include <GL/gl.h>

#include <cmath>

#include "d3edit.h"
#include <QtGlobal>
#include <QTimer>

#include "descent.h"
#include "objinfo.h"
#include "polymodel.h"
#include "renderer.h"
#include "vecmat.h"

RobotPreviewWidget::RobotPreviewWidget(QWidget *parent) : QOpenGLWidget(parent) {
  // A fixed FOV matching the editor's default camera so the model fills the
  // view port predictably regardless of the D3_DEFAULT_ZOOM the frame uses.
}

RobotPreviewWidget::~RobotPreviewWidget() {}

void RobotPreviewWidget::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.35f, 0.35f, 0.35f, 1.0f);
}

void RobotPreviewWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void RobotPreviewWidget::advanceAnim() {
  // Advance a small animation phase so spinning/frame-based models move.
  m_animTime += 0.02;
  if (m_animTime > 1.0f)
    m_animTime = 0.0f;
  update();
}

void RobotPreviewWidget::refresh() { update(); }

void RobotPreviewWidget::paintGL() {
  const QSize sz = size();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const int model = D3EditState.current_robot;
  if (model < 0 || model >= MAX_OBJECT_TYPES)
    return;
  object_info *oi = &Object_info[model];
  const int pmHandle = oi->render_handle;
  if (pmHandle < 0 || pmHandle >= MAX_POLY_MODELS)
    return;

  poly_model *pm = GetPolymodelPointer(pmHandle);
  if (pm == nullptr || !pm->used)
    return;
  m_lastModel = pmHandle;

  // Set an orthogonal projection so the model is drawn in a 2D-style viewport,
  // matching how the model renderer projects into Window_*.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, sz.width(), sz.height(), 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Camera framing: place the eye far enough away to fit the model, like the
  // legacy robot preview (view_vector.z = -(maxs.z - mins.z)*2).
  vector3 viewVector = {0, 0, 0};
  viewVector.z() = -(std::fabs(pm->maxs.z() - pm->mins.z()) * 2.0f);
  if (viewVector.z() >= 0)
    viewVector.z() = -40.0f;

  matrix idMatrix;
  vm_MakeIdentity(&idMatrix);

  // Rotate the model slowly for a live preview.
  matrix rotMatrix;
  vm_AnglesToMatrix(&rotMatrix, 0, (angle)(m_animTime * 360.0 * 8.0f), 0);

  float normTime[MAX_SUBOBJECTS];
  for (int i = 0; i < MAX_SUBOBJECTS; i++)
    normTime[i] = (float)m_animTime;

  vector3 zeroVector{};
  g3_StartFrame(&viewVector, &idMatrix, D3_DEFAULT_ZOOM);
  DrawPolygonModel(&zeroVector, &rotMatrix, pmHandle, normTime, 0, 0.9f, 0.9f, 0.9f);
  g3_EndFrame();
}