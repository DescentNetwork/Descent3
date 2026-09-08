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

#include <QOpenGLWidget>
#include <QSize>

#include "vecmat.h"

// RobotPreviewWidget renders the poly model of the robot currently selected in
// the object/robot page (Object_info[D3EditState.current_robot].render_handle)
// into a QOpenGLWidget, using the standard D3 frame contract:
//   g3_StartFrame(...) + DrawPolygonModel(...) + g3_EndFrame()
// inside paintGL.  This mirrors the legacy Win32
// CWorldObjectsRobotDialog::UpdateRobotView() preview.
class RobotPreviewWidget : public QOpenGLWidget {
  Q_OBJECT
public:
  explicit RobotPreviewWidget(QWidget *parent = nullptr);
  ~RobotPreviewWidget() override;

  // Re-renders the model with the current robot selection.
  void refresh();

  // Returns the model handle currently being previewed (-1 if none).
  int currentModelHandle() const { return m_lastModel; }

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  // Cached frame interval used to animate the model's keyframes.
  void advanceAnim();

  qreal m_animTime = 0.0;
  int m_lastModel = -1;
};