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

#include <QMainWindow>

class QAction;
class QMenu;
class QTimer;
class QWidget;

namespace QtEditor {

// Constant set for the editor's view mode. Mirrors the Win32 enum in
// editor/d3edit.h (VM_MINE, VM_TERRAIN, VM_ROOM). Used by SetViewMode and
// the View menu's ID_MINE_VIEW / ID_TERRAIN_VIEW / ID_ROOM_VIEW handlers.
enum {
  VIEW_MODE_MINE = 0,
  VIEW_MODE_TERRAIN = 1,
  VIEW_MODE_ROOM = 2,
};

// Qt port of CMainFrame::SetViewMode() and the global "int Editor_view_mode"
// from editor/EDVARS.cpp. Updates the in-process view mode used by the
// renderer, status bar, and SlewFrame logic. Returns the previous mode so
// callers can save/restore when toggling temporarily.
int SetViewMode(int view_mode);
int currentViewMode();

class Dialog;
class KeypadBar;
class ViewerPropDialog;
class EditorView;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  // Generic "this feature is not yet ported" prompt used by the menu handlers
  // for items whose dialog has not been reimplemented for Qt.
  void showNotPorted(const QString &name);

  // CMainFrame OnIdle equivalent. The Qt port uses a QTimer so it ticks when
  // the application is idle without spinning a CPU. Default interval is
  // 100ms — matches the Win32 editor's frame cadence for keypad state sync.
  void startOnIdleTimer(int intervalMs = 100);
  void stopOnIdleTimer();
  bool isOnIdleTimerActive() const;

  // Save / restore dock state and main-window geometry to QSettings so the
  // editor reopens with the same docked keypads / window size the user
  // closed with. Mirrors the Win32 CMainFrame's ShowWindow/RestoreWindow
  // paths around OnDestroy / OnCreateClient.
  void saveWindowState();
  void restoreWindowState();

  // Number of times the on-idle timer has fired since the window was
  // created. Exposed for tests so we can verify the timer is actually
  // running without having to observe status-bar side effects.
  int onIdleTickCount() const { return m_onIdleTickCount; }

private slots:
  void onIdleTimer();

private:
  // Looks up a menu action by its ID_* identifier in the .ui-loaded action
  // host (falling back to a stub action if the identifier is missing).
  QAction *action(const QString &id);
  void buildMenus();
  void buildKeypadBar();
  QMenu *addMenu(const QString &title);

  void toggleKeypadBar();
  void showAboutBox();
  void showLevelInfo();
  void showPreferences();
  void toggleViewerProps();
  void showHogMaker();
  void showSelectRange();
  void showTerrainSound();
  void showWorldObjectsDoor();
  void showWorldObjectsPlayer();
  void showWorldObjectsLight();
  void showWorldObjectsSound();
  void showWorldWeapons();
  void showWorldTextures();
  void showGenericObject(int objType, int current);

  // File-menu slots driven by ID_FILE_NEW / ID_FILE_OPEN / ID_FILE_SAVE /
  // ID_FILE_SAVE_AS. They go through OpenFileDialog / SaveFileDialog and
  // remember the resulting path so subsequent Save calls write to it without
  // re-prompting. The actual level I/O still delegates to editor/HFile.cpp
  // BringUpLevel / SaveLevel etc.; the Qt port tracks the path that those
  // will be passed.
  void onFileNew();
  void onFileOpen();
  void onFileSave();
  void onFileSaveAs();
  // ID_FILE_STATS / ID_FILE_VERIFY_LEVEL / ID_FILE_FIXCRACKS map to the
  // editor/ShowLevelStats / level verification / crack-fixing routines in
  // HFile.cpp. The Qt port routes them via level_io helpers + status
  // messages; the real implementations will land once the engine-side
  // level walker ships.
  void onFileStats();
  void onFileVerifyLevel();
  void onFileFixCracks();

  // View-menu slots for ID_MINE_VIEW / ID_TERRAIN_VIEW / ID_ROOM_VIEW macros
  // call SetViewMode(). Toggle on the docked keypad bar, mirror Win32.
  void onViewMine();
  void onViewTerrain();
  void onViewRoom();
  // ID_VIEW_TOOLBAR toggles the keypad dock (Win32's "toolbar" maps to the
  // keypad dialogbar in the Qt port — there is no separate toolbar widget
  // yet). ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW mirrors D3EditState.objects_in_wireframe
  // — toggling it changes the wireframe redraw behaviour without rebuilding
  // geometry.
  void onViewToolbar();
  void onViewShowObjectsInWireframe();
  QWidget *m_actionsHost = nullptr;
  QDockWidget *m_keypadDock = nullptr;
  KeypadBar *m_keypadBar = nullptr;
  Dialog *m_aboutBox = nullptr;
  ViewerPropDialog *m_viewerProps = nullptr;
  EditorView *m_editorView = nullptr;
  QTimer *m_idleTimer = nullptr;

  // Path of the currently open .d3l, or empty if none / untitled. Updated by
  // onFileOpen / onFileSaveAs and cleared by onFileNew.
  QString m_currentLevelFile;

  // Drives the on-idle tick counter for tests.
  int m_onIdleTickCount = 0;
};

}
