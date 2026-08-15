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
class SplashDialog;
class ViewerPropDialog;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  // Looks up a menu action by its ID_* identifier in the .ui-loaded action
  // host (falling back to a stub action if the identifier is missing).
  QAction *action(const QString &id);
  void buildMenus();
  QMenu *addMenu(const QString &title);

  void showSplash();
  void showAboutBox();
  void showNotPorted(const QString &name);
  void showLevelInfo();
  void showPreferences();
  void toggleViewerProps();
  void showHogMaker();
  void showSelectRange();
  void showTerrainSound();
  void showWorldObjectsDoor();
  void showWorldObjectsPlayer();
  void showWorldObjectsLight();

  // Host widget that owns the QAction definitions loaded from a .ui resource.
  QWidget *m_actionsHost = nullptr;
  SplashDialog *m_splash = nullptr;
  Dialog *m_aboutBox = nullptr;
  ViewerPropDialog *m_viewerProps = nullptr;
};

}
