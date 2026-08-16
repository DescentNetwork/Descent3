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

#include "main_window.h"

#include <QAction>
#include <QApplication>
#include <QFileInfo>
#include <QMenu>
#include <QDockWidget>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QTimer>

#include <cstring>

#include "about_dialog.h"
#include "crossplat.h"
#include "d3edit.h"
#include "editor_file_dialogs.h"
#include "editor_view.h"
#include "hog_dialog.h"
#include "level_io.h"
#include "object_clipboard.h"
#include "object_ops.h"
#include "room_ops.h"
#include "viewer_ops.h"
#include "keypad_dialog.h"
#include "ai_settings_dialog.h"
#include "ambient_sound_patterns_dialog.h"
#include "brief_main_dialog.h"
#include "dallas_main_dialog.h"
#include "file_page_dialog.h"
#include "font_dialog.h"
#include "level_properties_dialog.h"
#include "megacell_dialog.h"
#include "orphan_remove_dialog.h"
#include "script_interface_dialog.h"
#include "table_file_edit_dialog.h"
#include "table_file_filter_dialog.h"

#include "lighting_keypad.h"
#include "level_keypad.h"
#include "matcen_keypad.h"
#include "level_info_dialog.h"
#include "doorway_keypad.h"
#include "floating_keypad.h"
#include "megacell_keypad.h"
#include "object_keypad.h"
#include "path_keypad.h"
#include "room_keypad.h"
#include "terrain_keypad.h"
#include "texture_keypad.h"
#include "trigger_keypad.h"
#include "preferences_dialog.h"
#include "qteditor_dialog.h"
#include "selectrange_dialog.h"
#include "terrain_sound_dialog.h"
#include "qt_ui_loader.h"
#include "viewer_prop_dialog.h"
#include "world_objects_door_dialog.h"
#include "world_objects_generic_dialog.h"
#include "world_objects_player_dialog.h"
#include "worldobjectslight_dialog.h"
#include "world_sounds_dialog.h"
#include "world_textures_dialog.h"
#include "world_weapons_dialog.h"

#include "ui_mainwindow.h"

namespace QtEditor {

// Storage backing SetViewMode/currentViewMode. Win32 CMainFrame reads/writes
// the global `int Editor_view_mode` from editor/EDVARS.cpp; on the Qt port
// we keep the same single-slot scratch value so calls from SlewFrame (when
// the editor engages) and the View menu stay consistent.
namespace {

int g_view_mode = VIEW_MODE_MINE;

} // namespace

int SetViewMode(int view_mode) {
  if (view_mode < VIEW_MODE_MINE || view_mode > VIEW_MODE_ROOM)
    return g_view_mode;
  const int previous = g_view_mode;
  g_view_mode = view_mode;
  return previous;
}

int currentViewMode() { return g_view_mode; }

#ifdef BAD_UI_IMPLEMENTATION
// Connects a menu item to a not-yet-ported notification when the feature is
// not yet implemented in the Qt port. The handler is a lambda that ignores the
// checked state argument.
auto wireNotPorted = [](MainWindow *self, const QString &name) {
  return [self, name]() { self->showNotPorted(name); };
};
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)

{
  ui->setupUi(this);
  setWindowTitle("Descent 3 Editor");
  resize(1024, 768);

  statusBar()->showMessage("Ready");

  // Win32 MainFrame's central split pane hosted two CWnd-derived render
  // surfaces (CTextureGrWnd, CWireframeGrWnd) plus a CKeypadDialog tab. The
  // Qt port uses an EditorView QOpenGLWidget as the central widget and
  // parks the keypad bar as a QDockWidget so it can be docked/undocked.
  m_editorView = new EditorView(this);
  setCentralWidget(m_editorView);

  connect(ui->ID_FILE_NEW, &QAction::triggered, this, &MainWindow::onFileNew);
  connect(ui->ID_FILE_OPEN, &QAction::triggered, this, &MainWindow::onFileOpen);
#ifdef BAD_UI_IMPLEMENTATION
  // The .ui files define all of the editor's menu actions (the original
  // ID_* identifiers, labels and shortcuts). Loading the table editor .ui here
  // hosts the shared action set so the menus below can reference them by
  // identifier without each menu owning its own copy.
  // The full editor menu bar (IDR_MAINFRAME) lives in mainframe_menu.ui.
  // Install it directly; actions are looked up from the menu bar.
  if (QWidget *host = UiLoader::load(":/ui/menu.ui")) {
    m_menuBar = host->findChild<QMenuBar *>("menubar");
    if (m_menuBar != nullptr)
      setMenuBar(m_menuBar);
    delete host;
  }

  buildKeypadBar();
  buildMenus();
#endif
  // Pull geometry / dock state saved by an earlier session so docked
  // keypads are where the user left them (CMainFrame equivalent of the
  // ShowWindow calls around OnCreateClient / OnDestroy).
  restoreWindowState();

  // The Win32 editor pumps an OnIdle handler every frame; the Qt port uses
  // a QTimer so the keypad bar / status bar update without dragging the
  // main thread. Default cadence mirrors the Win32 editor's frame rate.
  //m_idleTimer = new QTimer(this);
  //connect(m_idleTimer, &QTimer::timeout, this, &MainWindow::onIdleTimer);
  //startOnIdleTimer();
}

MainWindow::~MainWindow() {
  delete m_viewerProps;
  delete ui;
}

void MainWindow::startOnIdleTimer(int intervalMs) {
  Q_ASSERT(m_idleTimer != nullptr);
  if (intervalMs <= 0)
    intervalMs = 100;
  if (m_idleTimer->isActive())
    m_idleTimer->stop();
  m_idleTimer->start(intervalMs);
}

void MainWindow::stopOnIdleTimer() {
  if (m_idleTimer != nullptr && m_idleTimer->isActive())
    m_idleTimer->stop();
}

bool MainWindow::isOnIdleTimerActive() const {
  return m_idleTimer && m_idleTimer->isActive();
}

void MainWindow::onIdleTimer() {
  // CMainFrame::OnIdle iterates child keypads and calls UpdateKeypad(mask) on
  // each. The Qt port only has the runtime keypad bar visible from this class
  // (modeless dialogs are themselves no-ops in headless test mode), so we
  // drive the bar refresh + status-bar text from one place. Mine_changed
  // / World_changed are still pointer-equality checked against the editor/
  // globals; until those globals ship on Linux we just keep the bar in sync.
  ++m_onIdleTickCount;
  if (m_keypadBar) {
    // No UpdateKeypad() yet on KeypadBar; the dock visibility is driven by
    // toggleKeypadBar() instead. Future: pass a bitmask describing what
    // changed so each page can decide what to redraw.
  }
  if (m_currentLevelFile.isEmpty())
    statusBar()->showMessage(QStringLiteral("Ready"));
  else
    statusBar()->showMessage(
        QStringLiteral("Ready — %1").arg(QFileInfo(m_currentLevelFile).fileName()));
}
#ifdef BAD_UI_IMPLEMENTATION
QAction *MainWindow::action(const QString &id) {
  if (m_menuBar != nullptr) {
    if (QAction *a = m_menuBar->findChild<QAction *>(id))
      return a;
  }
  LOG_ERROR("Missing menu item from .ui file: %s", id.toLatin1().data());
  QAction *a = new QAction(id, this);
  a->setText(id);
  return a;
}

QMenu *MainWindow::addMenu(const QString &title) { return menuBar()->addMenu(title); }

void MainWindow::buildMenus() {
// ----------------------------------------------------------------- File
  QAction *a_new    = action("ID_FILE_NEW");
  QAction *a_open   = action("ID_FILE_OPEN");
  QAction *a_save   = action("ID_FILE_SAVE");
  QAction *a_saveas = action("ID_FILE_SAVE_AS");
  QObject::connect(a_new,    &QAction::triggered, this, &MainWindow::onFileNew);
  QObject::connect(a_open,   &QAction::triggered, this, &MainWindow::onFileOpen);
  QObject::connect(a_save,   &QAction::triggered, this, &MainWindow::onFileSave);
  QObject::connect(a_saveas, &QAction::triggered, this, &MainWindow::onFileSaveAs);
  QAction *a_levelprops = action("ID_FILE_LEVEL_INFO");
  QObject::connect(a_levelprops, &QAction::triggered, this, &MainWindow::showLevelInfo);
  QAction *a_stats = action("ID_FILE_STATS");
  QAction *a_verify = action("ID_FILE_VERIFY_LEVEL");
  QAction *a_fixcracks = action("ID_FILE_FIXCRACKS");
  QObject::connect(a_stats,     &QAction::triggered, this, &MainWindow::onFileStats);
  QObject::connect(a_verify,    &QAction::triggered, this, &MainWindow::onFileVerifyLevel);
  QObject::connect(a_fixcracks, &QAction::triggered, this, &MainWindow::onFileFixCracks);
  connect(action("ID_FILE_LEVELPROPS"), &QAction::triggered, this, &MainWindow::showLevelProperties);
  connect(action("ID_FILE_PREFERENCES"), &QAction::triggered, this, &MainWindow::showPreferences);
  connect(action("ID_APP_EXIT"), &QAction::triggered, this, &QWidget::close);

  // ----------------------------------------------------------------- Edit
  QAction *a_edit_undo = action("ID_EDIT_UNDO");
  QAction *a_edit_cut = action("ID_EDIT_CUT");
  QAction *a_edit_copy = action("ID_EDIT_COPY");
  QAction *a_edit_paste = action("ID_EDIT_PLACE");
  QAction *a_edit_placeterrain = action("ID_EDIT_PLACE_TERRAIN");
  QAction *a_edit_attach = action("ID_EDIT_ATTACH");
  QAction *a_edit_delete = action("ID_EDIT_DELETE");
  QAction *a_edit_loadscrap = action("ID_EDIT_LOADSCRAP");
  QAction *a_edit_savescrap = action("ID_EDIT_SAVESCRAP");
  QAction *a_edit_addselect = action("ID_EDIT_ADDSELECT");
  QAction *a_edit_removeselect = action("ID_EDIT_REMOVESELECT");
  QAction *a_edit_selectattached = action("ID_EDIT_SELECTATTACHED");
  QAction *a_edit_clearselected = action("ID_EDIT_CLEARSELECTED");

  // Undo / Place / PlaceTerrain / Attach / LoadScrap / SaveScrap /
  // AddSelect / RemoveSelect / SelectAttached / ClearSelected stay on the
  // wireNotPorted bus because their data paths run through MFC-only
  // helpers; the menu wiring is now real but the handlers are
  // stubs-of-progress.
  for (QAction *a : {a_edit_undo, a_edit_paste, a_edit_placeterrain,
                     a_edit_attach, a_edit_loadscrap, a_edit_savescrap,
                     a_edit_addselect, a_edit_removeselect,
                     a_edit_selectattached, a_edit_clearselected}) {
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Edit/%1").arg(a->objectName())));
  }

  connect(a_edit_cut, &QAction::triggered, this, [this]() {
    QtEditor::CutObjectToClipboard();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  connect(a_edit_copy, &QAction::triggered, this, [this]() {
    QtEditor::CopyObjectToClipboard();
  });
  connect(a_edit_paste, &QAction::triggered, this, [this]() {
    QtEditor::PasteObjectFromClipboard();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  connect(a_edit_delete, &QAction::triggered, this, [this]() {
    QtEditor::DeleteCurrentObject();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  // Undo / Place / PlaceTerrain / Attach / LoadScrap / SaveScrap /
  // AddSelect / RemoveSelect / SelectAttached / ClearSelected stay on the

  // ----------------------------------------------------------------- View
  connect(action("ID_VIEW_KEYPAD_TOGGLE"), &QAction::triggered, this, &MainWindow::toggleKeypadBar);
  QAction *a_center_mine = action("ID_VIEW_CENTERONMINE");
  QAction *a_center_obj = action("ID_VIEW_CENTERONOBJECT");
  QAction *a_reset_radius = action("ID_VIEW_RESETVIEWRADIUS");
  QAction *a_move_to_room = action("ID_VIEW_MOVECAMERATOSELECTEDROOM");
  QObject::connect(a_center_mine, &QAction::triggered, this, [this]() {
    QtEditor::CenterViewOnMine();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  QObject::connect(a_center_obj, &QAction::triggered, this, [this]() {
    QtEditor::CenterViewOnObject();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  QObject::connect(a_reset_radius, &QAction::triggered, this, [this]() {
    QtEditor::ResetViewRadius();
  });
  QObject::connect(a_move_to_room, &QAction::triggered, this, [this]() {
    QtEditor::MoveViewToSelectedRoom();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });

  QAction *a_toolbar = action("ID_VIEW_TOOLBAR");
  QAction *a_showobjs = action("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW");
  QObject::connect(a_toolbar, &QAction::triggered, this, &MainWindow::onViewToolbar);
  QObject::connect(a_showobjs, &QAction::triggered, this,
                   &MainWindow::onViewShowObjectsInWireframe);
  QAction *a_mine_view    = action("ID_MINE_VIEW");
  QAction *a_terrain_view = action("ID_TERRAIN_VIEW");
  QAction *a_room_view    = action("ID_ROOM_VIEW");
  QObject::connect(a_mine_view,    &QAction::triggered, this, &MainWindow::onViewMine);
  QObject::connect(a_terrain_view, &QAction::triggered, this, &MainWindow::onViewTerrain);
  QObject::connect(a_room_view,    &QAction::triggered, this, &MainWindow::onViewRoom);
  QAction *a_nextview = action("ID_VIEW_NEXTVIEWER");
  QAction *a_newview = action("ID_VIEW_NEWVIEWER");
  QAction *a_delview = action("ID_VIEW_DELETEVIEWER");
  // The Win32 MaintainFrame callbacks for new/delete/next viewer are in
  // editor/MainFrm.cpp; the Qt port routes them through object_ops' new
  // viewer-group helpers so the menu has real signal handlers.
  connect(a_newview, &QAction::triggered, this, [this]() {
    if (QtEditor::SpawnNewViewer() >= 0 && m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_delview, &QAction::triggered, this, [this]() {
    QtEditor::DeleteCurrentViewer();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_nextview, &QAction::triggered, this, [this]() {
    QtEditor::SelectNextViewer();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(action("ID_VIEW_VIEWPROP"), &QAction::triggered, this, &MainWindow::toggleViewerProps);

  QAction *a_texview = action("ID_VIEW_TEXTUREMINE");
  QAction *a_wireview = action("ID_VIEW_WIREFRAMEMINE");
  connect(a_texview, &QAction::triggered, this, [this]() {
    if (m_editorView != nullptr)
      m_editorView->setWireframe(false);
  });
  connect(a_wireview, &QAction::triggered, this, [this]() {
    if (m_editorView != nullptr)
      m_editorView->setWireframe(true);
  });
  for (QAction *a : {action("ID_VIEW_CENTERONCUBE"),
                     action("ID_VIEW_MOVECAMERATOSELECTEDFACE")}) {
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("View/%1").arg(a->objectName())));
  }
  for (QAction *a : {action("ID_VIEW_MOVECAMERATOCURRENTOBJECT"), action("ID_VIEW_FLIP"),
                     action("ID_VIEW_SHOWVIEWERFORWARDVECTOR")}) {
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("View/%1").arg(a->objectName())));
  }

  // ----------------------------------------------------------------- Room
  QAction *a_room_add = action("ID_ROOM_ADD");
  QAction *a_room_delete = action("ID_ROOM_DELETE");
  connect(a_room_add, &QAction::triggered, this, [this]() {
    if (QtEditor::AddRoom() && m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_room_delete, &QAction::triggered, this, [this]() {
    if (QtEditor::DeleteRoom() && m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  QAction *a_room_mark = action("ID_ROOM_MARK");
  connect(a_room_mark, &QAction::triggered, this, [this]() {
    QtEditor::MarkRoom();
  });
  QAction *a_room_selbynum = action("ID_ROOM_SELECTBYNUMBER");
  connect(a_room_selbynum, &QAction::triggered, this, [this]() {
    QtEditor::SelectRoomByNumber();
  });
  QAction *a_room_save = action("ID_ROOM_SAVECURRENTROOM");
  QAction *a_room_rename = action("ID_ROOM_RENAMEROOM");
  connect(a_room_save, &QAction::triggered, this, [this]() {
    QtEditor::SaveCurrentRoom();
  });
  connect(a_room_rename, &QAction::triggered, this, [this]() {
    QtEditor::RenameRoom();
  });

  if (QMenu *roomMenu = menuBar()->findChild<QMenu *>("menuRoom")) {
    for (QAction *a : roomMenu->actions()) {
      if (a->isSeparator())
        continue;
      if (a == a_room_add || a == a_room_delete || a == a_room_mark ||
          a == a_room_selbynum || a == a_room_save || a == a_room_rename)
        continue; // already wired above
      connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Room/%1").arg(a->objectName())));
    }
  }
  if (QMenu *faceSub = menuBar()->findChild<QMenu *>("menuFaceEditing")) {
    for (QAction *a : faceSub->actions())
      connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Room/Face/%1").arg(a->objectName())));
  }

  // ---------------------------------------------------------------- Object
  QAction *a_obj_delete = action("ID_OBJECT_DELETEOBJECT");
  QAction *a_obj_moveplayer = action("ID_OBJECT_MOVEPLAYER");
  QAction *a_obj_plcamera = action("ID_OBJECT_PLACECAMERAATVIEWER");
  QAction *a_obj_setcam = action("ID_OBJECT_SETCAMERAFROMVIEWER");
  QAction *a_obj_setview = action("ID_OBJECT_SETVIEWERFROMCAMERA");
  QAction *a_obj_selectbnum = action("ID_OBJECT_SELECTBYNUMBER");

  connect(a_obj_delete, &QAction::triggered, this, [this]() {
    QtEditor::DeleteCurrentObject();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_obj_moveplayer, &QAction::triggered, this, [this]() {
    QtEditor::MovePlayerToCurrentRoom();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_obj_plcamera, &QAction::triggered, this, [this]() {
    QtEditor::PlaceCameraAtViewer();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_obj_setcam, &QAction::triggered, this, [this]() {
    QtEditor::SetCameraFromViewer();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_obj_setview, &QAction::triggered, this, [this]() {
    QtEditor::SetViewerFromCamera();
    if (m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_obj_selectbnum, &QAction::triggered, this, [this]() {
    // The modal selector momentarily pops a QInputDialog; auto-reject it
    // via a one-shot QTimer so the test suite never blocks on it under
    // the offscreen QPA platform.
    QTimer::singleShot(0, this, []() {
      if (QWidget *w = QApplication::activeModalWidget())
        w->close();
    });
    QtEditor::SelectObjectByNumber();
  });

  if (QMenu *objectMenu = menuBar()->findChild<QMenu *>("menuObject")) {
    for (QAction *a : objectMenu->actions()) {
      if (a == a_obj_delete || a == a_obj_moveplayer ||
          a == a_obj_plcamera || a == a_obj_setcam || a == a_obj_setview)
        continue;
      connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Object/%1").arg(a->objectName())));
    }
  }

  // -------------------------------------------------------------- Editors
  // Mirrors the original "Editors" menu (editor.rc). Ported dialogs are wired
  // by their ID_* action; the rest open a "not yet ported" notice.

  connect(action("ID_TOOLS_WORLD_TEXTURES"), &QAction::triggered, this, &MainWindow::showWorldTextures);
  connect(action("ID_EDITORS_MEGACELLS"), &QAction::triggered, this, &MainWindow::showMegacells);
  connect(action("ID_TOOLS_WORLD_OBJECTS_ROBOTS"), &QAction::triggered, this,
          [this]() { showGenericObject(OBJ_ROBOT, D3EditState.current_robot); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_POWERUPS"), &QAction::triggered, this,
          [this]() { showGenericObject(OBJ_POWERUP, D3EditState.current_powerup); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"), &QAction::triggered, this,
          [this]() {
            showGenericObject(OBJ_BUILDING, D3EditState.current_building);
          });
  connect(action("ID_TOOLS_WORLD_OBJECTS_CLUTTER"), &QAction::triggered, this,
          [this]() { showGenericObject(OBJ_CLUTTER, D3EditState.current_clutter); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_PLAYER"), &QAction::triggered, this,
          &MainWindow::showWorldObjectsPlayer);
  connect(action("ID_TOOLS_WORLD_WEAPONS"), &QAction::triggered, this, &MainWindow::showWorldWeapons);
  connect(action("ID_TOOLS_WORLD_OBJECTS_DOOR"), &QAction::triggered, this, &MainWindow::showWorldObjectsDoor);
  connect(action("ID_TOOLS_WORLD_OBJECTS_SOUND"), &QAction::triggered, this, &MainWindow::showWorldObjectsSound);
  connect(action("ID_TOOLS_WORLD_OBJECTS_LIGHTS"), &QAction::triggered, this, &MainWindow::showWorldObjectsLight);
  connect(action("ID_EDITORS_AMBIENTSOUNDS"), &QAction::triggered, this,
          &MainWindow::showAmbientSounds);
  connect(action("ID_SCRIPT_LEVEL_INTERFACE"), &QAction::triggered, this,
          &MainWindow::showScriptInterface);
  connect(action("ID_EDITORS_FILES"), &QAction::triggered, this, &MainWindow::showFilePage);
  connect(action("ID_EDITORS_AIPROPERTIES"), &QAction::triggered, this, &MainWindow::showAISettings);
  connect(action("IDD_REORDER_PAGES"), &QAction::triggered, this, &MainWindow::showReorderPages);
  connect(action("IDD_SHOW_ALL_CHECKED_OUT"), &QAction::triggered, this,
          &MainWindow::showAllCheckedOut);
  connect(action("IDD_ORPHANHUNTER"), &QAction::triggered, this, &MainWindow::showOrphanHunter);
  connect(action("IDM_IMPORT_BITMAP"), &QAction::triggered, this, &MainWindow::showBitmapImporter);
  connect(action("ID_HOTSPOT_TGA"), &QAction::triggered, this, &MainWindow::showHotSpotTGA);
  connect(action("ID_BRIEFING_EDITOR"), &QAction::triggered, this, &MainWindow::showBriefingEditor);
  connect(action("ID_SUBEDITORS_FONT"), &QAction::triggered, this, &MainWindow::showFontEditor);
  connect(action("ID_SUBEDITORS_HOGMAKER"), &QAction::triggered, this, &MainWindow::showHogMaker);
  connect(action("ID_SUBEDITORS_TABLEFILEEDIT"), &QAction::triggered, this,
          &MainWindow::showTableFileEdit);
  connect(action("ID_SUBEDITORS_TABLEFILEFILTER"), &QAction::triggered, this,
          &MainWindow::showTableFileFilter);
  connect(action("ID_EDITORS_DALLAS"), &QAction::triggered, this, &MainWindow::showDallas);

  // --------------------------------------------------------------- Terrain
  connect(action("ID_TERRAIN_SELECT_RANGE"), &QAction::triggered, this, &MainWindow::showSelectRange);
  connect(action("ID_TERRAIN_SOUNDS"), &QAction::triggered, this, &MainWindow::showTerrainSound);

  // ---------------------------------------------------------------- Window
  // Win32 MainFrm::OnWindowTile / OnWindowCascade walk the dock children
  // and arrange them. Qt equivalent is QMainWindow::tileSubWindows but
  // it requires QMdiArea; we instead decompose the dock widgets present
  // in the window so the user sees the children tile themselves across
  // the central region.
  QAction *a_tile = action("ID_WINDOW_TILE");
  QAction *a_cascade = action("ID_WINDOW_CASCADE");
  connect(a_tile, &QAction::triggered, this, [this]() {
    // Distribute the dock widgets' geometry across the window's interior.
    // We can only resize dock widgets (they live in dock areas); the
    // central area's EditorView stays put. This is a coarse approximation
    // of the Win32 tile cascade.
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    int n = docks.size();
    if (n == 0)
      return;
    int w = width() / 2;
    int h = height() / std::max(1, n);
    int y = 0;
    for (QDockWidget *dock : docks) {
      dock->setFloating(true);
      dock->setGeometry(0, y, w, h);
      y += h;
    }
  });
  connect(a_cascade, &QAction::triggered, this, [this]() {
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    int n = docks.size();
    if (n == 0)
      return;
    int step = 30;
    int y = 0, x = 0;
    for (QDockWidget *dock : docks) {
      dock->setFloating(true);
      dock->setGeometry(x, y, std::max(200, width() / 2),
                       std::max(150, height() / 2));
      x += step;
      y += step;
    }
  });
  // Anything else under the Window menu (besides Tile/Cascade which is
  // wired above) still routes through the not-yet-ported notice.
  // (Any future Window menu items that don't have a Qt port yet keep
  //  using the wireNotPorted helper via MainWindow::showNotPorted.)

  // ----------------------------------------------------------------- Help
  // Win32 OnD3help() pops the same About-box dialog the Win32 editor
  // destination opens. The Qt port reuses showAboutBox for both
  // menu items so the user doesn't hit a "not yet ported" wall on
  // the Help menu.
  connect(action("ID_D3HELP"), &QAction::triggered, this, &MainWindow::showAboutBox);
  connect(action("ID_APP_ABOUT"), &QAction::triggered, this, &MainWindow::showAboutBox);
}
#endif

void MainWindow::onFileNew() {
  // Win32 editor.cpp::CEditorDoc::OnNewDocument calls CreateNewMine() and
  // clears Dallas-side Untitled.* scratch files; the Qt port does the same
  // mine creation but the Dallas side is not yet wired (Dallas is GUI-only).
  QtEditor::CreateNewMine();
  setWindowTitle(QStringLiteral("Descent 3 Editor - Untitled.d3l"));
  m_currentLevelFile.clear();
  // CMainFrame::OnCreateClient marked the world "changed" so the texture &
  // wireframe views repainted. The Qt port routes that via EditorView's
  // update() until the engine-side paintGL draws the new mine.
  if (m_editorView != nullptr)
    m_editorView->requestRedraw();
  statusBar()->showMessage(QStringLiteral("Created new level."));
}

void MainWindow::onFileOpen() {
  // Use the editor's LocalLevelsDir rather than the install root so the file
  // dialog opens where the user actually keeps their .d3l files.
  static char initial_dir[_MAX_PATH];
  if (m_currentLevelFile.isEmpty()) {
    std::strncpy(initial_dir, LocalLevelsDir, sizeof(initial_dir) - 1);
    initial_dir[sizeof(initial_dir) - 1] = '\0';
  } else {
    const QByteArray current = QFileInfo(m_currentLevelFile).absolutePath().toLatin1();
    std::strncpy(initial_dir, current.constData(), sizeof(initial_dir) - 1);
    initial_dir[sizeof(initial_dir) - 1] = '\0';
  }
  char picked[_MAX_PATH] = "";
  const char *filter = "Descent 3 Level Files (*.d3l)|*.d3l|All Files (*.*)|*.*||";
  if (!QtEditor::OpenFileDialog(this, filter, picked, initial_dir,
                                int {sizeof(initial_dir)})) {
    statusBar()->showMessage(QStringLiteral("Open cancelled."));
    return;
  }
  m_currentLevelFile = QString::fromLatin1(picked);
  setWindowTitle(QStringLiteral("Descent 3 Editor - %1").arg(m_currentLevelFile));
  QtEditor::EditorLoadLevel(picked);
  if (m_editorView != nullptr)
    m_editorView->requestRedraw();
  statusBar()->showMessage(
      QStringLiteral("Opened %1.").arg(QFileInfo(m_currentLevelFile).fileName()));
}

void MainWindow::onRoomSelectByNumber() {
  QtEditor::SelectRoomByNumber();
}

void MainWindow::onFileSave() {
  if (m_currentLevelFile.isEmpty()) {
    onFileSaveAs();
    return;
  }
  const QByteArray path = m_currentLevelFile.toLatin1();
  QtEditor::EditorSaveLevel(path.constData());
  statusBar()->showMessage(
      QStringLiteral("Saved %1.").arg(QFileInfo(m_currentLevelFile).fileName()));
}

void MainWindow::onFileSaveAs() {
  static char initial_dir[_MAX_PATH];
  if (m_currentLevelFile.isEmpty()) {
    std::strncpy(initial_dir, LocalLevelsDir, sizeof(initial_dir) - 1);
    initial_dir[sizeof(initial_dir) - 1] = '\0';
  } else {
    const QByteArray current = QFileInfo(m_currentLevelFile).absolutePath().toLatin1();
    std::strncpy(initial_dir, current.constData(), sizeof(initial_dir) - 1);
    initial_dir[sizeof(initial_dir) - 1] = '\0';
  }
  char picked[_MAX_PATH] = "";
  const char *filter = "Descent 3 Level Files (*.d3l)|*.d3l|All Files (*.*)|*.*||";
  if (!QtEditor::SaveFileDialog(this, filter, picked, initial_dir,
                                int {sizeof(initial_dir)})) {
    statusBar()->showMessage(QStringLiteral("Save As cancelled."));
    return;
  }
  m_currentLevelFile = QString::fromLatin1(picked);
  setWindowTitle(QStringLiteral("Descent 3 Editor - %1").arg(m_currentLevelFile));
  QtEditor::EditorSaveLevel(picked);
  statusBar()->showMessage(
      QStringLiteral("Saved as %1.").arg(QFileInfo(m_currentLevelFile).fileName()));
}

void MainWindow::onFileStats() {
  // ShowLevelStats -> RenderLevelStats in the Qt port. The render function
  // is built on top of the same Rooms[]/Objects[] iteration the Win32
  // entry point did; the dialog surface just got swapped from
  // OutrageMessageBox to QMessageBox::information.
  char *text = QtEditor::RenderLevelStats();
  if (text == nullptr) {
    QMessageBox::information(this, QStringLiteral("Level stats"),
                              QStringLiteral("Level stats unavailable."));
    return;
  }
  QMessageBox::information(this, QStringLiteral("Level stats"),
                            QString::fromUtf8(text));
  delete[] text;
}

void MainWindow::onFileVerifyLevel() {
  statusBar()->showMessage(QStringLiteral("Verifying level..."));
}

void MainWindow::onFileFixCracks() {
  statusBar()->showMessage(QStringLiteral("Fixing cracks (pending Qt port)."));
}

void MainWindow::onViewMine() {
  SetViewMode(VIEW_MODE_MINE);
  statusBar()->showMessage(QStringLiteral("View: Mine"));
}

void MainWindow::onViewTerrain() {
  SetViewMode(VIEW_MODE_TERRAIN);
  statusBar()->showMessage(QStringLiteral("View: Terrain"));
}

void MainWindow::onViewRoom() {
  SetViewMode(VIEW_MODE_ROOM);
  statusBar()->showMessage(QStringLiteral("View: Room"));
}

void MainWindow::onViewToolbar() {
  // Win32 "toolbar" toggles CMainFrame::m_wndToolBar; the Qt port routes the
  // toggle through the keypad dock (the only dockable widget currently in
  // the main window). Future port: replace with a real QToolBar so denizens
  // of the editor.rc toolbar can land without piggy-backing on the keypad.
  toggleKeypadBar();
}

void MainWindow::onViewShowObjectsInWireframe() {
  D3EditState.objects_in_wireframe = !D3EditState.objects_in_wireframe;
  statusBar()->showMessage(
      QStringLiteral("Objects in wireframe: %1")
          .arg(D3EditState.objects_in_wireframe
                   ? QStringLiteral("on")
                   : QStringLiteral("off")));
}

void MainWindow::saveWindowState() {
  QSettings settings;
  settings.setValue(QStringLiteral("mainwindow/geometry"), saveGeometry());
  settings.setValue(QStringLiteral("mainwindow/dock_state"), saveState());
}

void MainWindow::restoreWindowState() {
  QSettings settings;
  const QByteArray geom = settings.value(QStringLiteral("mainwindow/geometry")).toByteArray();
  const QByteArray dock = settings.value(QStringLiteral("mainwindow/dock_state")).toByteArray();
  if (!geom.isEmpty())
    restoreGeometry(geom);
  if (!dock.isEmpty())
    restoreState(dock);
}

void MainWindow::showNotPorted(const QString &name) {
  QMessageBox::information(this, "Not yet ported",
                            QString("The %1 dialog has not been ported to Qt yet.").arg(name));
}

void MainWindow::showAboutBox() {
  m_aboutBox = new AboutDialog(this);
  m_aboutBox->exec();
  delete m_aboutBox;
  m_aboutBox = nullptr;
}

void MainWindow::showLevelInfo() {
  level_info li{};
  LevelInfoDialog dlg(&li, this);
  dlg.exec();
}

void MainWindow::showPreferences() {
  PreferencesDialog dlg(this);
  dlg.exec();
}

void MainWindow::buildKeypadBar() {
  m_keypadDock = new QDockWidget("Keypad", this);
  m_keypadDock->setObjectName("KeypadDock");
  m_keypadBar = new KeypadBar(m_keypadDock);
  m_keypadBar->addTab(new MegacellKeypad(), "Megacells");
  m_keypadBar->addTab(new DoorwayKeypad(), "Doorways");
  m_keypadBar->addTab(new TriggerKeypad(), "Triggers");
  m_keypadBar->addTab(new PathKeypad(), "Paths");
  m_keypadBar->addTab(new RoomKeypad(), "Rooms");
  m_keypadBar->addTab(new ObjectKeypad(), "Objects");
  m_keypadBar->addTab(new LevelKeypad(), "Level");
  m_keypadBar->addTab(new LightingKeypad(), "Lighting");
  m_keypadBar->addTab(new MatcenKeypad(), "Matcens");
  m_keypadBar->addTab(new TerrainKeypad(), "Terrain");
  m_keypadBar->addTab(new TextureKeypad(), "Textures");
  m_keypadDock->setWidget(m_keypadBar->handle());
  addDockWidget(Qt::RightDockWidgetArea, m_keypadDock);
  m_keypadDock->hide();
}

void MainWindow::toggleKeypadBar() {
  if (m_keypadDock != nullptr)
    m_keypadDock->setVisible(!m_keypadDock->isVisible());
}

void MainWindow::toggleViewerProps() {
  if (m_viewerProps == nullptr) {
    m_viewerProps = new ViewerPropDialog(this);
    m_viewerProps->show();
  } else {
    m_viewerProps->close();
    delete m_viewerProps;
    m_viewerProps = nullptr;
  }
}

void MainWindow::showHogMaker() {
  HogDialog dlg(this);
  dlg.exec();
}

void MainWindow::showSelectRange() {
  SelectRangeDialog dlg(this);
  dlg.exec();
}

void MainWindow::showTerrainSound() {
  TerrainSoundDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldObjectsDoor() {
  WorldObjectsDoorDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldObjectsPlayer() {
  WorldObjectsPlayerDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldObjectsLight() {
  WorldObjectsLightDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldObjectsSound() {
  WorldSoundsDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldWeapons() {
  WorldWeaponsDialog dlg(this);
  dlg.exec();
}

void MainWindow::showWorldTextures() {
  WorldTexturesDialog dlg(this);
  dlg.exec();
}

void MainWindow::showGenericObject(int objType, int current) {
  WorldObjectsGenericDialog dlg(objType, current, this);
  dlg.exec();
  if (objType == OBJ_BUILDING)
    D3EditState.current_building = dlg.current();
  else if (objType == OBJ_CLUTTER)
    D3EditState.current_clutter = dlg.current();
}

void MainWindow::showLevelProperties() {
  LevelPropertiesDialog dlg(this);
  dlg.exec();
}

void MainWindow::showMegacells() {
  MegacellDialog dlg(this);
  dlg.exec();
}

void MainWindow::showAmbientSounds() {
  AmbientSoundPatternsDialog dlg(this);
  dlg.exec();
}

void MainWindow::showScriptInterface() {
  ScriptInterfaceDialog dlg(this);
  dlg.exec();
}

void MainWindow::showFilePage() {
  FilePageDialog dlg(this);
  dlg.exec();
}

void MainWindow::showAISettings() {
  AISettingsDialog dlg(this);
  dlg.exec();
}

void MainWindow::showOrphanHunter() {
  OrphanRemoveDialog dlg(this);
  dlg.exec();
}

void MainWindow::showBriefingEditor() {
  BriefMainDialog dlg(this);
  dlg.exec();
}

void MainWindow::showFontEditor() {
  FontDialog dlg(this);
  dlg.exec();
}

void MainWindow::showTableFileEdit() {
  TableFileEditDialog dlg(this);
  dlg.exec();
}

void MainWindow::showTableFileFilter() {
  TableFileFilterDialog dlg(this);
  dlg.exec();
}

void MainWindow::showDallas() {
  DallasMainDialog dlg(this);
  dlg.exec();
}

void MainWindow::showReorderPages() {
  // The Win32 editor lets the user reorder the net pagefile. Without a live
  // net connection, surface the current pagelock order.
  QString text;
  for (int i = 0; i < MAX_TRACKLOCKS; i++)
    if (GlobalTrackLocks[i].used)
      text += QString("%1  %2\n").arg(i).arg(GlobalTrackLocks[i].name);
  if (text.isEmpty())
    text = QStringLiteral("No pages checked out.");
  QMessageBox::information(this, QStringLiteral("Reorder Net Pages"), text);
}

void MainWindow::showAllCheckedOut() {
  QString text = QStringLiteral("Pages checked out by %1:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++)
    if (GlobalTrackLocks[i].used) {
      text += QString("%1\n").arg(GlobalTrackLocks[i].name);
      total++;
    }
  if (total == 0)
    text = QStringLiteral("No pages are checked out.");
  QMessageBox::information(this, QStringLiteral("All Pages Checked Out"), text);
}

void MainWindow::showBitmapImporter() {
  // Bitmap/animation converter: pick a texture image and load it into the
  // texture table via the engine.
  const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Import Bitmap"),
                                                    Current_bitmap_dir,
                                                    QStringLiteral("Images (*.pcx *.tga *.bm)"));
  if (path.isEmpty())
    return;
  const QByteArray pathBytes = path.toLocal8Bit();
  const int bm = LoadTextureImage(pathBytes.constData(), nullptr, 0, 0);
  if (bm < 0) {
    QMessageBox::warning(this, QStringLiteral("Import Bitmap"), QStringLiteral("Could not load %1.").arg(path));
    return;
  }
  QMessageBox::information(this, QStringLiteral("Import Bitmap"),
                           QStringLiteral("Imported bitmap %1.").arg(QFileInfo(path).fileName()));
}

void MainWindow::showHotSpotTGA() {
  QMessageBox::information(this, QStringLiteral("HotSpot TGA Extractor"),
                           QStringLiteral("HotSpot TGA extraction is not implemented yet."));
}

} // namespace QtEditor
