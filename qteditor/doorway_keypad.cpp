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

#include "doorway_keypad.h"
#include "ui_doorwaykeypad.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "d3edit.h"
#include "door.h"
#include "doorway.h"
#include "editor_room_state.h"
#include "room_external.h"
#include "d3edit.h"

DoorwayKeypad::DoorwayKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::DoorwayKeypad)
{
  ui->setupUi(this);
  connect(ui->IDC_NEXT_DOOR, &QPushButton::clicked, this, &DoorwayKeypad::onNextDoor);
  connect(ui->IDC_PREV_DOOR, &QPushButton::clicked, this, &DoorwayKeypad::onPrevDoor);
  connect(ui->IDC_DOORWAY_PLACEDOOR, &QPushButton::clicked, this, &DoorwayKeypad::onPlaceDoor);
  connect(ui->IDC_DOORWAY_ATTACHDOOR, &QPushButton::clicked, this, &DoorwayKeypad::onAttachDoor);
  connect(ui->IDC_DOORWAY_LOCKED, &QCheckBox::toggled, this, &DoorwayKeypad::onLockedToggled);
  connect(ui->IDC_DOORWAY_AUTO, &QCheckBox::toggled, this, &DoorwayKeypad::onAutoToggled);
  connect(ui->IDC_DOORWAY_GB_IGNORE_LOCKED, &QCheckBox::toggled, this, &DoorwayKeypad::onIgnoreLockedToggled);
  connect(ui->IDC_DOORWAY_KEY_ALL, &QRadioButton::clicked, this, &DoorwayKeypad::onKeyAll);
  connect(ui->IDC_DOORWAY_KEY_ONLY_ONE, &QRadioButton::clicked, this, &DoorwayKeypad::onKeyOnlyOne);
  connect(ui->IDC_DOORWAY_POS_EDIT, &QLineEdit::editingFinished, this, &DoorwayKeypad::onPosEdited);

  connect(ui->IDC_KEY1_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY2_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY3_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY4_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY5_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY6_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY7_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);
  connect(ui->IDC_KEY8_CHECK, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);

  updateDialog();
}

DoorwayKeypad::~DoorwayKeypad() { delete ui; }

doorway *currentDoorway() {
  if (Curroomp == nullptr)
    return nullptr;
  return Curroomp->doorway_data.get();
}

void DoorwayKeypad::updateDialog() {
  doorway *dp = currentDoorway();
  // Win32 disables the doorway editing controls when no room is current.
  const bool active = (dp != nullptr);
  const QList<QWidget *> all = this->findChildren<QWidget *>();
  for (QWidget *w : all) {
    if (w->objectName().startsWith("IDC_KEY") || w->objectName().startsWith("IDC_DOORWAY"))
      w->setEnabled(active);
  }
  if (dp == nullptr)
    return;

  ui->IDC_DOORWAY_LOCKED->setChecked(dp->flags & DF_LOCKED);
  ui->IDC_DOORWAY_AUTO->setChecked(dp->flags & DF_AUTO);
  ui->IDC_DOORWAY_GB_IGNORE_LOCKED->setChecked(dp->flags & DF_GB_IGNORE_LOCKED);
  ui->IDC_DOORWAY_KEY_ALL->setChecked(!(dp->flags & DF_KEY_ONLY_ONE));
  ui->IDC_DOORWAY_KEY_ONLY_ONE->setChecked(dp->flags & DF_KEY_ONLY_ONE);

  ui->IDC_KEY1_CHECK->setChecked((dp->keys_needed & (1 << 0)) != 0);
  ui->IDC_KEY2_CHECK->setChecked((dp->keys_needed & (1 << 1)) != 0);
  ui->IDC_KEY3_CHECK->setChecked((dp->keys_needed & (1 << 2)) != 0);
  ui->IDC_KEY4_CHECK->setChecked((dp->keys_needed & (1 << 3)) != 0);
  ui->IDC_KEY5_CHECK->setChecked((dp->keys_needed & (1 << 4)) != 0);
  ui->IDC_KEY6_CHECK->setChecked((dp->keys_needed & (1 << 5)) != 0);
  ui->IDC_KEY7_CHECK->setChecked((dp->keys_needed & (1 << 6)) != 0);
  ui->IDC_KEY8_CHECK->setChecked((dp->keys_needed & (1 << 7)) != 0);

  ui->IDC_DOORWAY_POS_EDIT->setText(QString::number(dp->position));

  ui->IDC_DOORWAY_ID->setText(QString::number(dp->doornum));
  if (dp->doornum >= 0 && dp->doornum < MAX_DOORS && Doors[dp->doornum].used)
      ui->IDC_DOORWAYSELEDIT->setText(QString::fromStdString(Doors[dp->doornum].name));
}

void DoorwayKeypad::onNextDoor() {
  if (app.current_door >= 0) {
    app.current_door = GetNextDoor(app.current_door);
    updateDialog();
  }
}

void DoorwayKeypad::onPrevDoor() {
  if (app.current_door >= 0) {
    app.current_door = GetPrevDoor(app.current_door);
    updateDialog();
  }
}

void DoorwayKeypad::onLockedToggled(bool checked) {
  if (doorway *dp = currentDoorway()) {
    if (checked)
      dp->flags |= DF_LOCKED;
    else
      dp->flags &= ~DF_LOCKED;
  }
}

void DoorwayKeypad::onAutoToggled(bool checked) {
  if (doorway *dp = currentDoorway()) {
    if (checked)
      dp->flags |= DF_AUTO;
    else
      dp->flags &= ~DF_AUTO;
  }
}

void DoorwayKeypad::onIgnoreLockedToggled(bool checked) {
  if (doorway *dp = currentDoorway()) {
    if (checked)
      dp->flags |= DF_GB_IGNORE_LOCKED;
    else
      dp->flags &= ~DF_GB_IGNORE_LOCKED;
  }
}

void DoorwayKeypad::onKeyToggled() {
  doorway *dp = currentDoorway();
  if (dp == nullptr)
    return;
  uint8_t keys = 0;
  if (ui->IDC_KEY1_CHECK->isChecked())
    keys |= (1 << 0);
  if (ui->IDC_KEY2_CHECK->isChecked())
    keys |= (1 << 1);
  if (ui->IDC_KEY3_CHECK->isChecked())
    keys |= (1 << 2);
  if (ui->IDC_KEY4_CHECK->isChecked())
    keys |= (1 << 3);
  if (ui->IDC_KEY5_CHECK->isChecked())
    keys |= (1 << 4);
  if (ui->IDC_KEY6_CHECK->isChecked())
    keys |= (1 << 5);
  if (ui->IDC_KEY7_CHECK->isChecked())
    keys |= (1 << 6);
  if (ui->IDC_KEY8_CHECK->isChecked())
    keys |= (1 << 7);
  dp->keys_needed = keys;
}

void DoorwayKeypad::onKeyAll() {
  doorway *dp = currentDoorway();
  if (dp == nullptr)
    return;
  ui->IDC_KEY1_CHECK->setChecked(true);
  ui->IDC_KEY2_CHECK->setChecked(true);
  ui->IDC_KEY3_CHECK->setChecked(true);
  ui->IDC_KEY4_CHECK->setChecked(true);
  ui->IDC_KEY5_CHECK->setChecked(true);
  ui->IDC_KEY6_CHECK->setChecked(true);
  ui->IDC_KEY7_CHECK->setChecked(true);
  ui->IDC_KEY8_CHECK->setChecked(true);
  dp->keys_needed = 0xFF;
  dp->flags &= ~DF_KEY_ONLY_ONE;
}

void DoorwayKeypad::onKeyOnlyOne() {
  doorway *dp = currentDoorway();
  if (dp == nullptr)
    return;
  ui->IDC_KEY2_CHECK->setChecked(false);
  ui->IDC_KEY3_CHECK->setChecked(false);
  ui->IDC_KEY4_CHECK->setChecked(false);
  ui->IDC_KEY5_CHECK->setChecked(false);
  ui->IDC_KEY6_CHECK->setChecked(false);
  ui->IDC_KEY7_CHECK->setChecked(false);
  ui->IDC_KEY8_CHECK->setChecked(false);
  ui->IDC_KEY1_CHECK->setChecked(true);
  dp->keys_needed = 1;
  dp->flags |= DF_KEY_ONLY_ONE;
}

void DoorwayKeypad::onPosEdited() {
  if (doorway *dp = currentDoorway())
    dp->position = ui->IDC_DOORWAY_POS_EDIT->text().toFloat();
}

void DoorwayKeypad::onPlaceDoor() {
  if (Curroomp == nullptr) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No current room.");
    return;
  }
  if (Curface < 0 || Curface >= Curroomp->num_faces) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No current face.");
    return;
  }
  if (Curroomp->faces[Curface].portal_num != -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot place a door on a portal face.");
    return;
  }
  if (app.current_door < 0 || !Doors[app.current_door].used) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No door selected. Use the World Objects Door dialog first.");
    return;
  }
  PlaceDoor(Curroomp, Curface, app.current_door);
  updateDialog();
}

void DoorwayKeypad::onAttachDoor() {
  if (Placed_room == -1) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No door placed. Use Place Door first.");
    return;
  }
  AttachRoom();
  updateDialog();
}

