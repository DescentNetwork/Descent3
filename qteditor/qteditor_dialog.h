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
#include <QObject>
#include <QString>

namespace QtEditor {

// Base class for any UI loaded at runtime from a .ui resource via QUiLoader.
// Controls are looked up by their objectName (the converted IDC_*/IDD_*
// identifiers) with find<T>() / operator[].
class Widget : public QObject {
  Q_OBJECT
public:
  explicit Widget(const QString &uiResource, QWidget *parent = nullptr);
  ~Widget() override;

  QWidget *handle() const { return m_widget; }

  template <typename T = QWidget>
  T *find(const QString &objectName) const {
    return m_widget->findChild<T *>(objectName);
  }

  QWidget *operator[](const QString &objectName) const;

  void show() const;

protected:
  QWidget *m_widget;
};

// Base class for modal/modeless dialogs: a Widget whose root is a QDialog.
// IDOK/IDCANCEL buttons are auto-wired to accept()/reject().
class Dialog : public Widget {
  Q_OBJECT
public:
  explicit Dialog(const QString &uiResource, QWidget *parent = nullptr);
  ~Dialog() override;

  QDialog *dialog() const { return m_dialog; }

  int exec() const;

protected:
  QDialog *m_dialog;
};

}
