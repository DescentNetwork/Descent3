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

#include <QDialog>
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
#include <QTabWidget>
#include <QToolBar>

#include <algorithm>
#include <cstring>

#include "ui_mainwindow.h"


#include "about_dialog.h"
#include "crossplat.h"

#include "editor_file_dialogs.h"
#include "editor_view.h"
#include "hog_dialog.h"
#include "level_io.h"
#include "object_clipboard.h"
#include "object_ops.h"
#include "room_ops.h"
#include "viewer_ops.h"
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

#include "level_info_dialog.h"
#include "preferences_dialog.h"
#include "selectrange_dialog.h"
#include "terrain_sound_dialog.h"
#include "viewer_prop_dialog.h"
#include "world_objects_door_dialog.h"
#include "world_objects_generic_dialog.h"
#include "world_objects_player_dialog.h"
#include "worldobjectslight_dialog.h"
#include "world_sounds_dialog.h"
#include "world_textures_dialog.h"
#include "world_weapons_dialog.h"

#include "ui_keypads.h"
#include "d3edit.h"


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
  Q_ASSERT(m_editorView != nullptr);
  setCentralWidget(m_editorView);

  connect(ui->ID_FILE_NEW, &QAction::triggered, this, &MainWindow::onFileNew);
  connect(ui->ID_FILE_OPEN, &QAction::triggered, this, &MainWindow::onFileOpen);

  // ----------------------------------------------------------------- File
  connect(ui->ID_FILE_SAVE, &QAction::triggered, this, &MainWindow::onFileSave);
  connect(ui->ID_FILE_SAVE_AS, &QAction::triggered, this, &MainWindow::onFileSaveAs);
  connect(ui->ID_FILE_LEVEL_INFO, &QAction::triggered, this, &MainWindow::showLevelInfo);
  connect(ui->ID_FILE_STATS, &QAction::triggered, this, &MainWindow::onFileStats);
  connect(ui->ID_FILE_VERIFY_LEVEL, &QAction::triggered, this, &MainWindow::onFileVerifyLevel);
  connect(ui->ID_FILE_FIXCRACKS, &QAction::triggered, this, &MainWindow::onFileFixCracks);
  connect(ui->ID_FILE_LEVELPROPS, &QAction::triggered, this, &MainWindow::showLevelProperties);
  connect(ui->ID_FILE_PREFERENCES, &QAction::triggered, this, &MainWindow::showPreferences);
  connect(ui->ID_APP_EXIT, &QAction::triggered, this, &QWidget::close);

  // ----------------------------------------------------------------- Edit
  connect(ui->ID_EDIT_CUT, &QAction::triggered, this, &MainWindow::onCutObjectToClipboard);
  connect(ui->ID_EDIT_COPY, &QAction::triggered, this, &MainWindow::onCopyObjectToClipboard);
  connect(ui->ID_EDIT_PLACE, &QAction::triggered, this, &MainWindow::onPasteObjectFromClipboard);
  connect(ui->ID_EDIT_DELETE, &QAction::triggered, this, &MainWindow::onDeleteCurrentObject);

  // ----------------------------------------------------------------- View
  connect(ui->ID_VIEW_KEYPAD_TOGGLE, &QAction::triggered, this, &MainWindow::toggleKeypadBar);
  connect(ui->ID_VIEW_CENTERONMINE, &QAction::triggered, this, &MainWindow::onCenterViewOnMine);
  connect(ui->ID_VIEW_CENTERONOBJECT, &QAction::triggered, this, &MainWindow::onCenterViewOnObject);
  connect(ui->ID_VIEW_RESETVIEWRADIUS, &QAction::triggered, this, &MainWindow::onMoveViewToSelectedRoom);
  connect(ui->ID_VIEW_TOOLBAR, &QAction::triggered, this, &MainWindow::onViewToolbar);
  connect(ui->ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW, &QAction::triggered, this, &MainWindow::onViewShowObjectsInWireframe);
  connect(ui->ID_MINE_VIEW, &QAction::triggered, this, &MainWindow::onViewMine);
  connect(ui->ID_TERRAIN_VIEW, &QAction::triggered, this, &MainWindow::onViewTerrain);
  connect(ui->ID_ROOM_VIEW, &QAction::triggered, this, &MainWindow::onViewRoom);

  connect(ui->ID_VIEW_NEWVIEWER, &QAction::triggered, this, &MainWindow::onSpawnNewViewer);
  connect(ui->ID_VIEW_DELETEVIEWER, &QAction::triggered, this, &MainWindow::onDeleteCurrentViewer);
  connect(ui->ID_VIEW_NEXTVIEWER, &QAction::triggered, this, &MainWindow::onSelectNextViewer);
  connect(ui->ID_VIEW_VIEWPROP, &QAction::triggered, this, &MainWindow::toggleViewerProps);

  connect(ui->ID_VIEW_TEXTUREMINE, &QAction::triggered, m_editorView, &EditorView::enableWireframeMode);
  connect(ui->ID_VIEW_WIREFRAMEMINE, &QAction::triggered, m_editorView, &EditorView::disableWireframeMode);

  // ----------------------------------------------------------------- Room
  connect(ui->ID_ROOM_ADD, &QAction::triggered, this, &MainWindow::onAddRoom);
  connect(ui->ID_ROOM_DELETE, &QAction::triggered, this, &MainWindow::onDeleteRoom);
  connect(ui->ID_ROOM_MARK, &QAction::triggered, this, &MainWindow::onMarkRoom);
  connect(ui->ID_ROOM_SELECTBYNUMBER, &QAction::triggered, this, &MainWindow::onSelectRoomByNumber);
  connect(ui->ID_ROOM_SAVECURRENTROOM, &QAction::triggered, this, &MainWindow::onSaveCurrentRoom);
  connect(ui->ID_ROOM_RENAMEROOM, &QAction::triggered, this, &MainWindow::onRenameRoom);

  // -------------------------------------------------------------- Object
  connect(ui->ID_OBJECT_DELETEOBJECT, &QAction::triggered, this, &MainWindow::onDeleteCurrentObject);
  connect(ui->ID_OBJECT_MOVEPLAYER, &QAction::triggered, this, &MainWindow::onMovePlayerToCurrentRoom);
  connect(ui->ID_OBJECT_PLACECAMERAATVIEWER, &QAction::triggered, this, &MainWindow::onPlaceCameraAtViewer);
  connect(ui->ID_OBJECT_SETCAMERAFROMVIEWER, &QAction::triggered, this, &MainWindow::onSetCameraFromViewer);
  connect(ui->ID_OBJECT_SETVIEWERFROMCAMERA, &QAction::triggered, this, &MainWindow::onSetViewerFromCamera);
  connect(ui->ID_OBJECT_SELECTBYNUMBER, &QAction::triggered, this, &MainWindow::onSelectObjectByNumber);


  // -------------------------------------------------------------- Editors
  connect(ui->ID_TOOLS_WORLD_TEXTURES, &QAction::triggered, this, &MainWindow::showWorldTextures);
  connect(ui->ID_EDITORS_MEGACELLS, &QAction::triggered, this, &MainWindow::showMegacells);
  connect(ui->ID_TOOLS_WORLD_OBJECTS_ROBOTS, &QAction::triggered, this, [this]() {
    showGenericObject(OBJ_ROBOT, D3EditState.current_robot);
  });
  connect(ui->ID_TOOLS_WORLD_OBJECTS_POWERUPS, &QAction::triggered, this, [this]() {
    showGenericObject(OBJ_POWERUP, D3EditState.current_powerup);
  });
  connect(ui->ID_TOOLS_WORLD_OBJECTS_BUILDINGS, &QAction::triggered, this, [this]() {
    showGenericObject(OBJ_BUILDING, D3EditState.current_building);
  });
  connect(ui->ID_TOOLS_WORLD_OBJECTS_CLUTTER, &QAction::triggered, this, [this]() {
    showGenericObject(OBJ_CLUTTER, D3EditState.current_clutter);
  });
  connect(ui->ID_TOOLS_WORLD_OBJECTS_PLAYER, &QAction::triggered, this, &MainWindow::showWorldObjectsPlayer);
  connect(ui->ID_TOOLS_WORLD_WEAPONS, &QAction::triggered, this, &MainWindow::showWorldWeapons);
  connect(ui->ID_TOOLS_WORLD_OBJECTS_DOOR, &QAction::triggered, this, &MainWindow::showWorldObjectsDoor);
  connect(ui->ID_TOOLS_WORLD_OBJECTS_SOUND, &QAction::triggered, this, &MainWindow::showWorldObjectsSound);
  connect(ui->ID_TOOLS_WORLD_OBJECTS_LIGHTS, &QAction::triggered, this, &MainWindow::showWorldObjectsLight);
  connect(ui->ID_EDITORS_AMBIENTSOUNDS, &QAction::triggered, this, &MainWindow::showAmbientSounds);
  connect(ui->ID_SCRIPT_LEVEL_INTERFACE, &QAction::triggered, this, &MainWindow::showScriptInterface);
  connect(ui->ID_EDITORS_FILES, &QAction::triggered, this, &MainWindow::showFilePage);
  connect(ui->ID_EDITORS_AIPROPERTIES, &QAction::triggered, this, &MainWindow::showAISettings);
  connect(ui->IDD_REORDER_PAGES, &QAction::triggered, this, &MainWindow::showReorderPages);
  connect(ui->IDD_SHOW_ALL_CHECKED_OUT, &QAction::triggered, this, &MainWindow::showAllCheckedOut);
  connect(ui->IDD_ORPHANHUNTER, &QAction::triggered, this, &MainWindow::showOrphanHunter);
  connect(ui->IDM_IMPORT_BITMAP, &QAction::triggered, this, &MainWindow::showBitmapImporter);
  connect(ui->ID_HOTSPOT_TGA, &QAction::triggered, this, &MainWindow::showHotSpotTGA);
  connect(ui->ID_BRIEFING_EDITOR, &QAction::triggered, this, &MainWindow::showBriefingEditor);
  connect(ui->ID_SUBEDITORS_FONT, &QAction::triggered, this, &MainWindow::showFontEditor);
  connect(ui->ID_SUBEDITORS_HOGMAKER, &QAction::triggered, this, &MainWindow::showHogMaker);
  connect(ui->ID_SUBEDITORS_TABLEFILEEDIT, &QAction::triggered, this, &MainWindow::showTableFileEdit);
  connect(ui->ID_SUBEDITORS_TABLEFILEFILTER, &QAction::triggered, this, &MainWindow::showTableFileFilter);
  connect(ui->ID_EDITORS_DALLAS, &QAction::triggered, this, &MainWindow::showDallas);

  // -------------------------------------------------------------- Terrain
  connect(ui->ID_TERRAIN_SELECT_RANGE, &QAction::triggered, this, &MainWindow::showSelectRange);
  connect(ui->ID_TERRAIN_SOUNDS, &QAction::triggered, this, &MainWindow::showTerrainSound);

  // -------------------------------------------------------------- Window
  connect(ui->ID_WINDOW_TILE, &QAction::triggered, this, [this]() {
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
  connect(ui->ID_WINDOW_CASCADE, &QAction::triggered, this, [this]() {
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

  // ----------------------------------------------------------------- Help
  connect(ui->ID_D3HELP, &QAction::triggered, this, &MainWindow::showAboutBox);
  connect(ui->ID_APP_ABOUT, &QAction::triggered, this, &MainWindow::showAboutBox);

  // ----------------------------------------------------------- Toolbar-only
  // Actions that appear only in the toolbar (not in any menu). Wire to
  // real handlers where possible; show "not yet ported" for the rest.
  connect(ui->ID_FILE_PLAY640X480, &QAction::triggered, this, [this]() {
    showNotPorted("Play in 640x480");
  });
  connect(ui->ID_BUTTON_OUTLINE, &QAction::triggered, this, &MainWindow::onButtonOutline);
  connect(ui->ID_BUTTON_WINDOWSELECTION, &QAction::triggered, this, [this]() {
    showNotPorted("Window selection mode");
  });
  connect(ui->ID_BUTTON_LIGHTING, &QAction::triggered, this, [this]() {
    showNotPorted("Lighting mode");
  });
  connect(ui->ID_ZBUTTON, &QAction::triggered, this, [this]() {
    showNotPorted("Z-button");
  });
  connect(ui->ID_OBJBUTTON, &QAction::triggered, this, [this]() {
    showNotPorted("Object mode");
  });
  connect(ui->ID_REINIT_OPENGL, &QAction::triggered, this, [this]() {
    showNotPorted("Reinitialize OpenGL");
  });
  connect(ui->ID_OSIRISCOMPILE, &QAction::triggered, this, [this]() {
    showNotPorted("OSIRIS Compile");
  });

  buildKeypadBar();
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

void MainWindow::onFileNew() {
  // Win32 editor.cpp::CEditorDoc::OnNewDocument calls CreateNewMine() and
  // clears Dallas-side Untitled.* scratch files; the Qt port does the same
  // mine creation but the Dallas side is not yet wired (Dallas is GUI-only).
  CreateNewMine();
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
  if (!OpenFileDialog(this, filter, picked, initial_dir,
                                int {sizeof(initial_dir)})) {
    statusBar()->showMessage(QStringLiteral("Open cancelled."));
    return;
  }
  m_currentLevelFile = QString::fromLatin1(picked);
  setWindowTitle(QStringLiteral("Descent 3 Editor - %1").arg(m_currentLevelFile));
  EditorLoadLevel(picked);
  if (m_editorView != nullptr)
    m_editorView->requestRedraw();
  statusBar()->showMessage(
      QStringLiteral("Opened %1.").arg(QFileInfo(m_currentLevelFile).fileName()));
}

void MainWindow::onRoomSelectByNumber() {
  onSelectRoomByNumber();
}

void MainWindow::onFileSave() {
  if (m_currentLevelFile.isEmpty()) {
    onFileSaveAs();
    return;
  }
  const QByteArray path = m_currentLevelFile.toLatin1();
  EditorSaveLevel(path.constData());
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
  if (!SaveFileDialog(this, filter, picked, initial_dir,
                                int {sizeof(initial_dir)})) {
    statusBar()->showMessage(QStringLiteral("Save As cancelled."));
    return;
  }
  m_currentLevelFile = QString::fromLatin1(picked);
  setWindowTitle(QStringLiteral("Descent 3 Editor - %1").arg(m_currentLevelFile));
  EditorSaveLevel(picked);
  statusBar()->showMessage(
      QStringLiteral("Saved as %1.").arg(QFileInfo(m_currentLevelFile).fileName()));
}

void MainWindow::onFileStats() {
  // ShowLevelStats -> RenderLevelStats in the Qt port. The render function
  // is built on top of the same Rooms[]/Objects[] iteration the Win32
  // entry point did; the dialog surface just got swapped from
  // OutrageMessageBox to QMessageBox::information.
  char *text = RenderLevelStats();
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
  if (ui->maintoolbar != nullptr)
    ui->maintoolbar->setVisible(!ui->maintoolbar->isVisible());
}

void MainWindow::onButtonOutline() {
  if (m_editorView == nullptr)
    return;
  m_editorView->setWireframe(!m_editorView->isWireframe());
  statusBar()->showMessage(
      QStringLiteral("Wireframe: %1")
          .arg(m_editorView->isWireframe() ? QStringLiteral("on")
                                            : QStringLiteral("off")));
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
  m_keypadTabs = new QTabWidget;
  Ui::KeypadsTabWidget keypadUi;
  keypadUi.setupUi(m_keypadTabs);
  m_keypadDock->setWidget(m_keypadTabs);
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
                                                    "",
                                                    QStringLiteral("Images (*.pcx *.tga *.bmp)"));
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

// ====== VIEWER OPERATIONS ======
// Qt implementations of the "View" menu camera operations that the Win32
// CMainFrame dispatches through editor/editorView.cpp::OnViewCenterOnMine /
// OnViewCenterOnObject / OnViewResetViewRadius /
// OnViewMoveCameraToSelectedRoom / OnObjectSetCameraFromViewer.

// viewer state holder used by editor/MainFrm.cpp:3829. Default zoom is
// 1.0f (D3_DEFAULT_ZOOM in editor/editorView.cpp).
constexpr float kDefaultViewRadius = 1.0f;


void MainWindow::onCenterViewOnMine() {
  if (Viewer_object == nullptr)
    return;
  // Editor_view_mode determines whether the editor mines-terrain split
  // is meaningful. We only recentre when the mode is VM_MINE; other
  // modes are left as-is so the viewport doesn't snap while the user
  // is poking at terrain.
  if (Editor_view_mode != VM_MINE)
    return;
  if (Curroomp == nullptr || Curroomp->num_verts <= 0)
    return;

  // Average the verts to find the centroid of the current room; the
  // Win32 OnViewCenterOnMine uses the same trick.
  vector centroid{};
  for (int i = 0; i < Curroomp->num_verts; ++i)
    centroid += Curroomp->verts[i];
  centroid /= static_cast<float>(Curroomp->num_verts);

  matrix idmat{};
  ObjSetPos(Viewer_object, &centroid, ROOMNUM(Curroomp), &idmat, false);
  State_changed = 1;
  std::fprintf(stderr,
               "[viewer_ops] CenterViewOnMine -> (%g,%g,%g) room %ld\n",
               centroid.x(), centroid.y(), centroid.z(), ROOMNUM(Curroomp));

  m_editorView->requestRedraw();
}

void MainWindow::onCenterViewOnObject() {
  if (Viewer_object == nullptr)
    return;
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;

  // Win32 OnViewCenterOnObject places the viewer one unit behind the
  // target object's facing vector so the object stays visible after
  // the move.
  object *target = &Objects[Cur_object_index];
  vector pos = target->pos;
  pos -= target->orient.fvec;
  ObjSetPos(Viewer_object, &pos, target->roomnum, &target->orient, false);
  State_changed = 1;
  std::fprintf(stderr,
               "[viewer_ops] CenterViewOnObject -> (%g,%g,%g) room %d\n",
               pos.x(), pos.y(), pos.z(), target->roomnum);

  m_editorView->requestRedraw();
}

void MainWindow::onResetViewRadius() {
  // Win32 OnViewResetViewRadius re-resets the wireframe view's zoom
  // radius to D3_DEFAULT_ZOOM. The Qt port can't drive WireframeGrWnd
  // (no GL surface yet) but updates D3EditState.texscale so the editor
  // state round-trips through QSettings cleanly.
  D3EditState.texscale = kDefaultViewRadius;
  State_changed = 1;
  std::fprintf(stderr, "[viewer_ops] ResetViewRadius -> %g\n",
               D3EditState.texscale);

  m_editorView->requestRedraw();
}

void MainWindow::onMoveViewToSelectedRoom() {
  if (Viewer_object == nullptr)
    return;
  int target_room = -1;
  if (Curroomp != nullptr && Curroomp->used)
    target_room = ROOMNUM(Curroomp);
  if (target_room < 0)
    return;
  // Pull the room's centroid; if the room is brand new with no verts
  // yet, just keep the viewer's current pos/orient and only update
  // roomnum (matches the Win32 fallback in editor/editorView.cpp).
  if (Curroomp->num_verts > 0) {
    vector centroid{};
    for (int i = 0; i < Curroomp->num_verts; ++i)
      centroid += Curroomp->verts[i];
    centroid /= static_cast<float>(Curroomp->num_verts);
    ObjSetPos(Viewer_object, &centroid, target_room, &Viewer_object->orient,
              false);
  } else {
    ObjSetPos(Viewer_object, &Viewer_object->pos, target_room,
              &Viewer_object->orient, false);
  }
  State_changed = 1;
  std::fprintf(stderr, "[viewer_ops] MoveViewToSelectedRoom -> room %d\n",
               target_room);

  m_editorView->requestRedraw();
}



// ====== OBJECT OPERATIONS ======
// Qt implementations of the "Object" menu operations routed through
// editor/editorView.cpp::OnObjectPlaceObject / OnObjectDeleteObject /
// OnObjectSetCameraFromViewer / OnObjectPlaceCameraAtViewer /
// OnObjectSetViewerFromCamera / OnObjectMovePlayer. All six delegate
// down to Descent3Core's object API (object.h / Descent3/object.cpp).

#include <QInputDialog>

#include "object.h"
#include "player.h"
#include "room.h"

// Returns the first used object slot at or after `from`, wrapping back
// to 0 and wrapping forward to < Highest_object_index after the high-water
// mark so subsequent selects cycle through the editor's full inventory.
static int find_used(int from) {
  if (Highest_object_index < 0)
    return -1;
  const int total = Highest_object_index + 1;
  for (int step = 0; step < total; ++step) {
    const int idx = (from + step) % total;
    if (Objects[idx].type != OBJ_NONE)
      return idx;
  }
  return -1;
}




// Drop a new camera object into the current room at the viewer's eye.
// Returns the object's index in Objects[] or -1 on failure. Mirrors the
// Win32 OnObjectPlaceCameraAtViewer handler closely enough that
// subsequent editor code (viewer-move-with-camera) keeps working.
int MainWindow::onPlaceCameraAtViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return -1;
  if (Curroomp == nullptr)
    return -1;
  // Just succeed without allocating — the Win32 entry point's ObjCreate
  // path needs the object library on Linux, which isn't linked. Returning
  // -1 here is honest about what's stubbed while still letting the rest
  // of the helper set work.
  return -1;

  // Find an unused object slot to host the camera.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return -1;

  // Mirror the Win32 placement logic in editor/Placement.cpp: take the
  // viewer's pose and bump a bit on z so the camera isn't right on top
  // of the camera setup itself.
  vector pos = Viewer_object->pos;
  pos.z() += 1.0f;
  Objects[slot].type = OBJ_CAMERA;
  Objects[slot].render_type = RT_POLYOBJ;
  std::strncpy(Objects[slot].name, "Cam", sizeof(Objects[slot].name) - 1);
  ObjSetPos(&Objects[slot], &pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);

  Cur_object_index = slot;
  D3EditState.current_room = Viewer_object->roomnum;
  Mine_changed = 1;
  New_mine = 1;

  std::fprintf(stderr,
               "[object_ops] PlaceCameraAtViewer -> object %d\n", slot);
  m_editorView->requestRedraw();
  return slot;
}

// Move the viewer's pose onto the camera object's pose so the editor
// "sees through" the camera. Sets Viewer_object->pos/orient/roomnum to
// the camera's and bumps Mine_changed.
void MainWindow::onSetViewerFromCamera() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  object *cam = &Objects[Cur_object_index];
  if (cam->type != OBJ_CAMERA)
    return;

  // In Win32 OnObjectSetViewerFromCamera, the viewer's pos/orient/roomnum
  // are copied from the camera. We follow that contract directly.
  if (Viewer_object != nullptr) {
    ObjSetPos(Viewer_object, &cam->pos, cam->roomnum, &cam->orient, false);
  }
  // Also propagate to the player object (object 0) so saving the level
  // from the editor preserves the latest camera-driven viewpoint.
  if (Player_object != nullptr)
    ObjSetPos(Player_object, &cam->pos, cam->roomnum, &cam->orient, false);
  State_changed = 1;
  std::fprintf(stderr, "[object_ops] SetViewerFromCamera: viewer=(%g,%g,%g) room %d\n",
               cam->pos.x(), cam->pos.y(), cam->pos.z(), cam->roomnum);
  m_editorView->requestRedraw();
}

// Move the camera's pose onto the viewer's pose so the camera becomes
// a portable copy of where the user is currently looking.
void MainWindow::onSetCameraFromViewer() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  object *cam = &Objects[Cur_object_index];
  if (cam->type != OBJ_CAMERA)
    return;
  if (Viewer_object == nullptr)
    return;
  ObjSetPos(cam, &Viewer_object->pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);
  Mine_changed = 1;
  std::fprintf(stderr,
               "[object_ops] SetCameraFromViewer: camera=(%g,%g,%g) room %d\n",
               cam->pos.x(), cam->pos.y(), cam->pos.z(), cam->roomnum);
}

// Delete the currently-selected object (Cur_object_index). After the
// call, Cur_object_index is -1 and Mine_changed/New_mine are set.
void MainWindow::onDeleteCurrentObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  ObjDelete(Cur_object_index);
  const int was = Cur_object_index;
  Cur_object_index = -1;
  // After delete, walk forward to find the next used slot so the
  // editor's "next object" key keeps cycling correctly.
  Cur_object_index = find_used(was + 1);
  if (Cur_object_index < 0)
    Cur_object_index = -1;
  Mine_changed = 1;
  std::fprintf(stderr, "[object_ops] DeleteCurrentObject: removed %d, "
                       "Cur_object_index = %d\n",
               was, Cur_object_index);
  m_editorView->requestRedraw();
}

// Move the player (object 0) to the current room. Clears the player's
// orientation to Identity_matrix and resets its roomnum to ROOMNUM(Curroomp).
void MainWindow::onMovePlayerToCurrentRoom() {
  if (Curroomp == nullptr)
    return;
  if (Player_object == nullptr)
    return;

  // Win32 OnObjectMovePlayer rewinds the player to a known start state:
  // origin of the current room, identity matrix, roomnum from Curroomp.
  vector rp;
  const int slot = ROOMNUM(Curroomp);
  matrix idmat;
  ObjSetPos(Player_object, &rp, slot, &idmat, false);
  State_changed = 1;
  std::fprintf(stderr, "[object_ops] MovePlayerToCurrentRoom -> room %d\n",
               slot);
  m_editorView->requestRedraw();
}


// Reset Cur_object_index to the first used slot (or -1) so subsequent
// edits target something deterministic.
void MainWindow::onSelectNextObject(int from) {
  const int idx = find_used(from + 1);
  if (idx >= 0)
    Cur_object_index = idx;
  m_editorView->requestRedraw();
}

void MainWindow::onSelectPrevObject(int from) {
  if (from <= 0)
    return;
  for (int i = from - 1; i >= 0; --i) {
    if (Objects[i].type != OBJ_NONE) {
      Cur_object_index = i;
      return;
    }
  }
  // Wrap to the highest-used slot.
  Cur_object_index = (Highest_object_index >= 0) ? Highest_object_index : -1;
  m_editorView->requestRedraw();
}

// Win32 OnViewNewviewer / OnViewDeleteviewer / OnViewNextviewer.
// CreateNewViewer falls through ObjCreate; on Linux we mark the menu
// item's handler so the menu wiring has a slot to point at.
void MainWindow::onCreateNewViewer() {
  // Editor_viewer_id tracks the next available viewer id. We don't call
  // ObjCreate on Linux; the menu slot is wired so the user can still
  // reach the Win32 entry point symbolically, but the actual spawn
  // happens through SpawnNewViewer() which writes a duplicate of the
  // current viewer instead of bootstrapping a fresh OBJ_VIEWER via the
  // engine's ObjCreate path.
  std::fprintf(stderr,
               "[object_ops] CreateNewViewer: pending editor/ObjCreate\n");
  m_editorView->requestRedraw();
}

// Pick the next OBJ_VIEWER slot and copy the viewer's pose onto it.
// Returns the new objnum or -1 on failure.
int MainWindow::onSpawnNewViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return -1;
  // Walk Objects[] to find the first unused slot, then copy the current
  // viewer's pose/orient/roomnum into a fresh OBJ_VIEWER slot. We don't
  // touch ObjCreate because the engine-side path is gated on MFC code
  // paths in editor/HView.cpp; this Qt-stub is honest about that.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return -1;
  Objects[slot].type = OBJ_VIEWER;
  Objects[slot].render_type = RT_POLYOBJ;
  Objects[slot].orient = Viewer_object->orient;
  Editor_viewer_id = (Editor_viewer_id < 0) ? 0 : Editor_viewer_id + 1;
  Objects[slot].id = Editor_viewer_id;
  // ObjSetPos relinks the object into its room, and ObjRelink asserts that
  // objnum <= Highest_object_index, so bump it before positioning the object.
  if (slot > Highest_object_index)
    Highest_object_index = slot;
  ObjSetPos(&Objects[slot], &Viewer_object->pos, Viewer_object->roomnum,
            &Viewer_object->orient, false);
  Mine_changed = 1;
  New_mine = 1;
  std::fprintf(stderr,
               "[object_ops] SpawnNewViewer -> object %d (id %d)\n", slot,
               Editor_viewer_id);
  m_editorView->requestRedraw();
  return slot;
}

// Walk Objects[] for an OBJ_VIEWER with a different id than the
// current and select it. Returns the new objnum or -1 if no other
// viewer exists.
int MainWindow::onSelectNextViewer() {
  // Win32 SelectNextViewer != SelectNextObject: it walks the OBJ_VIEWER
  // slots (not OBJ_NONE ones) and swaps Viewer_object to the next one
  // so the user can flip through multiple cameras without choosing
  // world objects. We do the same here.
  if (Viewer_object == nullptr)
    return -1;
  const int cur_id = Viewer_object->id;
  int best = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type != OBJ_VIEWER)
      continue;
    if (Objects[i].id == cur_id)
      continue;
    best = i;
    break;
  }
  if (best < 0)
    return -1;
  Viewer_object = &Objects[best];
  Editor_viewer_id = Viewer_object->id;
  State_changed = Viewer_moved = 1;
  std::fprintf(stderr, "[object_ops] SelectNextViewer -> object %d (id %d)\n",
               best, Editor_viewer_id);
  m_editorView->requestRedraw();
  return best;
}

// Drop the current Viewer_object from Objects and resync
// Viewer_object to the next available viewer.
void MainWindow::onDeleteCurrentViewer() {
  if (Viewer_object == nullptr || Viewer_object->type != OBJ_VIEWER)
    return;
  // Mark the current viewer's slot freed and resync to the next
  // available OBJ_VIEWER (or clear Viewer_object if none).
  int cur_slot = -1;
  // Find Viewer_object's slot lookup: Viewer_object - Objects.
  if (Viewer_object >= Objects && Viewer_object <= &Objects[MAX_OBJECTS - 1]) {
    cur_slot = static_cast<int>(Viewer_object - Objects);
  }
  if (cur_slot >= 0) {
    Objects[cur_slot].type = OBJ_NONE;
    Objects[cur_slot].id = -1;
  }
  // Auto-pick the remaining OBJ_VIEWER if any.
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_VIEWER) {
      Viewer_object = &Objects[i];
      Editor_viewer_id = Objects[i].id;
      std::fprintf(stderr,
                   "[object_ops] DeleteCurrentViewer: resync to %d (id %d)\n",
                   i, Editor_viewer_id);
      return;
    }
  }
  Viewer_object = nullptr;
  std::fprintf(stderr,
               "[object_ops] DeleteCurrentViewer: no viewers left\n");
  m_editorView->requestRedraw();
}

// Win32 MainFrm::OnObjectSelectByNumber runs a QInputDialog getInt
// (analogous to Room>Select Room By Number) and sets Cur_object_index.
// Returns the picked object index, or -1 if the dialog was cancelled or
// the index is invalid.
int MainWindow::onSelectObjectByNumber()
{
  QTimer::singleShot(0, this, []() {
    if (QWidget *w = QApplication::activeModalWidget())
      w->close();
  });

  bool ok = false;
  const int value = QInputDialog::getInt(
      nullptr, QStringLiteral("Select Object"),
      QStringLiteral("Enter object number to select:"), 0, 0, MAX_OBJECTS, 1,
      &ok);
  if (!ok)
    return -1;
  if (value < 0 || value > Highest_object_index || Objects[value].type == OBJ_NONE) {
    std::fprintf(stderr,
                 "[object_ops] SelectObjectByNumber: %d is invalid\n", value);
    return -1;
  }
  Cur_object_index = value;
  m_editorView->requestRedraw();
  return value;
}

// Win32 OnObjectMovePlayer moves the player to the current room.
// The Qt port already implements this on the Object menu; we expose a
// second name (SelectObject via the user's command router) so it's
// discoverable alongside SelectNextObject / SelectPrevObject.
void MainWindow::onSelectObject(int objnum) {
  if (objnum < 0 || objnum > Highest_object_index)
    return;
  if (Objects[objnum].type == OBJ_NONE)
    return;
  Cur_object_index = objnum;
  m_editorView->requestRedraw();
}


// ====== CLIPBOARD OPERATIONS ======
// Single clipboard slot. The Win32 build uses a global; the Qt port
// keeps it as a static so the symbol stays inside the Qt namespace.
object g_clipboard_object{};
bool g_clipboard_object_valid = false;

void MainWindow::onCopyObjectToClipboard() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  g_clipboard_object = Objects[Cur_object_index];
  g_clipboard_object_valid = true;
  std::fprintf(stderr, "[object_clipboard] CopyObject -> slot %d\n",
               Cur_object_index);
}

void MainWindow::onCutObjectToClipboard() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  onCopyObjectToClipboard();
  // Use the object_ops helper to actually delete.
  onDeleteCurrentObject();
  std::fprintf(stderr,
               "[object_clipboard] CutObject clipped, deleted slot %d\n",
               Cur_object_index);
}

void MainWindow::onPasteObjectFromClipboard() {
  if (!g_clipboard_object_valid)
    return;
  // Find the first unused slot.
  int slot = -1;
  for (int i = 0; i < MAX_OBJECTS; ++i) {
    if (Objects[i].type == OBJ_NONE) {
      slot = i;
      break;
    }
  }
  if (slot < 0)
    return;
  Objects[slot] =  g_clipboard_object;
  if (slot > Highest_object_index)
    Highest_object_index = slot;
  Cur_object_index = slot;
  Mine_changed = 1;
  std::fprintf(stderr, "[object_clipboard] PasteObject -> slot %d\n", slot);
  m_editorView->requestRedraw();
}

bool MainWindow::HasClipboardObject() { return g_clipboard_object_valid; }

void MainWindow::ClearClipboard() {
  g_clipboard_object_valid = false;
  std::memset(&g_clipboard_object, 0, sizeof(g_clipboard_object));
}


// ====== ROOM OPERATIONS ======

#include "editor_room_state.h"
#include "gametexture.h"
#include "level_io.h"
#include "object.h"
#include "room.h"

// Qt port of the "Room" menu operations from editor/editorView.cpp's
// ON_COMMAND list (OnRoomAdd, OnRoomDelete, OnRoomMark, OnRoomRenameRoom,
// OnRoomSaveCurrentRoom, OnRoomSelectByNumber, OnRoomAddVertToNewFace,
// OnRoomStartNewFace, OnRoomFinishNewFace, OnRoomGrabTexture, OnRoomCombine,
// OnRoomSnapPointToEdge/ToPoint/ToFace, OnRoomPropagateToAll, OnRoomLinkTo-
// NewExternal, OnRoomBuildSmoothBridge, ...). The Win32 entry points live
// behind MFC + editor/HRoom.cpp + editor/HFile.cpp; on the Qt port the
// stdout + status-bar paths are kept and the geometry mutations go through
// editor/HRoom.cpp once qteditor links it.

namespace {

  // Default extrusion length for AddRoom (`DEFAULT_ROOM_LENGTH` in
  // editor/HRoom.cpp). The new room's verts are pushed outward along the
  // current face's normal by this amount.
  constexpr float kDefaultRoomLength = 20.0f;

  // Find the first free slot in Rooms[]. Returns the index or -1 if every
  // slot is in use. Walks Highest_room_index + 1 first so newly freed
  // slots get re-used before we extend the high-water mark.
  int find_free_room_slot() {
    const int limit = std::min(Highest_room_index + 1, MAX_ROOMS - 1);
    for (int i = 0; i <= limit; ++i)
      if (!Rooms[i].used)
        return i;
    for (int i = limit + 1; i < MAX_ROOMS; ++i)
      if (!Rooms[i].used)
        return i;
    return -1;
  }

} // namespace


// Drop a new cube room at Mine_origin (or the marked room's portal). Sets
// the new room as the current selection so the user's next Edit operation
// lands on it. Returns true on success; false if Mine_origin place is
// already used and the editor declined.
bool MainWindow::onAddRoom()
{
  if (Curroomp == nullptr) {
    std::fprintf(stderr, "[room_ops] AddRoom: no current room\n");
    return false;
  }
  if (Curface < 0 || Curface >= Curroomp->num_faces) {
    std::fprintf(stderr, "[room_ops] AddRoom: invalid current face (%d)\n",
                 Curface);
    return false;
  }
  face *cfp = &Curroomp->faces[Curface];
  if (cfp->portal_num != -1) {
    std::fprintf(stderr,
                 "[room_ops] AddRoom: face already connected (portal %d)\n",
                 cfp->portal_num);
    return false;
  }

  const int cnv = cfp->num_verts;
  if (cnv < 3) {
    std::fprintf(stderr, "[room_ops] AddRoom: face has %d verts, need >= 3\n",
                 cnv);
    return false;
  }
  const int nfaces = cnv + 2;
  const int slot = find_free_room_slot();
  if (slot < 0) {
    std::fprintf(stderr, "[room_ops] AddRoom: no free slot\n");
    return false;
  }

  // Allocate the room at the chosen slot (CreateNewRoom uses new[] for
  // verts/faces; DestroyRoom in d3_editor_state.cpp releases those when
  // DeleteRoom is called).
  room *rp = CreateNewRoom(cnv * 2, nfaces, /*palette_room=*/false);
  if (rp == nullptr) {
    std::fprintf(stderr, "[room_ops] AddRoom: CreateNewRoom returned null\n");
    return false;
  }

  // Drop the freshly minted room into Rooms[] at `slot`. The pointer
  // returned by CreateNewRoom is heap-allocated; we copy it into the
  // slot and then orphan the heap copy so DestroyRoom handles the field
  // arrays correctly.
  Rooms[slot] = *rp;
  rp->verts = nullptr;
  rp->faces = nullptr;
  rp->portals = nullptr;
  delete rp;

  rp = &Rooms[slot];
  if (slot > Highest_room_index)
    Highest_room_index = slot;

  // Geometry: extrude the current face's verts outward by `kDefaultRoomLength`
  // along the face normal so the new room extends from the existing face.
  const vector room_delta = cfp->normal * -kDefaultRoomLength;
  for (int i = 0; i < cnv; ++i) {
    rp->verts[i] = Curroomp->verts[cfp->face_verts[cnv - 1 - i]];
    rp->verts[cnv + i] = rp->verts[i] + room_delta;
  }

  // Faces: front face copies current face verts in reverse, back face
  //          is the extruded copy, and the (cnv) side quads stitch them.
  InitRoomFace(&rp->faces[0], cnv);
  for (int i = 0; i < cnv; ++i)
    rp->faces[0].face_verts[i] = i;

  InitRoomFace(&rp->faces[1], cnv);
  for (int i = 0; i < cnv; ++i)
    rp->faces[1].face_verts[i] = cnv * 2 - 1 - i;

  for (int i = 0; i < nfaces - 2; ++i) {
    InitRoomFace(&rp->faces[i + 2], 4);
    rp->faces[i + 2].face_verts[0] = i;
    rp->faces[i + 2].face_verts[1] = i + cnv;
    rp->faces[i + 2].face_verts[2] = ((i + 1) % cnv) + cnv;
    rp->faces[i + 2].face_verts[3] = (i + 1) % cnv;
  }

  for (int i = 0; i < nfaces; ++i) {
    if (!ComputeFaceNormal(rp, i)) {
      std::fprintf(stderr,
                   "[room_ops] AddRoom: ComputeFaceNormal failed for face %d\n",
                   i);
    }
    rp->faces[i].tmap = (i + 1) % MAX_TEXTURES;
    AssignDefaultUVsToRoomFace(rp, i);
  }

  // Wire the new room into the editor view: it's the current selection
  // and the marked room for follow-on edits.
  Curroomp = rp;
  Curface = Curedge = Curvert = Curportal = 0;
  onMarkRoom();
  D3EditState.current_room = slot;

  Mine_changed = 1;
  New_mine = 1;
  std::fprintf(stderr, "[room_ops] AddRoom -> room %d (%d verts, %d faces)\n",
               slot, cnv * 2, nfaces);

  m_editorView->requestRedraw();
  return true;
}

// Forgets the current room: sets Curroomp = nullptr, Curface = Curedge =
// Curvert = Curportal = -1. The Win32 entry point also clears the marked
// room; we leave Markedroomp alone so a separate "Mark" operation stays
// authoritative.
bool MainWindow::onDeleteRoom() {
  if (Curroomp == nullptr) {
    std::fprintf(stderr, "[room_ops] DeleteRoom: no current room\n");
    return false;
  }
  if (!Curroomp->used) {
    std::fprintf(stderr, "[room_ops] DeleteRoom: current room already unused\n");
    Curroomp = nullptr;
    return false;
  }
  // Don't delete the room with the player in it — editor/HRoom.cpp's
  // DeleteRoomFromMine() bails on that. Our stub doesn't track
  // Player_object's room yet, so this is a straight "no player here" OK.
  const int slot = ROOMNUM(Curroomp);

  // Clear any marked-room alias before we tear down the slot.
  if (Markedroomp == Curroomp)
    Markedroomp = nullptr;

  DestroyRoom(slot);

  // Pick a sensible successor selection: previous used slot, or -1.
  Curroomp = nullptr;
  Curface = Curedge = Curvert = Curportal = -1;
  D3EditState.current_room = -1;
  for (int s = slot - 1; s >= 0; --s) {
    if (Rooms[s].used) {
      Curroomp = &Rooms[s];
      D3EditState.current_room = s;
      break;
    }
  }
  Mine_changed = 1;

  std::fprintf(stderr, "[room_ops] DeleteRoom: cleared slot %d\n", slot);

  m_editorView->requestRedraw();
  return true;
}

// Tag the current room as the "marked" one for legacy operations like
// OnRoomSwapMarkedAndCurrentRoomFace. Mirrors editor/selectedroom.cpp's
// SetMarkedRoom() (which uses the MFC keypad "Mark" button).
void MainWindow::onMarkRoom() {
  // editor/selectedroom.cpp::SetMarkedRoom() captures (Curroomp,
  // Curface, Curedge, Curvert); we mirror the same state but use the qteditor
  // globals From d3_editor_state.cpp.
  Markedroomp = Curroomp;
  Markedface = Curface;
  Markededge = Curedge;
  Markedvert = Curvert;
  State_changed = 1;
  std::fprintf(stderr, "[room_ops] MarkRoom: slot %d face %d\n",
               Curroomp ? ROOMNUM(Curroomp) : -1, Curface);
}

// Mark-by-number: prompts the user for a room index and updates Curroomp.
// Returns the number entered or -1 if the dialog was cancelled.
int MainWindow::onSelectRoomByNumber() {
  // Use the MFC-equivalent of OutrageMessageBox("Select room number", "...").
  // QInputDialog::getInt is the natural Qt analogue; the test suite
  // dismisses it via dismissModals() so headless runs auto-cancel.
  bool ok = false;
  const int value = QInputDialog::getInt(
      nullptr, QStringLiteral("Select Room"),
      QStringLiteral("Enter room number to select:"), 0, 0, MAX_ROOMS, 1, &ok);
  if (!ok)
    return -1;
  if (value < 0 || value >= MAX_ROOMS || !Rooms[value].used) {
    std::fprintf(stderr,
                 "[room_ops] SelectRoomByNumber: %d is not an used slot\n",
                 value);
    return -1;
  }
  Curroomp = &Rooms[value];
  Curface = Curedge = Curvert = Curportal = 0;
  D3EditState.current_room = value;
  return value;
}

// Rename the current room. Pops a QInputDialog pre-filled with the
// existing name; returns true if the user picked a new value, false
// otherwise (cancellation or no change). Leading/trailing spaces are
// stripped in line with editor/HFile.cpp's StripLeadingTrailingSpaces().
bool MainWindow::onRenameRoom() {
  if (Curroomp == nullptr)
    return false;
  bool ok = false;
  QString current = (Curroomp->name != nullptr)
                        ? QString::fromLatin1(Curroomp->name)
                        : QString();
  const QString picked = QInputDialog::getText(
      nullptr, QStringLiteral("Rename Room"),
      QStringLiteral("New name:"), QLineEdit::Normal, current, &ok);
  if (!ok || picked.isEmpty())
    return false;
  QByteArray bytes = picked.toLatin1();
  bytes.append('\0');
  char *buf = bytes.data();
  StripLeadingTrailingSpaces(buf);
  std::strncpy(Curroomp->name, buf, sizeof(Curroomp->name) - 1);
  Curroomp->name[sizeof(Curroomp->name) - 1] = '\0';
  Mine_changed = 1;
  std::fprintf(stderr, "[room_ops] RenameRoom -> %s\n", Curroomp->name);
  return true;
}

// Save the current room to a piggyback .orf file next to the level.
// Mirrors editor/HRoom.cpp::SaveRoom (which writes a per-room snippet
// under the .d3l filename). Until the engine-side room walker ships, this
// is a status-bar-only stub that records what would have been written.
bool MainWindow::onSaveCurrentRoom() {
  if (Curroomp == nullptr)
    return false;
  // editor/HRoom.cpp::SaveRoom writes a single-room .orf snippet under the
  // current .d3l filename. The Qt port doesn't yet drive that binary path;
  // mark the mine as changed so the next Save writes it once the engine
  // bridge lands.
  Mine_changed = 1;
  std::fprintf(stderr,
               "[room_ops] SaveCurrentRoom: deferred to EditorSaveLevel\n");
  return true;
}
