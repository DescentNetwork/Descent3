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

#include <QDialog>
#include <QString>
#include <QWidget>

#include "ddebug/pserror.h"

namespace QtEditor {

// adoptUi copies the loaded .ui root's properties (title/geometry/enabled)
// into `self` and reparents the root's direct children, then deletes the
// temporary root widget. Used by both Widget and Dialog.
void adoptUi(QWidget *self, QWidget *loaded);

// Base class for any UI loaded at runtime from a .ui resource via QUiLoader.
// Widget IS the widget: the loaded .ui's children are reparented into this and
// its root properties are copied over, so the class behaves as a normal
// QWidget. Controls are looked up by their objectName (the converted
// IDC_*/IDD_* identifiers) with find<T>() / operator[].
class Widget : public QWidget {
public:
  explicit Widget(const QString &uiResource, QWidget *parent = nullptr);
  ~Widget() override;

  QWidget *handle() { return this; }

  template <typename T = QWidget>
  T *find(const QString &objectName) const {
    return findChild<T *>(objectName);
  }

  QWidget *operator[](const QString &objectName) const { return find(objectName); }

protected:
  void loadUi(const QString &uiResource);
};

// Base class for modal/modeless dialogs: Widget is a QDialog. IDOK/IDCANCEL
// buttons are auto-wired to accept()/reject().
class Dialog : public QDialog {
public:
  explicit Dialog(const QString &uiResource, QWidget *parent = nullptr);
  ~Dialog() override;

  QDialog *dialog() { return this; }

  int exec() { return QDialog::exec(); }

  QWidget *handle() { return this; }

  template <typename T = QWidget>
  T *find(const QString &objectName) const {
    return findChild<T *>(objectName);
  }

  QWidget *operator[](const QString &objectName) const { return find(objectName); }

protected:
  void loadUi(const QString &uiResource);
};

}
