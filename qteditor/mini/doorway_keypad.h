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
namespace Ui { class DoorwayKeypad; }
QT_END_NAMESPACE

// Port of CDoorwayDialog (IDD_DOORWAYKEYPAD): doorway keypad editing the
// current room face's doorway (keys, locked, auto, position, hit points).
class DoorwayKeypad : public QDialog {
  Q_OBJECT
public:
  explicit DoorwayKeypad(QWidget *parent = nullptr);
  ~DoorwayKeypad();

private slots:
  void onNextDoor();
  void onPrevDoor();
  void onLockedToggled(bool checked);
  void onAutoToggled(bool checked);
  void onIgnoreLockedToggled(bool checked);
  void onKeyToggled();
  void onKeyAll();
  void onKeyOnlyOne();
  void onPosEdited();
  void onPlaceDoor();
  void onAttachDoor();

private:
  void updateDialog();
  void setKeyBit(int bit, bool checked);

  template <typename T>
  T *find(const QString &name) const { return findChild<T *>(name); }

  Ui::DoorwayKeypad *ui;
};

