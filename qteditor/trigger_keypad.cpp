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

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

#include "d3edit.h"
#include "room_external.h"
#include "trigger.h"

namespace QtEditor {

TriggerKeypad::TriggerKeypad(QWidget *parent) : Keypad(":/ui/triggerkeypad.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_DELETE"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onDelete);
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_PREV_IN_MINE"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onPrevInMine);
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_NEXT_IN_MINE"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onNextInMine);
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_PREV_IN_ROOM"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onPrevInRoom);
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_NEXT_IN_ROOM"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onNextInRoom);
  if (QPushButton *b = find<QPushButton>("IDC_TRIG_NEXT_PORTAL"))
    connect(b, &QPushButton::clicked, this, &TriggerKeypad::onNextPortal);
  if (QCheckBox *cb = find<QCheckBox>("IDC_TRIG_ONESHOT"))
    connect(cb, &QCheckBox::toggled, this, &TriggerKeypad::onOneshotToggled);

  const char *activators[] = {"IDC_TRIG_ACTIV_PLAYER", "IDC_TRIG_ACTIV_PLAYER_WEAPONS",
                              "IDC_TRIG_ACTIV_ROBOTS", "IDC_TRIG_ACTIV_ROBOT_WEAPONS", "IDC_TRIG_ACTIV_CLUTTER"};
  for (const char *name : activators)
    if (QCheckBox *cb = find<QCheckBox>(name))
      connect(cb, &QCheckBox::toggled, this, &TriggerKeypad::onActivatorToggled);

  updateDialog();
}

TriggerKeypad::~TriggerKeypad() = default;

void TriggerKeypad::updateDialog() {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  trigger *tp = &Triggers[Current_trigger];

  if (QLabel *label = find<QLabel>("IDC_TRIG_CURRENT_NAME"))
    label->setText(tp->name);
  if (QLabel *label = find<QLabel>("IDC_TRIG_CURRENT_NUM"))
    label->setText(QString::number(Current_trigger));
  if (QLabel *label = find<QLabel>("IDC_TRIG_CURRENT_ROOM"))
    label->setText(QString::number(tp->roomnum));
  if (QLabel *label = find<QLabel>("IDC_TRIG_CURRENT_FACE"))
    label->setText(QString::number(tp->facenum));

  if (QCheckBox *cb = find<QCheckBox>("IDC_TRIG_ONESHOT"))
    cb->setChecked(tp->flags & TF_ONESHOT);

  const struct {
    const char *name;
    uint16_t flag;
  } act[] = {
      {"IDC_TRIG_ACTIV_PLAYER", AF_PLAYER},
      {"IDC_TRIG_ACTIV_PLAYER_WEAPONS", AF_PLAYER_WEAPON},
      {"IDC_TRIG_ACTIV_ROBOTS", AF_ROBOT},
      {"IDC_TRIG_ACTIV_ROBOT_WEAPONS", AF_ROBOT_WEAPON},
      {"IDC_TRIG_ACTIV_CLUTTER", AF_CLUTTER},
  };
  for (const auto &a : act)
    if (QCheckBox *cb = find<QCheckBox>(a.name))
      cb->setChecked(tp->activator & a.flag);
}

void TriggerKeypad::setActivator(uint16_t flag, const char *checkName, bool checked) {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  if (checked)
    Triggers[Current_trigger].activator |= flag;
  else
    Triggers[Current_trigger].activator &= ~flag;
}

void TriggerKeypad::onOneshotToggled(bool checked) {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  if (checked)
    Triggers[Current_trigger].flags |= TF_ONESHOT;
  else
    Triggers[Current_trigger].flags &= ~TF_ONESHOT;
}

void TriggerKeypad::onActivatorToggled() {
  const struct {
    const char *name;
    uint16_t flag;
  } act[] = {
      {"IDC_TRIG_ACTIV_PLAYER", AF_PLAYER},
      {"IDC_TRIG_ACTIV_PLAYER_WEAPONS", AF_PLAYER_WEAPON},
      {"IDC_TRIG_ACTIV_ROBOTS", AF_ROBOT},
      {"IDC_TRIG_ACTIV_ROBOT_WEAPONS", AF_ROBOT_WEAPON},
      {"IDC_TRIG_ACTIV_CLUTTER", AF_CLUTTER},
  };
  for (const auto &a : act)
    if (QCheckBox *cb = find<QCheckBox>(a.name))
      setActivator(a.flag, a.name, cb->isChecked());
}

void TriggerKeypad::onDelete() {
  if (Current_trigger < 0 || Current_trigger >= Num_triggers)
    return;
  // Mirror the original: mark unused and renumber triggers above it.
  Triggers[Current_trigger].flags |= TF_UNUSED;
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

}
