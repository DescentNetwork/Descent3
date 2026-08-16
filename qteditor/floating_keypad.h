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

#include "keypad_dialog.h"

class QTabWidget;

namespace QtEditor {

// Port of CFloatingKeypadDialog (IDD_FLOATING_KEYPAD): a floating window
// hosting the keypad panels as tabs, independent of the docked keypad bar.
class FloatingKeypad : public Widget {
  Q_OBJECT
public:
  explicit FloatingKeypad(QWidget *parent = nullptr);
  ~FloatingKeypad() override;

  QTabWidget *tabWidget() const { return m_tabs; }
  void addTab(Keypad *keypad, const QString &title);

private:
  QTabWidget *m_tabs;
};

}
