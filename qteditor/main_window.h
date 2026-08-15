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
class QWidget;

namespace QtEditor {

class Dialog;
class KeypadBar;
class ViewerPropDialog;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  // Generic "this feature is not yet ported" prompt used by the menu handlers
  // for items whose dialog has not been reimplemented for Qt.
  void showNotPorted(const QString &name);

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

  // Host widget that owns the QAction definitions loaded from a .ui resource.
  QWidget *m_actionsHost = nullptr;
  QDockWidget *m_keypadDock = nullptr;
  KeypadBar *m_keypadBar = nullptr;
  Dialog *m_aboutBox = nullptr;
  ViewerPropDialog *m_viewerProps = nullptr;

  // Path of the currently open .d3l, or empty if none / untitled. Updated by
  // onFileOpen / onFileSaveAs and cleared by onFileNew.
  QString m_currentLevelFile;
};

}
