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
#include <cstring>

#include "d3_editor_init.h"
#include "dialog_test.h"
#include "main_window.h"


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("Descent 3 Editor");
  app.setOrganizationName("DescentDevelopers");

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--dialog-test") == 0) {
      QtEditor::initD3Core(argc, argv);
      return QtEditor::runDialogTest();
    }
  }

  QtEditor::MainWindow window;
  window.show();
  QCoreApplication::processEvents();

  QtEditor::initD3Core(argc, argv);

  return app.exec();
}
