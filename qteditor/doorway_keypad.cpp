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

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "d3edit.h"
#include "door.h"
#include "doorway.h"
#include "room_external.h"

namespace QtEditor {

namespace {
const char *kKeyCheck[8] = {"IDC_KEY1_CHECK", "IDC_KEY2_CHECK", "IDC_KEY3_CHECK", "IDC_KEY4_CHECK",
                            "IDC_KEY5_CHECK", "IDC_KEY6_CHECK", "IDC_KEY7_CHECK", "IDC_KEY8_CHECK"};
} // namespace

DoorwayKeypad::DoorwayKeypad(QWidget *parent) : Keypad(":/ui/doorwaykeypad.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_NEXT_DOOR"))
    connect(b, &QPushButton::clicked, this, &DoorwayKeypad::onNextDoor);
  if (QPushButton *b = find<QPushButton>("IDC_PREV_DOOR"))
    connect(b, &QPushButton::clicked, this, &DoorwayKeypad::onPrevDoor);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_LOCKED"))
    connect(cb, &QCheckBox::toggled, this, &DoorwayKeypad::onLockedToggled);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_AUTO"))
    connect(cb, &QCheckBox::toggled, this, &DoorwayKeypad::onAutoToggled);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_GB_IGNORE_LOCKED"))
    connect(cb, &QCheckBox::toggled, this, &DoorwayKeypad::onIgnoreLockedToggled);
  if (QRadioButton *rb = find<QRadioButton>("IDC_DOORWAY_KEY_ALL"))
    connect(rb, &QRadioButton::clicked, this, &DoorwayKeypad::onKeyAll);
  if (QRadioButton *rb = find<QRadioButton>("IDC_DOORWAY_KEY_ONLY_ONE"))
    connect(rb, &QRadioButton::clicked, this, &DoorwayKeypad::onKeyOnlyOne);
  if (QLineEdit *edit = find<QLineEdit>("IDC_DOORWAY_POS_EDIT"))
    connect(edit, &QLineEdit::editingFinished, this, &DoorwayKeypad::onPosEdited);

  for (const char *name : kKeyCheck)
    if (QCheckBox *cb = find<QCheckBox>(name))
      connect(cb, &QCheckBox::toggled, this, &DoorwayKeypad::onKeyToggled);

  updateDialog();
}

DoorwayKeypad::~DoorwayKeypad() = default;

doorway *currentDoorway() {
  if (Curroomp == nullptr)
    return nullptr;
  return Curroomp->doorway_data;
}

void DoorwayKeypad::updateDialog() {
  doorway *dp = currentDoorway();
  // Win32 disables the doorway editing controls when no room is current.
  const bool active = (dp != nullptr);
  const QList<QWidget *> all = m_widget->findChildren<QWidget *>();
  for (QWidget *w : all) {
    if (w->objectName().startsWith("IDC_KEY") || w->objectName().startsWith("IDC_DOORWAY"))
      w->setEnabled(active);
  }
  if (dp == nullptr)
    return;

  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_LOCKED"))
    cb->setChecked(dp->flags & DF_LOCKED);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_AUTO"))
    cb->setChecked(dp->flags & DF_AUTO);
  if (QCheckBox *cb = find<QCheckBox>("IDC_DOORWAY_GB_IGNORE_LOCKED"))
    cb->setChecked(dp->flags & DF_GB_IGNORE_LOCKED);
  if (QRadioButton *rb = find<QRadioButton>("IDC_DOORWAY_KEY_ALL"))
    rb->setChecked(!(dp->flags & DF_KEY_ONLY_ONE));
  if (QRadioButton *rb = find<QRadioButton>("IDC_DOORWAY_KEY_ONLY_ONE"))
    rb->setChecked(dp->flags & DF_KEY_ONLY_ONE);

  for (int k = 0; k < 8; k++)
    if (QCheckBox *cb = find<QCheckBox>(kKeyCheck[k]))
      cb->setChecked((dp->keys_needed & (1 << k)) != 0);

  if (QLineEdit *edit = find<QLineEdit>("IDC_DOORWAY_POS_EDIT"))
    edit->setText(QString::number(dp->position));

  if (QLabel *label = find<QLabel>("IDC_DOORWAY_ID"))
    label->setText(QString::number(dp->doornum));
  if (QLineEdit *edit = find<QLineEdit>("IDC_DOORWAYSELEDIT")) {
    if (dp->doornum >= 0 && dp->doornum < MAX_DOORS && Doors[dp->doornum].used)
      edit->setText(Doors[dp->doornum].name);
  }
}

void DoorwayKeypad::onNextDoor() {
  if (D3EditState.current_door >= 0) {
    D3EditState.current_door = GetNextDoor(D3EditState.current_door);
    updateDialog();
  }
}

void DoorwayKeypad::onPrevDoor() {
  if (D3EditState.current_door >= 0) {
    D3EditState.current_door = GetPrevDoor(D3EditState.current_door);
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
  for (int k = 0; k < 8; k++)
    if (QCheckBox *cb = find<QCheckBox>(kKeyCheck[k]))
      if (cb->isChecked())
        keys |= (1 << k);
  dp->keys_needed = keys;
}

void DoorwayKeypad::onKeyAll() {
  doorway *dp = currentDoorway();
  if (dp == nullptr)
    return;
  for (int k = 0; k < 8; k++)
    if (QCheckBox *cb = find<QCheckBox>(kKeyCheck[k]))
      cb->setChecked(true);
  dp->keys_needed = 0xFF;
  dp->flags &= ~DF_KEY_ONLY_ONE;
}

void DoorwayKeypad::onKeyOnlyOne() {
  doorway *dp = currentDoorway();
  if (dp == nullptr)
    return;
  for (int k = 1; k < 8; k++)
    if (QCheckBox *cb = find<QCheckBox>(kKeyCheck[k]))
      cb->setChecked(false);
  if (QCheckBox *cb = find<QCheckBox>(kKeyCheck[0]))
    cb->setChecked(true);
  dp->keys_needed = 1;
  dp->flags |= DF_KEY_ONLY_ONE;
}

void DoorwayKeypad::onPosEdited() {
  if (doorway *dp = currentDoorway())
    if (QLineEdit *edit = find<QLineEdit>("IDC_DOORWAY_POS_EDIT"))
      dp->position = edit->text().toFloat();
}

}
