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

namespace QtEditor {

// Port of the matcen keypad (IDD_MATCENKEYPAD): materialization center
// navigation (prev/next/new/copy/paste/delete) and display.
class MatcenKeypad : public Keypad {
  Q_OBJECT
public:
  explicit MatcenKeypad(QWidget *parent = nullptr);
  ~MatcenKeypad() override;

private slots:
  void onPrev();
  void onNext();
  void onNew();
  void onDelete();
  void onCopy();
  void onPaste();

private:
  void updateDialog();

  int m_matcenId = 0;
};

}
