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

#include "level_keypad.h"
#include "ui_levelkeypad.h"

#include <QLineEdit>

#include "d3edit.h"
#include "physics.h"
#include "room_external.h"


namespace {
// Level ceiling height lives in the rooms; report the max ceiling of used rooms.
float levelCeiling() {
  float maxy = -1e30f;
  bool any = false;
  for (int r = 0; r <= Highest_room_index; r++) {
    room *rp = &Rooms[r];
    if (!rp->used)
      continue;
    for (int v = 0; v < rp->num_verts; v++) {
      if (rp->verts[v].y() > maxy) {
        maxy = rp->verts[v].y();
        any = true;
      }
    }
  }
  return any ? maxy : 0.0f;
}
} // namespace

LevelKeypad::LevelKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::LevelKeypad)
{
  ui->setupUi(this);
  if (QLineEdit *edit = ui->IDC_LEVEL_GRAVITY_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &LevelKeypad::onGravityEdited);
  if (QLineEdit *edit = ui->IDC_LEVEL_CEILING_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &LevelKeypad::onCeilingEdited);

  updateDialog();
}

LevelKeypad::~LevelKeypad() { delete ui; }

void LevelKeypad::updateDialog() {
  if (QLineEdit *edit = ui->IDC_LEVEL_GRAVITY_EDIT)
    edit->setText(QString::number(Gravity_strength));
  if (QLineEdit *edit = ui->IDC_LEVEL_CEILING_EDIT)
    edit->setText(QString::number(levelCeiling()));
}

void LevelKeypad::onGravityEdited() {
  if (QLineEdit *edit = ui->IDC_LEVEL_GRAVITY_EDIT)
    Gravity_strength = edit->text().toFloat();
  World_changed = true;
}

void LevelKeypad::onCeilingEdited() {
  // Setting the ceiling shifts all rooms' vertices so their max Y equals the
  // entered value (a simple uniform fit; the Win32 editor did per-room).
  if (QLineEdit *edit = ui->IDC_LEVEL_CEILING_EDIT) {
    const float target = edit->text().toFloat();
    const float cur = levelCeiling();
    const float delta = target - cur;
    for (int r = 0; r <= Highest_room_index; r++) {
      room *rp = &Rooms[r];
      if (!rp->used)
        continue;
      for (int v = 0; v < rp->num_verts; v++)
        rp->verts[v].y() += delta;
    }
    World_changed = true;
  }
}

