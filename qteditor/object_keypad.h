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

// Port of CObjectDialog (IDD_OBJECTKEYPAD): object placement/movement axes,
// flip, delete, next-object selection and default-reset.
class ObjectKeypad : public Keypad {
  Q_OBJECT
public:
  explicit ObjectKeypad(QWidget *parent = nullptr);
  ~ObjectKeypad() override;

private slots:
  void onPlaceObject();
  void onDeleteObject();
  void onNextObject();
  void onFlipObject();
  void onResetObjects();
  void onMoveAxis();
  void onAxisX();
  void onAxisY();
  void onAxisZ();
  void onAxisP();
  void onAxisH();
  void onAxisB();

private:
  void updateDialog();
  void setMoveAxis(int axis);
};

}
