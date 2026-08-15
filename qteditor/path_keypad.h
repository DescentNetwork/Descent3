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

// Port of CPathPadDialog (IDD_PATHKEYPAD): path and AI-node editing.
class PathKeypad : public Keypad {
  Q_OBJECT
public:
  explicit PathKeypad(QWidget *parent = nullptr);
  ~PathKeypad() override;

private slots:
  void onAddPath();
  void onDeletePath();
  void onPathPulldownChanged();
  void onNextNode();
  void onPrevNode();
  void onCurrentNodeEdited();
  void onInsertNode();
  void onDeleteNode();
  void onMoveNode();
  void onShowNodesToggled(bool checked);
  void onIncEdited();

private:
  void updateDialog();
  int currentPath();
  int currentNode();

  float m_inc = 10.0f;
};

}
