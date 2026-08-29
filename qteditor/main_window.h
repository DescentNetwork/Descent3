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

#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QDialog>
#include <QWidget>
#include <QTimer>
#include <QFileInfo>
//#include <DockManager.h>
//#include <DockWidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


// Constant set for the editor's view mode. Mirrors the Win32 enum in
// editor/d3edit.h (VM_MINE, VM_TERRAIN, VM_ROOM). Used by SetViewMode and
// the View menu's ID_MINE_VIEW / ID_TERRAIN_VIEW / ID_ROOM_VIEW handlers.
enum class view_mode_t
{
  VIEW_MODE_MINE = 0,
  VIEW_MODE_TERRAIN,
  VIEW_MODE_ROOM,
};


class KeypadBar;
class ViewerPropDialog;
class EditorView;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  // Generic "this feature is not yet ported" prompt used by the menu handlers
  // for items whose dialog has not been reimplemented for Qt.
  void showNotPorted(const QString &name);

  // Save / restore dock state and main-window geometry to QSettings so the
  // editor reopens with the same docked keypads / window size the user
  // closed with. Mirrors the Win32 CMainFrame's ShowWindow/RestoreWindow
  // paths around OnDestroy / OnCreateClient.
  void saveWindowState();
  void restoreWindowState();


private:
  void buildKeypadBar();

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
  void showLevelProperties();
  void showMegacells();
  void showAmbientSounds();
  void showScriptInterface();
  void showFilePage();
  void showAISettings();
  void showOrphanHunter();
  void showBriefingEditor();
  void showFontEditor();
  void showTableFileEdit();
  void showTableFileFilter();
  void showDallas();
  void showReorderPages();
  void showAllCheckedOut();
  void showBitmapImporter();
  void showHotSpotTGA();

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
  // ID_VIEW_TOOLBAR toggles the main toolbar. ID_VIEW_KEYPAD_TOGGLE
  // toggles the keypad dock.
  void onViewToolbar();
  void onViewShowObjectsInWireframe();
  void onButtonOutline();

  // Room menu slots that delegate to room_ops. The Room menu has many more
  // items than this list — they route through wireNotPorted() above — but
  // these ones have real Qt-side implementations.
  void onRoomAdd();
  void onRoomDelete();
  void onRoomDeleteFace();
  void onRoomDeletePortal();
  void onRoomMark();
  void onRoomSelectByNumber();
  void onRoomRename();
  void onRoomSaveCurrent();
  void onRoomCombine();
  void onRoomRotatePlaced45();
  void onRoomAttach();

  // ====== VIEWER OPERATIONS ======
  void onCenterViewOnMine();
  void onCenterViewOnObject();
  void onResetViewRadius();
  void onMoveViewToSelectedRoom();

  // ====== OBJECT OPERATIONS ======
  int onPlaceCameraAtViewer();
  void onSetViewerFromCamera();
  void onSetCameraFromViewer();
  void onDeleteCurrentObject();
  void onMovePlayerToCurrentRoom();
  void onSelectNextObject(int from);
  void onSelectPrevObject(int from);
  void onCreateNewViewer();
  int onSpawnNewViewer();
  int onSelectNextViewer();
  void onDeleteCurrentViewer();
  int onSelectObjectByNumber();
  void onSelectObject(int objnum);

  // ====== CLIPBOARD OPERATIONS ======
  void onCopyObjectToClipboard();
  void onCutObjectToClipboard();
  void onPasteObjectFromClipboard();
  bool HasClipboardObject();
  void ClearClipboard();

  // ====== ROOM OPERATIONS ======
  bool onAddRoom();
  bool onDeleteRoom();
  void onMarkRoom();
  int onSelectRoomByNumber();
  bool onRenameRoom();
  bool onSaveCurrentRoom();

private:
  // Vars
  Ui::MainWindow *ui;

  QMenuBar *m_menuBar = nullptr;
  QDockWidget* m_keypadDock = nullptr;
  QTabWidget *m_keypadTabs = nullptr;
  QDialog *m_aboutBox = nullptr;
  ViewerPropDialog *m_viewerProps = nullptr;
  EditorView *m_editorView = nullptr;
  view_mode_t m_view_mode = view_mode_t::VIEW_MODE_MINE;

  // Path of the currently open .d3l, or empty if none / untitled. Updated by
  // onFileOpen / onFileSaveAs and cleared by onFileNew.
  QString m_currentLevelFile;
};

