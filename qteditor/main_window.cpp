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
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTimer>

#include "about_dialog.h"
#include "d3edit.h"
#include "hog_dialog.h"
#include "level_info_dialog.h"
#include "preferences_dialog.h"
#include "qteditor_dialog.h"
#include "selectrange_dialog.h"
#include "splash_dialog.h"
#include "terrain_sound_dialog.h"
#include "ui_loader.h"
#include "viewer_prop_dialog.h"
#include "world_objects_door_dialog.h"
#include "world_objects_generic_dialog.h"
#include "world_objects_player_dialog.h"
#include "worldobjectslight_dialog.h"
#include "world_sounds_dialog.h"
#include "world_weapons_dialog.h"

namespace QtEditor {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Descent 3 Editor");
  resize(1024, 768);

  statusBar()->showMessage("Ready");

  // The .ui files define all of the editor's menu actions (the original
  // ID_* identifiers, labels and shortcuts). Load one of them to own those
  // actions so the menus below can reference them by identifier.
  m_actionsHost = UiLoader::load(":/ui/table_file_editor.ui", this);

  buildMenus();
  showSplash();
}

MainWindow::~MainWindow() {
  delete m_viewerProps;
  delete m_splash;
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
  // ------------------------------------------------------------------ File
  QMenu *fileMenu = addMenu("&File");
  fileMenu->addAction(action("ID_FILE_LEVEL_INFO"));
  fileMenu->addAction(action("ID_FILE_LEVELPROPS"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_FILE_PREFERENCES"));
  fileMenu->addSeparator();
  fileMenu->addAction(action("ID_APP_EXIT"));

  connect(action("ID_FILE_LEVEL_INFO"), &QAction::triggered, this, &MainWindow::showLevelInfo);
  connect(action("ID_FILE_PREFERENCES"), &QAction::triggered, this, &MainWindow::showPreferences);
  connect(action("ID_APP_EXIT"), &QAction::triggered, this, &QWidget::close);
  connect(action("ID_FILE_LEVELPROPS"), &QAction::triggered, this,
          [this]() { showNotPorted("LevelProperties"); });

  // ------------------------------------------------------------------ View
  QMenu *viewMenu = addMenu("&View");
  viewMenu->addAction(action("ID_VIEW_VIEWPROP"));
  viewMenu->addSeparator();
  viewMenu->addAction(action("ID_VIEW_NEWVIEWER"));
  viewMenu->addAction(action("ID_VIEW_DELETEVIEWER"));

  connect(action("ID_VIEW_VIEWPROP"), &QAction::triggered, this, &MainWindow::toggleViewerProps);
  connect(action("ID_VIEW_NEWVIEWER"), &QAction::triggered, this, [this]() { showNotPorted("NewViewer"); });
  connect(action("ID_VIEW_DELETEVIEWER"), &QAction::triggered, this, [this]() { showNotPorted("DeleteViewer"); });

  // --------------------------------------------------------------- Editors
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
  editorsMenu->addAction(action("IDD_ORPHANHUNTER"));
  editorsMenu->addSeparator();
  editorsMenu->addAction(action("ID_BRIEFING_EDITOR"));
  editorsMenu->addAction(action("ID_SUBEDITORS_FONT"));
  editorsMenu->addAction(action("ID_SUBEDITORS_HOGMAKER"));
  editorsMenu->addAction(action("ID_SUBEDITORS_TABLEFILEEDIT"));
  editorsMenu->addAction(action("ID_SUBEDITORS_TABLEFILEFILTER"));
  editorsMenu->addAction(action("ID_EDITORS_DALLAS"));

  connect(action("ID_TOOLS_WORLD_TEXTURES"), &QAction::triggered, this,
          [this]() { showNotPorted("WorldTextures"); });
  connect(action("ID_EDITORS_MEGACELLS"), &QAction::triggered, this, [this]() { showNotPorted("Megacells"); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_ROBOTS"), &QAction::triggered, this,
          [this]() { showNotPorted("WorldObjectsRobot"); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_POWERUPS"), &QAction::triggered, this,
          [this]() { showNotPorted("WorldObjectsPowerup"); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"), &QAction::triggered, this,
          [this]() { showGenericObject(OBJ_BUILDING, D3EditState.current_building); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_CLUTTER"), &QAction::triggered, this,
          [this]() { showGenericObject(OBJ_CLUTTER, D3EditState.current_clutter); });
  connect(action("ID_TOOLS_WORLD_OBJECTS_PLAYER"), &QAction::triggered, this,
          &MainWindow::showWorldObjectsPlayer);
  connect(action("ID_TOOLS_WORLD_WEAPONS"), &QAction::triggered, this, &MainWindow::showWorldWeapons);
  connect(action("ID_TOOLS_WORLD_OBJECTS_DOOR"), &QAction::triggered, this, &MainWindow::showWorldObjectsDoor);
  connect(action("ID_TOOLS_WORLD_OBJECTS_SOUND"), &QAction::triggered, this,
          &MainWindow::showWorldObjectsSound);
  connect(action("ID_TOOLS_WORLD_OBJECTS_LIGHTS"), &QAction::triggered, this,
          &MainWindow::showWorldObjectsLight);
  connect(action("ID_EDITORS_AMBIENTSOUNDS"), &QAction::triggered, this,
          [this]() { showNotPorted("AmbientSounds"); });
  connect(action("ID_SCRIPT_LEVEL_INTERFACE"), &QAction::triggered, this,
          [this]() { showNotPorted("ScriptLevelInterface"); });
  connect(action("ID_EDITORS_FILES"), &QAction::triggered, this, [this]() { showNotPorted("FilePage"); });
  connect(action("ID_EDITORS_AIPROPERTIES"), &QAction::triggered, this,
          [this]() { showNotPorted("AISettings"); });
  connect(action("IDD_ORPHANHUNTER"), &QAction::triggered, this, [this]() { showNotPorted("OrphanRemove"); });
  connect(action("ID_BRIEFING_EDITOR"), &QAction::triggered, this, [this]() { showNotPorted("BriefEdit"); });
  connect(action("ID_SUBEDITORS_FONT"), &QAction::triggered, this, [this]() { showNotPorted("GrFont"); });
  connect(action("ID_SUBEDITORS_HOGMAKER"), &QAction::triggered, this, &MainWindow::showHogMaker);
  connect(action("ID_SUBEDITORS_TABLEFILEEDIT"), &QAction::triggered, this,
          [this]() { showNotPorted("TableFileEdit"); });
  connect(action("ID_SUBEDITORS_TABLEFILEFILTER"), &QAction::triggered, this,
          [this]() { showNotPorted("TableFileFilter"); });
  connect(action("ID_EDITORS_DALLAS"), &QAction::triggered, this, [this]() { showNotPorted("DallasMain"); });

  // -------------------------------------------------------------- Terrain
  QMenu *terrainMenu = addMenu("&Terrain");
  terrainMenu->addAction(action("ID_TERRAIN_SELECT_RANGE"));
  terrainMenu->addAction(action("ID_TERRAIN_SOUNDS"));

  connect(action("ID_TERRAIN_SELECT_RANGE"), &QAction::triggered, this, &MainWindow::showSelectRange);
  connect(action("ID_TERRAIN_SOUNDS"), &QAction::triggered, this, &MainWindow::showTerrainSound);

  // ----------------------------------------------------------------- Help
  QMenu *helpMenu = addMenu("&Help");
  helpMenu->addAction(action("ID_D3HELP"));
  helpMenu->addAction(action("ID_APP_ABOUT"));

  connect(action("ID_D3HELP"), &QAction::triggered, this, [this]() { showNotPorted("Help"); });
  connect(action("ID_APP_ABOUT"), &QAction::triggered, this, &MainWindow::showAboutBox);
}

void MainWindow::showNotPorted(const QString &name) {
  QMessageBox::information(this, "Not yet ported",
                           QString("The %1 dialog has not been ported to Qt yet.").arg(name));
}

void MainWindow::showSplash() {
  m_splash = new SplashDialog(this);
  m_splash->putText("Loading...");
  m_splash->show();

  QTimer::singleShot(2500, this, [this]() {
    if (m_splash != nullptr) {
      delete m_splash;
      m_splash = nullptr;
    }
  });
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

void MainWindow::showGenericObject(int objType, int current) {
  WorldObjectsGenericDialog dlg(objType, current, this);
  dlg.exec();
  if (objType == OBJ_BUILDING)
    D3EditState.current_building = dlg.current();
  else if (objType == OBJ_CLUTTER)
    D3EditState.current_clutter = dlg.current();
}

}
