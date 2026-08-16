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
#include "level_info_dialog.h"
#include "doorway_keypad.h"
#include "megacell_keypad.h"
#include "path_keypad.h"
#include "terrain_keypad.h"
#include "trigger_keypad.h"
#include "preferences_dialog.h"
#include "qteditor_dialog.h"
#include "selectrange_dialog.h"
#include "terrain_sound_dialog.h"
#include "ui_loader.h"
#include "viewer_prop_dialog.h"
#include "world_objects_door_dialog.h"
#include "world_objects_generic_dialog.h"
#include "world_objects_player_dialog.h"
#include "worldobjectslight_dialog.h"
#include "world_sounds_dialog.h"
#include "world_textures_dialog.h"
#include "world_weapons_dialog.h"

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

namespace {

// Forward-declare helper from showNotPorted() so we can use it freely without
// an instance method.
struct MenuEntry {
  const char *id;
  const char *label;
  const char *notPortedName;
};

// Connects a menu item to a not-yet-ported notification when the feature is
// not yet implemented in the Qt port. The handler is a lambda that ignores the
// checked state argument.
auto wireNotPorted = [](MainWindow *self, const QString &name) {
  return [self, name]() { self->showNotPorted(name); };
};

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Descent 3 Editor");
  resize(1024, 768);

  statusBar()->showMessage("Ready");

  // Win32 MainFrame's central split pane hosted two CWnd-derived render
  // surfaces (CTextureGrWnd, CWireframeGrWnd) plus a CKeypadDialog tab. The
  // Qt port uses an EditorView QOpenGLWidget as the central widget and
  // parks the keypad bar as a QDockWidget so it can be docked/undocked.
  m_editorView = new EditorView(this);
  setCentralWidget(m_editorView);

  // The .ui files define all of the editor's menu actions (the original
  // ID_* identifiers, labels and shortcuts). Loading the table editor .ui here
  // hosts the shared action set so the menus below can reference them by
  // identifier without each menu owning its own copy.
  m_actionsHost = UiLoader::load(":/ui/table_file_editor.ui", this);

  buildKeypadBar();
  buildMenus();

  // Pull geometry / dock state saved by an earlier session so docked
  // keypads are where the user left them (CMainFrame equivalent of the
  // ShowWindow calls around OnCreateClient / OnDestroy).
  restoreWindowState();

  // The Win32 editor pumps an OnIdle handler every frame; the Qt port uses
  // a QTimer so the keypad bar / status bar update without dragging the
  // main thread. Default cadence mirrors the Win32 editor's frame rate.
  m_idleTimer = new QTimer(this);
  connect(m_idleTimer, &QTimer::timeout, this, &MainWindow::onIdleTimer);
  startOnIdleTimer();
}

MainWindow::~MainWindow() {
  delete m_viewerProps;
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

QAction *MainWindow::action(const QString &id) {
  if (m_actionsHost != nullptr) {
    if (QAction *a = m_actionsHost->findChild<QAction *>(id))
      return a;
  }
  QAction *a = new QAction(id, this);
  a->setText(id);
  return a;
}

QMenu *MainWindow::addMenu(const QString &title) { return menuBar()->addMenu(title); }

void MainWindow::buildMenus() {
// ----------------------------------------------------------------- File
  QMenu *fileMenu = addMenu("&File");
  QAction *a_new    = action("ID_FILE_NEW");
  QAction *a_open   = action("ID_FILE_OPEN");
  QAction *a_save   = action("ID_FILE_SAVE");
  QAction *a_saveas = action("ID_FILE_SAVE_AS");
  fileMenu->addAction(a_new);
  fileMenu->addAction(a_open);
  fileMenu->addAction(a_save);
  fileMenu->addAction(a_saveas);
  QObject::connect(a_new,    &QAction::triggered, this, &MainWindow::onFileNew);
  QObject::connect(a_open,   &QAction::triggered, this, &MainWindow::onFileOpen);
  QObject::connect(a_save,   &QAction::triggered, this, &MainWindow::onFileSave);
  QObject::connect(a_saveas, &QAction::triggered, this, &MainWindow::onFileSaveAs);
  fileMenu->addSeparator();
  QAction *a_levelprops = action("ID_FILE_LEVEL_INFO");
  fileMenu->addAction(a_levelprops);
  QObject::connect(a_levelprops, &QAction::triggered, this, &MainWindow::showLevelInfo);
  fileMenu->addAction(action("ID_FILE_SAVEGOALTEXT"));
  QAction *a_stats = action("ID_FILE_STATS");
  QAction *a_verify = action("ID_FILE_VERIFY_LEVEL");
  QAction *a_fixcracks = action("ID_FILE_FIXCRACKS");
  fileMenu->addAction(a_stats);
  fileMenu->addAction(a_verify);
  fileMenu->addAction(a_fixcracks);
  QObject::connect(a_stats,     &QAction::triggered, this, &MainWindow::onFileStats);
  QObject::connect(a_verify,    &QAction::triggered, this, &MainWindow::onFileVerifyLevel);
  QObject::connect(a_fixcracks, &QAction::triggered, this, &MainWindow::onFileFixCracks);
  fileMenu->addAction(action("ID_FILE_REMOVEEXTRAPOINTS"));
  fileMenu->addAction(action("ID_FILE_FIXDEGENERATEFACES"));
  fileMenu->addAction(action("ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM"));
  fileMenu->addAction(action("ID_FILE_LEVELPROPS"));
  connect(action("ID_FILE_LEVELPROPS"), &QAction::triggered, this,
          [this]() { showNotPorted("LevelProperties"); });
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_IMPORT_ROOM"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_PLAY640X480"));
  fileMenu->addAction(action("ID_FILE_LEAVE_EDITOR"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_AUTOSAVE"));
  fileMenu->addAction(action("ID_FILE_RESTOREGAMESTATE"));
  fileMenu->addAction(action("ID_FILE_INFORMATION"));
  fileMenu->addAction(action("ID_FILE_CHECKMINE"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_PRINT"));
  fileMenu->addAction(action("ID_FILE_PRINT_PREVIEW"));
  fileMenu->addAction(action("ID_FILE_PRINT_SETUP"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_PREFERENCES"));
  connect(action("ID_FILE_PREFERENCES"), &QAction::triggered, this, &MainWindow::showPreferences);
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_MRU_FILE1"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_APP_EXIT"));
  connect(action("ID_APP_EXIT"), &QAction::triggered, this, &QWidget::close);

  // ----------------------------------------------------------------- Edit
  QMenu *editMenu = addMenu("&Edit");
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
  editMenu->addAction(a_edit_undo);
  editMenu->addSeparator();
  editMenu->addAction(a_edit_cut);
  editMenu->addAction(a_edit_copy);
  editMenu->addAction(a_edit_paste);
  editMenu->addAction(a_edit_placeterrain);
  editMenu->addAction(a_edit_attach);
  editMenu->addAction(a_edit_delete);
  editMenu->addSeparator();
  editMenu->addAction(a_edit_loadscrap);
  editMenu->addAction(a_edit_savescrap);
  editMenu->addSeparator();
  editMenu->addAction(a_edit_addselect);
  editMenu->addAction(a_edit_removeselect);
  editMenu->addAction(a_edit_selectattached);
  editMenu->addAction(a_edit_clearselected);

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

  Q_UNUSED(editMenu);
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
  QMenu *viewMenu = addMenu("&View");
  viewMenu->addAction(action("ID_VIEW_TOOLBAR"));
  viewMenu->addAction(action("ID_VIEW_KEYPAD_TOGGLE"));
  connect(action("ID_VIEW_KEYPAD_TOGGLE"), &QAction::triggered, this, &MainWindow::toggleKeypadBar);
  viewMenu->addAction(action("ID_VIEW_TEXTUREMINE"));
  viewMenu->addAction(action("ID_VIEW_WIREFRAMEMINE"));
  viewMenu->addAction(action("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW"));
  viewMenu->addSeparator();
  viewMenu->addAction(action("ID_VIEW_CENTERONCUBE"));
  viewMenu->addAction(action("ID_VIEW_CENTERONOBJECT"));
  viewMenu->addAction(action("ID_VIEW_CENTERONMINE"));
  viewMenu->addAction(action("ID_VIEW_RESETVIEWRADIUS"));
  viewMenu->addSeparator();
  viewMenu->addAction(action("ID_VIEW_MOVECAMERATOSELECTEDROOM"));
  viewMenu->addAction(action("ID_VIEW_MOVECAMERATOSELECTEDFACE"));
  viewMenu->addAction(action("ID_VIEW_MOVECAMERATOCURRENTOBJECT"));
  viewMenu->addAction(action("ID_VIEW_FLIP"));
  viewMenu->addSeparator();
  viewMenu->addAction(action("ID_VIEW_SHOWVIEWERFORWARDVECTOR"));
  QAction *a_center_mine = action("ID_VIEW_CENTERONMINE");
  QAction *a_center_obj = action("ID_VIEW_CENTERONOBJECT");
  QAction *a_reset_radius = action("ID_VIEW_RESETVIEWRADIUS");
  QAction *a_move_to_room = action("ID_VIEW_MOVECAMERATOSELECTEDROOM");
  viewMenu->addAction(a_center_mine);
  viewMenu->addAction(a_center_obj);
  QObject::connect(a_center_mine, &QAction::triggered, this, [this]() {
    QtEditor::CenterViewOnMine();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  QObject::connect(a_center_obj, &QAction::triggered, this, [this]() {
    QtEditor::CenterViewOnObject();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });
  viewMenu->addAction(a_reset_radius);
  QObject::connect(a_reset_radius, &QAction::triggered, this, [this]() {
    QtEditor::ResetViewRadius();
  });
  viewMenu->addAction(a_move_to_room);
  QObject::connect(a_move_to_room, &QAction::triggered, this, [this]() {
    QtEditor::MoveViewToSelectedRoom();
    if (m_editorView != nullptr) m_editorView->requestRedraw();
  });

  viewMenu->addSeparator();
  QAction *a_toolbar = action("ID_VIEW_TOOLBAR");
  QAction *a_showobjs = action("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW");
  viewMenu->addAction(a_toolbar);
  viewMenu->addAction(a_showobjs);
  QObject::connect(a_toolbar, &QAction::triggered, this, &MainWindow::onViewToolbar);
  QObject::connect(a_showobjs, &QAction::triggered, this,
                   &MainWindow::onViewShowObjectsInWireframe);
  viewMenu->addSeparator();
  QAction *a_mine_view    = action("ID_MINE_VIEW");
  QAction *a_terrain_view = action("ID_TERRAIN_VIEW");
  QAction *a_room_view    = action("ID_ROOM_VIEW");
  viewMenu->addAction(a_mine_view);
  viewMenu->addAction(a_terrain_view);
  viewMenu->addAction(a_room_view);
  QObject::connect(a_mine_view,    &QAction::triggered, this, &MainWindow::onViewMine);
  QObject::connect(a_terrain_view, &QAction::triggered, this, &MainWindow::onViewTerrain);
  QObject::connect(a_room_view,    &QAction::triggered, this, &MainWindow::onViewRoom);
  viewMenu->addSeparator();
  QAction *a_nextview = action("ID_VIEW_NEXTVIEWER");
  QAction *a_newview = action("ID_VIEW_NEWVIEWER");
  QAction *a_delview = action("ID_VIEW_DELETEVIEWER");
  viewMenu->addAction(a_nextview);
  viewMenu->addAction(a_newview);
  viewMenu->addAction(a_delview);
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
  viewMenu->addSeparator();
  viewMenu->addAction(action("ID_VIEW_VIEWPROP"));
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
  QMenu *roomMenu = addMenu("&Room");
  QAction *a_room_add = action("ID_ROOM_ADD");
  QAction *a_room_delete = action("ID_ROOM_DELETE");
  roomMenu->addAction(a_room_add);
  roomMenu->addAction(a_room_delete);
  connect(a_room_add, &QAction::triggered, this, [this]() {
    if (QtEditor::AddRoom() && m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  connect(a_room_delete, &QAction::triggered, this, [this]() {
    if (QtEditor::DeleteRoom() && m_editorView != nullptr)
      m_editorView->requestRedraw();
  });
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_DELETEFACE"));
  roomMenu->addAction(action("ID_ROOM_DELETEPORTAL"));
  roomMenu->addAction(action("ID_ROOM_DELETEVERT"));
  roomMenu->addAction(action("ID_ROOM_DELETECONNECTEDFACES"));
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_PLACEROOM"));
  roomMenu->addAction(action("ID_ROOM_PLACETERRAINROOM"));
  roomMenu->addAction(action("ID_ROOM_ROTATEPLACEDROOM45DEGREES"));
  roomMenu->addAction(action("ID_ROOM_SNAPPLACEDROOM"));
  roomMenu->addAction(action("ID_ROOM_ATTACHROOM"));
  roomMenu->addAction(action("ID_ROOM_UNPLACEROOM"));
  roomMenu->addAction(action("ID_ROOM_DROPROOM"));
  roomMenu->addSeparator();
  QAction *a_room_mark = action("ID_ROOM_MARK");
  roomMenu->addAction(a_room_mark);
  connect(a_room_mark, &QAction::triggered, this, [this]() {
    QtEditor::MarkRoom();
  });
  roomMenu->addAction(action("ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE"));
  roomMenu->addSeparator();
  QAction *a_room_selbynum = action("ID_ROOM_SELECTBYNUMBER");
  roomMenu->addAction(a_room_selbynum);
  connect(a_room_selbynum, &QAction::triggered, this, [this]() {
    QtEditor::SelectRoomByNumber();
  });
  roomMenu->addSeparator();
  QAction *a_room_save = action("ID_ROOM_SAVECURRENTROOM");
  QAction *a_room_rename = action("ID_ROOM_RENAMEROOM");
  roomMenu->addAction(a_room_save);
  roomMenu->addAction(a_room_rename);
  connect(a_room_save, &QAction::triggered, this, [this]() {
    QtEditor::SaveCurrentRoom();
  });
  connect(a_room_rename, &QAction::triggered, this, [this]() {
    QtEditor::RenameRoom();
  });
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_BUILDBRIDGE"));
  roomMenu->addAction(action("ID_ROOM_BUILDSMOOTHBRIDGE"));
  roomMenu->addAction(action("ID_ROOM_JOINROOMS"));
  roomMenu->addAction(action("ID_ROOM_JOINROOMSEXACT"));
  roomMenu->addAction(action("ID_ROOM_JOIN_ADJACENT_FACES"));
  roomMenu->addAction(action("ID_ROOM_COMBINE"));
  roomMenu->addAction(action("ID_ROOM_LINKTONEWEXTERNAL"));
  roomMenu->addAction(action("ID_ROOM_MERGEOBJECTINTOROOM"));
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_STARTNEWFACE"));
  roomMenu->addAction(action("ID_ROOM_ADDVERTTONEWFACE"));
  roomMenu->addAction(action("ID_ROOM_FINISHNEWFACE"));
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_SNAPPOINTTOEDGE"));
  roomMenu->addAction(action("ID_ROOM_SNAPPOINTTOPOINT"));
  roomMenu->addAction(action("ID_ROOM_SNAPPOINTTOFACE"));
  roomMenu->addAction(action("ID_ROOM_UNDOSNAP"));
  QMenu *faceSub = roomMenu->addMenu("Face Editing");
  faceSub->addAction(action("ID_ROOM_FACE_ADDVERTTOEDGE"));
  faceSub->addAction(action("ID_ROOM_FACE_DELETEVERTONEDGE"));
  faceSub->addAction(action("ID_ROOM_FACE_MOVEVERTONEDGE"));
  faceSub->addAction(action("ID_ROOM_FACE_SPLITFACE"));
  faceSub->addAction(action("ID_ROOM_SPLITFACE"));
  roomMenu->addSeparator();
  roomMenu->addAction(action("ID_ROOM_GRABTEXTURE"));
  roomMenu->addAction(action("ID_ROOM_PROPAGATETOALL"));
  roomMenu->addAction(action("ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES"));

  for (QAction *a : roomMenu->actions()) {
    if (a->isSeparator())
      continue;
    if (a == a_room_add || a == a_room_delete || a == a_room_mark ||
        a == a_room_selbynum || a == a_room_save || a == a_room_rename)
      continue; // already wired above
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Room/%1").arg(a->objectName())));
  }
  for (QAction *a : faceSub->actions()) {
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Room/Face/%1").arg(a->objectName())));
  }

  // ---------------------------------------------------------------- Object
  QMenu *objectMenu = addMenu("&Object");
  QAction *a_obj_delete = action("ID_OBJECT_DELETEOBJECT");
  QAction *a_obj_moveplayer = action("ID_OBJECT_MOVEPLAYER");
  QAction *a_obj_plcamera = action("ID_OBJECT_PLACECAMERAATVIEWER");
  QAction *a_obj_setcam = action("ID_OBJECT_SETCAMERAFROMVIEWER");
  QAction *a_obj_setview = action("ID_OBJECT_SETVIEWERFROMCAMERA");
  objectMenu->addAction(action("ID_OBJECT_PLACEOBJECT"));
  objectMenu->addAction(a_obj_delete);
  objectMenu->addAction(a_obj_moveplayer);
  objectMenu->addAction(a_obj_plcamera);
  objectMenu->addAction(action("ID_OBJECT_PLACECAMERAATCURRENTFACE"));
  objectMenu->addAction(a_obj_setcam);
  objectMenu->addAction(a_obj_setview);
  QAction *a_obj_selectbnum = action("ID_OBJECT_SELECTBYNUMBER");
  objectMenu->addAction(a_obj_selectbnum);
  objectMenu->addAction(action("ID_OBJECT_PLACESOUNDSOURCEATVIEWER"));
  objectMenu->addAction(action("ID_OBJECT_PLACEWAYPOINTATVIEWER"));

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

  for (QAction *a : objectMenu->actions()) {
    if (a == a_obj_delete || a == a_obj_moveplayer ||
        a == a_obj_plcamera || a == a_obj_setcam || a == a_obj_setview)
      continue;
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Object/%1").arg(a->objectName())));
  }

  // -------------------------------------------------------------- Editors
  // Mirrors the original "Editors" menu (editor.rc). Ported dialogs are wired
  // by their ID_* action; the rest open a "not yet ported" notice.
  QMenu *editorsMenu = addMenu("Edi&tors");
  editorsMenu->addAction(action("ID_TOOLS_WORLD_TEXTURES"));
  editorsMenu->addAction(action("ID_EDITORS_MEGACELLS"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_ROBOTS"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_POWERUPS"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_CLUTTER"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_PLAYER"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_WEAPONS"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_DOOR"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_SOUND"));
  editorsMenu->addAction(action("ID_TOOLS_WORLD_OBJECTS_LIGHTS"));
  editorsMenu->addAction(action("ID_EDITORS_AMBIENTSOUNDS"));
  editorsMenu->addAction(action("ID_SCRIPT_LEVEL_INTERFACE"));
  editorsMenu->addAction(action("ID_EDITORS_FILES"));
  editorsMenu->addAction(action("ID_EDITORS_AIPROPERTIES"));
  editorsMenu->addSeparator();
  editorsMenu->addAction(action("IDD_REORDER_PAGES"));
  editorsMenu->addAction(action("IDD_SHOW_ALL_CHECKED_OUT"));
  editorsMenu->addAction(action("IDD_ORPHANHUNTER"));
  editorsMenu->addSeparator();
  editorsMenu->addAction(action("IDM_IMPORT_BITMAP"));
  editorsMenu->addAction(action("ID_HOTSPOT_TGA"));
  editorsMenu->addSeparator();
  editorsMenu->addAction(action("ID_BRIEFING_EDITOR"));
  editorsMenu->addAction(action("ID_SUBEDITORS_FONT"));
  editorsMenu->addAction(action("ID_SUBEDITORS_HOGMAKER"));
  editorsMenu->addAction(action("ID_SUBEDITORS_TABLEFILEEDIT"));
  editorsMenu->addAction(action("ID_SUBEDITORS_TABLEFILEFILTER"));
  editorsMenu->addAction(action("ID_EDITORS_DALLAS"));

  connect(action("ID_TOOLS_WORLD_TEXTURES"), &QAction::triggered, this, &MainWindow::showWorldTextures);
  connect(action("ID_EDITORS_MEGACELLS"), &QAction::triggered, this, [this]() { showNotPorted("Megacells"); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_ROBOTS"), &QAction::triggered, this,
          [this]() { showNotPorted("WorldObjectsRobot"); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_POWERUPS"), &QAction::triggered, this,
          [this]() { showNotPorted("WorldObjectsPowerup"); });
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
          [this]() { showNotPorted("AmbientSounds"); });
  connect(action("ID_SCRIPT_LEVEL_INTERFACE"), &QAction::triggered, this,
          [this]() { showNotPorted("ScriptLevelInterface"); });
  connect(action("ID_EDITORS_FILES"), &QAction::triggered, this, [this]() { showNotPorted("FilePage"); });
  connect(action("ID_EDITORS_AIPROPERTIES"), &QAction::triggered, this, [this]() { showNotPorted("AISettings"); });
  connect(action("IDD_REORDER_PAGES"), &QAction::triggered, this, [this]() { showNotPorted("ReorderPages"); });
  connect(action("IDD_SHOW_ALL_CHECKED_OUT"), &QAction::triggered, this,
          [this]() { showNotPorted("ShowAllCheckedOut"); });
  connect(action("IDD_ORPHANHUNTER"), &QAction::triggered, this, [this]() { showNotPorted("OrphanRemove"); });
  connect(action("IDM_IMPORT_BITMAP"), &QAction::triggered, this, [this]() { showNotPorted("BitmapImporter"); });
  connect(action("ID_HOTSPOT_TGA"), &QAction::triggered, this, [this]() { showNotPorted("HotSpotTGA"); });
  connect(action("ID_BRIEFING_EDITOR"), &QAction::triggered, this, [this]() { showNotPorted("BriefEdit"); });
  connect(action("ID_SUBEDITORS_FONT"), &QAction::triggered, this, [this]() { showNotPorted("GrFont"); });
  connect(action("ID_SUBEDITORS_HOGMAKER"), &QAction::triggered, this, &MainWindow::showHogMaker);
  connect(action("ID_SUBEDITORS_TABLEFILEEDIT"), &QAction::triggered, this,
          [this]() { showNotPorted("TableFileEdit"); });
  connect(action("ID_SUBEDITORS_TABLEFILEFILTER"), &QAction::triggered, this,
          [this]() { showNotPorted("TableFileFilter"); });
  connect(action("ID_EDITORS_DALLAS"), &QAction::triggered, this, [this]() { showNotPorted("DallasMain"); });

  // --------------------------------------------------------------- Terrain
  QMenu *terrainMenu = addMenu("&Terrain");
  terrainMenu->addAction(action("ID_TERRAIN_SELECT_RANGE"));
  terrainMenu->addAction(action("ID_TERRAIN_SOUNDS"));
  connect(action("ID_TERRAIN_SELECT_RANGE"), &QAction::triggered, this, &MainWindow::showSelectRange);
  connect(action("ID_TERRAIN_SOUNDS"), &QAction::triggered, this, &MainWindow::showTerrainSound);

  // ---------------------------------------------------------------- Window
  QMenu *windowMenu = addMenu("&Window");
  // Win32 MainFrm::OnWindowTile / OnWindowCascade walk the dock children
  // and arrange them. Qt equivalent is QMainWindow::tileSubWindows but
  // it requires QMdiArea; we instead decompose the dock widgets present
  // in the window so the user sees the children tile themselves across
  // the central region.
  QAction *a_tile = action("ID_WINDOW_TILE");
  QAction *a_cascade = action("ID_WINDOW_CASCADE");
  windowMenu->addAction(a_tile);
  windowMenu->addAction(a_cascade);
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
  QMenu *helpMenu = addMenu("&Help");
  helpMenu->addAction(action("ID_D3HELP"));
  helpMenu->addAction(action("ID_APP_ABOUT"));
  // Win32 OnD3help() pops the same About-box dialog the Win32 editor
  // destination opens. The Qt port reuses showAboutBox for both
  // menu items so the user doesn't hit a "not yet ported" wall on
  // the Help menu.
  connect(action("ID_D3HELP"), &QAction::triggered, this, &MainWindow::showAboutBox);
  connect(action("ID_APP_ABOUT"), &QAction::triggered, this, &MainWindow::showAboutBox);

  // ------------------------------------------------------------------ Test
  // The legacy Win32 editor reserved a Test menu for in-development features.
  // The Qt port keeps it as an explicit placeholders for diagnostics wired
  // up later.
  QMenu *testMenu = addMenu("&Test");
  testMenu->addAction(action("ID_TEST_TEST1"));
  testMenu->addAction(action("ID_TEST_TEST2"));
  testMenu->addAction(action("ID_TEST_TEST3"));
  for (auto *a : {action("ID_TEST_TEST1"), action("ID_TEST_TEST2"), action("ID_TEST_TEST3")}) {
    connect(a, &QAction::triggered, this, wireNotPorted(this, QString("Test/%1").arg(a->objectName())));
  }
}

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
  m_keypadBar->addTab(new MegacellKeypad(m_keypadBar->handle()), "Megacells");
  m_keypadBar->addTab(new DoorwayKeypad(m_keypadBar->handle()), "Doorways");
  m_keypadBar->addTab(new TriggerKeypad(m_keypadBar->handle()), "Triggers");
  m_keypadBar->addTab(new PathKeypad(m_keypadBar->handle()), "Paths");
  m_keypadBar->addTab(new TerrainKeypad(m_keypadBar->handle()), "Terrain");
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
    m_viewerProps->dialog()->close();
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

} // namespace QtEditor
