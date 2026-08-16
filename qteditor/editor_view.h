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

namespace QtEditor {

// EditorView is the Qt port of the Win32 editor's CTextureGrWnd:
// a QOpenGLWidget that hosts the renderer and repaints the current mine
// whenever the level state changes. The Win32 class was driven directly by
// MFC's OnPaint message; the Qt port uses QOpenGLWidget's
// initializeGL/resizeGL/paintGL trio and renders inside paintGL() via the
// Qt-side proxy of editor/gameeditor.cpp's StartEditorFrame/EndEditorFrame.
class EditorView : public QOpenGLWidget {
  Q_OBJECT
public:
  explicit EditorView(QWidget *parent = nullptr);
  ~EditorView() override;

  // Number of frames the view has actually painted. Exposed so tests can
  // verify the widget is alive and ticking without driving a full GL
  // pipeline (tests run in offscreen mode where paintGL fires once only).
  int frameCount() const { return m_frameCount; }

  // Forces a redraw. Mirrors the Win32 ::InvalidateRect() the legacy
  // CMainFrame triggers from its OnIdle loop. Trade in a small bit of
  // paint traffic for predictable cross-platform behaviour.
  void requestRedraw();

  // Size in device pixels, exposed for status-bar reporting ("500×400"
  // replacement for the Win32 GetClientRect path MainFrm::OnSize walked).
  QSize renderSize() const;

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  int m_frameCount = 0;
};

} // namespace QtEditor
