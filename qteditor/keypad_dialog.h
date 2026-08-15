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

#include "qteditor_dialog.h"

class QTabWidget;

namespace QtEditor {

// Port of CKeypadDialog: base for the editor's modeless keypad panels (which
// are hosted as tabs in the main window's keypad bar). The MFC scroll-bar
// plumbing is unnecessary in Qt; panels simply live inside a QTabWidget.
class Keypad : public Widget {
  Q_OBJECT
public:
  explicit Keypad(const QString &uiResource, QWidget *parent = nullptr);
  ~Keypad() override;

  virtual void runKeypadFunction(int code) { (void)code; }
  virtual void updateKeypad(int mask) { (void)mask; }

  static bool isActive() { return m_active; }
  static void deactivate() { m_active = false; }
  static void activate() { m_active = true; }

private:
  static bool m_active;
};

// Port of the main window's keypad bar (IDD_KEYPAD_DLGBAR): the docked tab
// control hosting all keypad panels.
class KeypadBar : public Widget {
  Q_OBJECT
public:
  explicit KeypadBar(QWidget *parent = nullptr);
  ~KeypadBar() override;

  QTabWidget *tabWidget() const { return m_tabs; }

private:
  QTabWidget *m_tabs;
};

}
