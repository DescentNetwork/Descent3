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

#include <QHash>
#include <QOpenGLWidget>
#include <QPoint>
#include <QRect>
#include <QVector3D>

#include "vecmat.h"


// EditorView is the Qt port of the Win32 editor's CTextureGrWnd /
// CWireframeGrWnd: a QOpenGLWidget that renders the current mine.
// The D3 renderer (renderer/HardwareOpenGL.cpp) owns its own SDL window and
// is not used here; instead the mine geometry is projected with the vecmat
// math and drawn directly with OpenGL, giving a correct textured/wireframe
// view of the loaded level.
class EditorView : public QOpenGLWidget {
  Q_OBJECT
public:
  explicit EditorView(QWidget *parent = nullptr);
  ~EditorView() override;

  int frameCount() const { return m_frameCount; }

  // Forces a redraw (the Win32 InvalidateRect equivalent).
  void requestRedraw();

  // Win32 ResetWireframeView (editor/moveworld.cpp:164-172): restores the
  // active wireframe view to identity orientation aimed at Mine_origin with
  // dist=500 / rad=5000.  Called after level load/new (editor/HFile.cpp:465)
  // and by the View>Center on Mine menu (editor/MainFrm.cpp:2214).
  void resetCamera();

  // Test/UI utility: frames the active view so the whole mine fits in the
  // current widget at identity orientation.  The Win32 editor does not auto-
  // fit after load, so tests call this to get deterministic framing instead of
  // relying on the (correct) default camera aimed at Mine_origin.
  void fitToMine();

  // Turntable camera state, a direct port of the Win32 wireframe_view
  // (editor/moveworld.cpp:104-113): the camera orbits around `target`, the
  // eye sits at target - orient.fvec * dist, and the wireframe render/pick
  // radius is `rad`.  Separate states are kept for the mine and room views
  // (Wireframe_view_mine / Wireframe_view_room, SetViewMode at
  // editor/MainFrm.cpp:2948-2951).
  struct WireframeViewState {
    matrix orient = IDENTITY_MATRIX;
    vector3 target = vector3{0, 0, 0};
    float dist = 500.0f;  // DEFAULT_VIEW_DIST
    float rad = 5000.0f;  // DEFAULT_VIEW_RAD
  };

  // Sets the active wireframe view orientation (and optionally distance) and
  // forces a redraw.  Angles are in degrees; the default distance keeps the
  // current dist.
  void setOrbitCamera(float yawDeg, float pitchDeg, float dist = -1.0f);

  QSize renderSize() const;
  void setPickRadius(float radius);

  // Win32 ResetWireframeView / SetWireframeView / ResetWireframeViewRad
  // (editor/moveworld.cpp:164-186): reset restores identity orientation aimed
  // at Mine_origin with the default dist/rad; SetWireframeView re-aims the
  // active view at `pos`; ResetWireframeViewRad restores the render radius.
  void resetWireframeView();
  void setWireframeView(const vector3 &pos);
  void resetWireframeViewRad();

  // Win32 MoveWorld (editor/moveworld.cpp:120-158): rotates the active
  // wireframe view around its target (Ctrl+drag), pans the target along the
  // view right/up axes (Ctrl+Shift+drag), zooms dist (Z+drag) or changes the
  // render radius rad (Z+Shift+drag).
  void moveWorld(int dx, int dy, bool ctrlDown, bool shiftDown, bool zDown);

  // The active wireframe view state (mine view for VM_MINE/VM_TERRAIN, room
  // view for VM_ROOM).
  const WireframeViewState &activeWireframeView() const;

  bool isWireframe() const { return m_wireframe; }
  void enableWireframeMode(void) { setWireframe(true); }
  void disableWireframeMode(void) { setWireframe(false); }
  void setWireframe(bool wireframe);

  // Renders the mine from the viewer; used by tests to grab the projected
  // geometry without needing a GL context.
  struct ProjectedVertex {
    float x, y; // normalized screen coords (0..1)
    float z;    // camera depth (positive = in front)
  };
  void projectMine(QVector<QVector<ProjectedVertex>> *outFaces) const;

  // Projects a world-space point to screen pixels using the same camera and
  // math as pickAt(); returns false when the point is behind the camera.
  bool projectWorldToScreen(const vector3 &world, float *sx, float *sy, float *depth = nullptr) const;

  // Picking result returned by pickAt().
  struct PickResult {
    int roomIndex = -1;
    int faceIndex = -1;
    int objectIndex = -1;
    float depth = 1e30f;
  };

  // Identifies what is under the given screen pixel (left-hand origin).
  PickResult pickAt(int screenX, int screenY) const;  // Same as pickAt(), but on a repeated click over the same surface cycles to
  // the next farther face (Win32 FM_NEXT behavior).
  PickResult pickAtCycle(int screenX, int screenY);
  // Shared pick implementation mirroring Win32 WireframeFindRoomFace.  A face
  // is a candidate only if it is FRONT-FACING (Win32 DoFacingCheck) and covers
  // the click pixel.  When (prevRoom, prevFace) are valid, FM_NEXT cycling is
  // active and only faces whose eye->face-center distance is strictly greater
  // than prevCenterDist are considered, choosing the minimum such distance.
  PickResult pickAtImpl(int screenX, int screenY, int prevRoom, int prevFace,
                        float prevCenterDist) const;

signals:
  void faceSelected(int roomIndex, int faceIndex);
  void objectSelected(int objIndex);
  void selectionCleared();
  void objectContextMenuRequested(const QPoint &globalPos, int objIndex);
  // Emitted on Shift+click over a room to toggle that room's selection
  // (Win32 ToggleRoomSelectedState).
  void roomToggleRequested(int roomIndex);
  void rectSelectionFinished(const QRect &rect);

  // Rectangle selection (rubber band / SelManager equivalent).
public:
  void startRectSelection();
  bool isRectSelecting() const { return m_rectSelecting; }
  QRect selectedRect() const { return m_selectedRect; }
  void getSelectedRect(int *l, int *t, int *r, int *b) const;

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;

private:
  bool projectVertex(const vector3 &world, float *sx, float *sy) const;
  bool projectVertexDepth(const vector3 &world, float *sx, float *sy, float *depth) const;
  float getFocal() const;
  void updateCamera();
  void drawTerrainDots();
  void renderRooms();
  void renderOverlays();
  void renderTerrain();
  void renderObjects();
  void renderPaths();
  void renderBNodes();

  static bool pointInPolygon(float px, float py, const float *sx, const float *sy, int n);

  vector3 m_eye;
  matrix m_orient;
  bool m_cameraValid = false;

  bool m_wireframe = false;
  int m_frameCount = 0;

  // Turntable camera state (Win32 wireframe_view).  m_viewMine is used for
  // VM_MINE and VM_TERRAIN; m_viewRoom for VM_ROOM (editor/MainFrm.cpp:2948).
  WireframeViewState m_viewMine;
  WireframeViewState m_viewRoom;
  WireframeViewState *activeView();
  const WireframeViewState *activeView() const;

  // Active camera mirrors, refreshed from the active wireframe view in
  // updateCamera() (m_eye/m_orient follow the Viewer_object when one exists).
  QPoint m_lastMouse;
  float m_dist = 500.0f;
  float m_rad = 5000.0f; // wireframe render radius
  vector3 m_target;
  bool m_zKeyHeld = false; // Z held -> zoom/radius drag (Win32 KEY_Z)

  // Face picking cycle state (Win32 WireframeFindRoomFace parity): the most
  // recently picked (room, face, eye->face-center distance).  A repeated click
  // without drag reuses these to pick the next-farther front-facing face under
  // the same pixel (FM_NEXT), ordered by increasing eye->face-center distance.
  int m_pickRoom = -1;
  int m_pickFace = -1;
  float m_pickCenterDist = 1e30f;
  QPoint m_pickScreen = QPoint(-1, -1);

  // Click vs drag tracking.
  bool m_mouseDown = false;
  bool m_dragged = false;
  QPoint m_pressPos;

  // Rectangle selection (SelManager equivalent).
  bool m_rectSelecting = false;
  QRect m_selectedRect;
  void endRectSelection();

  // Cached OpenGL textures, keyed by D3 bitmap handle.
  mutable QHash<int, GLuint> m_textures;
  void ensureTexture(int bmHandle);
};

