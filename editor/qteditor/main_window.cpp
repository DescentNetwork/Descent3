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

#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>

#include "about_dialog.h"
#include "qteditor_dialog.h"
#include "splash_dialog.h"

namespace QtEditor {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Descent 3 Editor");
  resize(1024, 768);

  statusBar()->showMessage("Ready");

  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

  QMenu *helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction("&About editor...", this, &MainWindow::showAboutBox);

  showSplash();
}

MainWindow::~MainWindow() = default;

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

}
