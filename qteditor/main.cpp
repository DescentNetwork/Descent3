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

#include <filesystem>

#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <cstring>
#include <cerrno>

#include "d3_editor_init.h"
#include "dialog_test.h"
#include "main_window.h"



int main(int argc, char *argv[]) {
  errno = 0;
  QApplication app(argc, argv);
  app.setApplicationName("Descent 3 Editor");
  app.setOrganizationName("DescentDevelopers");

  // Construct the splash BEFORE the MainWindow so the user gets immediate
  // feedback while the D3 core initialises. QSplashScreen::finish() closes
  // the splash automatically once the main window becomes the active window.
  QPixmap pixmap(":/IDB_D3SPLASH");
  QSplashScreen splash(pixmap);
  splash.show();
  splash.showMessage("Loading...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
  app.processEvents();

  initD3Core(argc, argv);
  splash.showMessage("Initialising editor...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
  app.processEvents();

  MainWindow window;
  window.show();
  splash.finish(&window);

  return app.exec();
}
