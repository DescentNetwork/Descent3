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

// Qt port of editor/TextureGrWnd.h + editor/WireframeGrWnd.h. The Win32
// CTextureGrWnd was an MFC CWnd that did its own OnPaint -> CTextureGrWnd::Render
// dance, calling StartEditorFrame / RenderMine from
// editor/gameeditor.cpp. The Qt port drives the same calls through the
// QOpenGLWidget paint override and routes the gold path through Descent3Core
// (rend_StartFrame, g3_StartFrame, RenderMine) — exactly what CTextureGrWnd
// did — once the engine-side integration lands. Until then, paintGL is a
// best-effort clear so the central widget at least has the right size and
// responds to update() while we wire the engine.

#include "editor_view.h"

#include <QOpenGLFunctions>
#include <QSurfaceFormat>

#include "d3edit.h"

namespace QtEditor {

EditorView::EditorView(QWidget *parent) : QOpenGLWidget(parent) {
  // The Qt port picks a 32-bit RGBA + depth surface — same default the
  // renderer/HardwareOpenGL.cpp asks for in renderer_preferred_state. This
  // matters only on real displays; offscreen test mode (QT_QPA_PLATFORM=offscreen)
  // ignores the format entirely.
  QSurfaceFormat fmt;
  fmt.setRenderableType(QSurfaceFormat::OpenGL);
  fmt.setRedBufferSize(8);
  fmt.setGreenBufferSize(8);
  fmt.setBlueBufferSize(8);
  fmt.setAlphaBufferSize(0);
  fmt.setDepthBufferSize(24);
  fmt.setStencilBufferSize(0);
  fmt.setSamples(0);
  setFormat(fmt);
  setMinimumSize(320, 240);
}

EditorView::~EditorView() = default;

void EditorView::requestRedraw() { update(); }

QSize EditorView::renderSize() const { return size(); }

void EditorView::initializeGL() {
  // The legacy Win32 code calls rend_Init() once at CEditorApp::InitInstance
  // time. The Qt port defers that until MainWindow requests a level load;
  // the renderer is global so reentrancy isn't an issue. Until that hook
  // lands we just clear the frame counter here so the test counts begin
  // from "frame zero".
  m_frameCount = 0;
}

void EditorView::resizeGL(int w, int h) {
  // Win32 CTextureGrWnd::OnSize sets the grViewport's bounds and triggers a
  // TextureGrStartOpenGL + Render(). The Qt port just records the size;
  // the next paintGL() re-projects.
  glViewport(0, 0, w, h);
  requestRedraw();
}

void EditorView::paintGL() {
  // The legacy Win32 Render() calls StartEditorFrame(...), RenderMine(...),
  // PostRender(...), DrawAllPaths(...), then EndEditorFrame(). On the Qt
  // port those calls become:
//   1. EditorFrame::begin(this)        (qteditor level_io equivalent below)
//   2. RenderMine(Curroomp)            (Descent3Core, gated by EDITOR)
//   3. EditorFrame::end
  //
  // The renderer's editor bridge paths (Editor_g3_vp, grSurface lock) and
  // Descent3Core's RENDERED_STATE_CHANGED guards mean the engine can paint
  // through this widget once editor/gameeditor.cpp has a Qt-side proxy.
  // Until that lands, paintGL just clears so resize / focus events have a
  // visible response.
  QOpenGLFunctions *f = context() ? context()->functions() : nullptr;
  if (f == nullptr)
    return;
  f->glClearColor(0.10f, 0.12f, 0.18f, 1.0f);
  f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ++m_frameCount;
}

} // namespace QtEditor
