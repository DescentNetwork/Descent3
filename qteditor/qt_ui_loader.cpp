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

#include "qt_ui_loader.h"

#include <QFile>
#include <QUiLoader>

namespace QtEditor {

namespace {

// The QUiLoader must outlive every widget it creates, because the loaded
// widgets hold references to its internal plugin factory loader.
QUiLoader &persistentLoader() {
  static QUiLoader *loader = new QUiLoader;
  return *loader;
}

} // namespace

QWidget *UiLoader::load(const QString &resourcePath, QWidget *parent) {
  QFile file(resourcePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return nullptr;
  }
  QWidget *widget = persistentLoader().load(&file, parent);
  file.close();
  return widget;
}

}
