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

QT_BEGIN_NAMESPACE
namespace Ui { class RoomKeypad; }
QT_END_NAMESPACE


// Port of CRoomKeypadDialog (IDD_ROOMKEYPAD): current room info, flags,
// expand/contract geometry and damage settings.
class RoomKeypad : public QDialog {
  Q_OBJECT
public:
  explicit RoomKeypad(QWidget *parent = nullptr);
  ~RoomKeypad();

private slots:
  void onMarkRoom();
  void onExpandEdge();
  void onContractEdge();
  void onExpandFace();
  void onContractFace();
  void onExpandRoom();
  void onContractRoom();

private:
  void updateDialog();
  void expandGeometry(float scale);

  Ui::RoomKeypad *ui;
};

