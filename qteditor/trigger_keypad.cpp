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

#include "trigger_keypad.h"
#include "ui_triggerkeypad.h"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

#include "d3edit.h"
#include "room_external.h"
#include "trigger.h"


TriggerKeypad::TriggerKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::TriggerKeypad)
{
  ui->setupUi(this);
  connect(ui->IDC_TRIG_DELETE, &QPushButton::clicked, this, &TriggerKeypad::onDelete);
  connect(ui->IDC_TRIG_PREV_IN_MINE, &QPushButton::clicked, this, &TriggerKeypad::onPrevInMine);
  connect(ui->IDC_TRIG_NEXT_IN_MINE, &QPushButton::clicked, this, &TriggerKeypad::onNextInMine);
  connect(ui->IDC_TRIG_PREV_IN_ROOM, &QPushButton::clicked, this, &TriggerKeypad::onPrevInRoom);
  connect(ui->IDC_TRIG_NEXT_IN_ROOM, &QPushButton::clicked, this, &TriggerKeypad::onNextInRoom);
  connect(ui->IDC_TRIG_NEXT_PORTAL, &QPushButton::clicked, this, &TriggerKeypad::onNextPortal);
  connect(ui->IDC_TRIG_ONESHOT, &QCheckBox::toggled, this, &TriggerKeypad::onOneshotToggled);

  connect(ui->IDC_TRIG_ACTIV_PLAYER, &QCheckBox::toggled, this, [this](bool checked) {
    if (Current_trigger >= 0 && Current_trigger < Num_triggers)
      Triggers[Current_trigger].activator.player = checked;
  });
  connect(ui->IDC_TRIG_ACTIV_PLAYER_WEAPONS, &QCheckBox::toggled, this, [this](bool checked) {
    if (Current_trigger >= 0 && Current_trigger < Num_triggers)
      Triggers[Current_trigger].activator.player_weapon = checked;
  });
  connect(ui->IDC_TRIG_ACTIV_ROBOTS, &QCheckBox::toggled, this, [this](bool checked) {
    if (Current_trigger >= 0 && Current_trigger < Num_triggers)
      Triggers[Current_trigger].activator.robot = checked;
  });
  connect(ui->IDC_TRIG_ACTIV_ROBOT_WEAPONS, &QCheckBox::toggled, this, [this](bool checked) {
    if (Current_trigger >= 0 && Current_trigger < Num_triggers)
      Triggers[Current_trigger].activator.robot_weapon = checked;
  });
  connect(ui->IDC_TRIG_ACTIV_CLUTTER, &QCheckBox::toggled, this, [this](bool checked) {
    if (Current_trigger >= 0 && Current_trigger < Num_triggers)
      Triggers[Current_trigger].activator.clutter = checked;
  });

  updateDialog();
}

TriggerKeypad::~TriggerKeypad() { delete ui; }

void TriggerKeypad::updateDialog() {
  // Win32 disables trigger editing when there is no current trigger (which
  // requires a loaded level with triggers).
  const bool active = (Current_trigger >= 0 && Current_trigger < Num_triggers);
  const QList<QWidget *> all = this->findChildren<QWidget *>();
  for (QWidget *w : all)
    if (w->objectName().startsWith("IDC_TRIG"))
      w->setEnabled(active);
  if (!active)
    return;
  trigger *tp = &Triggers[Current_trigger];

  ui->IDC_TRIG_CURRENT_NAME->setText(QString::fromStdString(tp->name));
  ui->IDC_TRIG_CURRENT_NUM->setText(QString::number(Current_trigger));
  ui->IDC_TRIG_CURRENT_ROOM->setText(QString::number(tp->roomnum));
  ui->IDC_TRIG_CURRENT_FACE->setText(QString::number(tp->facenum));

  ui->IDC_TRIG_ONESHOT->setChecked(tp->flags.oneshot);

  ui->IDC_TRIG_ACTIV_PLAYER->setChecked(tp->activator.player);
  ui->IDC_TRIG_ACTIV_PLAYER_WEAPONS->setChecked(tp->activator.player_weapon);
  ui->IDC_TRIG_ACTIV_ROBOTS->setChecked(tp->activator.robot);
  ui->IDC_TRIG_ACTIV_ROBOT_WEAPONS->setChecked(tp->activator.robot_weapon);
  ui->IDC_TRIG_ACTIV_CLUTTER->setChecked(tp->activator.clutter);
}

void TriggerKeypad::onOneshotToggled(bool checked) {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  Triggers[Current_trigger].flags.oneshot = checked;
}

void TriggerKeypad::onDelete() {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  // Mirror the original: mark unused and renumber triggers above it.
  Triggers[Current_trigger].flags.unused = true;
  for (int i = Current_trigger + 1; i < Num_triggers; i++)
    Triggers[i - 1] = Triggers[i];
  Num_triggers--;
  if (Current_trigger >= Num_triggers)
    Current_trigger = Num_triggers - 1;
  updateDialog();
}

void TriggerKeypad::onPrevInMine() {
  if (Num_triggers <= 0)
    return;
  Current_trigger = (Current_trigger <= 0) ? (Num_triggers - 1) : (Current_trigger - 1);
  updateDialog();
}

void TriggerKeypad::onNextInMine() {
  if (Num_triggers <= 0)
    return;
  Current_trigger = (Current_trigger + 1) % Num_triggers;
  updateDialog();
}

void TriggerKeypad::onPrevInRoom() {
  if (Num_triggers <= 0)
    return;
  int n = Current_trigger;
  for (int i = Num_triggers; i > 0; i--) {
    n = (n <= 0) ? (Num_triggers - 1) : (n - 1);
    if (Triggers[n].roomnum == (Curroomp != nullptr ? ROOMNUM(Curroomp) : Triggers[Current_trigger].roomnum)) {
      Current_trigger = n;
      break;
    }
  }
  updateDialog();
}

void TriggerKeypad::onNextInRoom() {
  if (Num_triggers <= 0)
    return;
  int n = Current_trigger;
  for (int i = 0; i < Num_triggers; i++) {
    n = (n + 1) % Num_triggers;
    if (Triggers[n].roomnum == (Curroomp != nullptr ? ROOMNUM(Curroomp) : Triggers[Current_trigger].roomnum)) {
      Current_trigger = n;
      break;
    }
  }
  updateDialog();
}

void TriggerKeypad::onNextPortal() {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  // Advance to the next trigger attached to a portal (face with a portal).
  for (int i = 1; i < Num_triggers; i++) {
    const int n = (Current_trigger + i) % Num_triggers;
    if (Triggers[n].roomnum >= 0 && Triggers[n].roomnum < MAX_ROOMS &&
        Rooms[Triggers[n].roomnum].faces[Triggers[n].facenum].portal_num != -1) {
      Current_trigger = n;
      break;
    }
  }
  updateDialog();
}

