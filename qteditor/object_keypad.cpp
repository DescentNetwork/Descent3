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

#include "object_keypad.h"
#include "ui_objectkeypad.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include "d3edit.h"
#include "debug.h"
#include "objinfo.h"
#include "object.h"
#include "object_ops.h"

#include "room.h"

extern bool f_allow_objects_to_be_pushed_through_walls;

ObjectKeypad::ObjectKeypad(QWidget *parent) : QDialog(parent), ui(new Ui::ObjectKeypad)
{
  ui->setupUi(this);
  connect(ui->IDC_OBJPAD_PLACEOBJ, &QPushButton::clicked, this, &ObjectKeypad::onPlaceObject);
  connect(ui->IDC_OBJ_DELOBJ, &QPushButton::clicked, this, &ObjectKeypad::onDeleteObject);
  connect(ui->IDC_OBJPAD_NEXTOBJ, &QPushButton::clicked, this, &ObjectKeypad::onNextObject);
  connect(ui->IDC_OBJPAD_FLIPOBJ, &QPushButton::clicked, this, &ObjectKeypad::onFlipObject);
  connect(ui->IDC_RESET_OBJECTS, &QPushButton::clicked, this, &ObjectKeypad::onResetObjects);
  connect(ui->IDC_OBJPAD_SETDEFAULT, &QPushButton::clicked, this, &ObjectKeypad::onSetDefault);
  connect(ui->IDC_OBJ_ROT90, &QPushButton::clicked, this, &ObjectKeypad::onRot90);
  connect(ui->IDC_OBJPAD_DELETEALL, &QPushButton::clicked, this, &ObjectKeypad::onDeleteAll);
  if (QCheckBox *cb = ui->IDC_OBJECT_PUSHTHROUGHWALLS)
    connect(cb, &QCheckBox::toggled, this, &ObjectKeypad::onPushThroughWalls);
  connect(ui->IDC_OBJMOVEX, &QPushButton::clicked, this, &ObjectKeypad::onAxisX);
  connect(ui->IDC_OBJMOVEY, &QPushButton::clicked, this, &ObjectKeypad::onAxisY);
  connect(ui->IDC_OBJMOVEZ, &QPushButton::clicked, this, &ObjectKeypad::onAxisZ);
  connect(ui->IDC_OBJMOVEP, &QPushButton::clicked, this, &ObjectKeypad::onAxisP);
  connect(ui->IDC_OBJMOVEH, &QPushButton::clicked, this, &ObjectKeypad::onAxisH);
  connect(ui->IDC_OBJMOVEB, &QPushButton::clicked, this, &ObjectKeypad::onAxisB);

  if (QCheckBox *cb = ui->IDC_OBJECT_PUSHTHROUGHWALLS)
    cb->setChecked(f_allow_objects_to_be_pushed_through_walls);

  updateDialog();
}

ObjectKeypad::~ObjectKeypad() { delete ui; }

void ObjectKeypad::setMoveAxis(int axis) {
  D3EditState.object_move_axis = axis;
  updateDialog();
}

void ObjectKeypad::updateDialog() {
  const bool hasObject = (Cur_object_index >= 0 && Cur_object_index <= Highest_object_index &&
                          Objects[Cur_object_index].type != OBJ_NONE);
  const char *names[] = {"IDC_OBJPAD_FLIPOBJ", "IDC_OBJ_DELOBJ", "IDC_OBJPAD_NEXTOBJ",
                         "IDC_OBJPAD_SETDEFAULT", "IDC_OBJ_ROT90"};
  for (const char *name : names)
    if (QWidget *w = findChild<QWidget*>(name))
      w->setEnabled(hasObject);

  const struct {
    const char *name;
    int axis;
  } axes[] = {{"IDC_OBJMOVEX", 0}, {"IDC_OBJMOVEY", 1}, {"IDC_OBJMOVEZ", 2},
              {"IDC_OBJMOVEP", 3}, {"IDC_OBJMOVEH", 4}, {"IDC_OBJMOVEB", 5}};
  for (const auto &a : axes)
    if (QPushButton *b = findChild<QPushButton*>(a.name))
      b->setChecked(D3EditState.object_move_axis == a.axis);
}

void ObjectKeypad::onPlaceObject() {
  // HObjectPlace handles all the validation internally.
  if (HObjectPlace(D3EditState.current_obj_type, D3EditState.current_obj_id)) {
    Mine_changed = true;
    updateDialog();
  }
}

void ObjectKeypad::onDeleteObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  HObjectDelete();
  updateDialog();
}

void ObjectKeypad::onNextObject() {
  if (Cur_object_index < 0)
    return;
  for (int i = Cur_object_index + 1; i <= Highest_object_index; i++) {
    if (Objects[i].type != OBJ_NONE && Objects[i].type != OBJ_ROOM) {
      Cur_object_index = i;
      updateDialog();
      return;
    }
  }
  for (int i = 0; i <= Cur_object_index; i++) {
    if (Objects[i].type != OBJ_NONE && Objects[i].type != OBJ_ROOM) {
      Cur_object_index = i;
      updateDialog();
      return;
    }
  }
}

void ObjectKeypad::onFlipObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  HObjectFlip();
  updateDialog();
}

void ObjectKeypad::onResetObjects() {
  for (int i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type == OBJ_NONE)
      continue;
    const int type = Objects[i].type;
    if (type < 0 || type >= MAX_OBJECT_TYPES || Object_info[type].type == OBJ_NONE)
      continue;
    Objects[i].flags = Object_info[type].flags;
    Objects[i].size = Object_info[type].size;
  }
  Mine_changed = true;
}

void ObjectKeypad::onSetDefault() {
  HObjectSetDefault();
  updateDialog();
}

void ObjectKeypad::onRot90() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  // Rotate 90 degrees (PI/2 radians = 8192 angle units in D3).
  RotateObject(Cur_object_index, 8192, 0, 0);
  World_changed = true;
  updateDialog();
}

void ObjectKeypad::onDeleteAll()
{
  if(QMessageBox::question(this, "Are you sure?", "Delete all objects except the player?") == QMessageBox::Yes)
  {
    for (int i = 0; i <= Highest_object_index; i++) {
      if (Objects[i].type == OBJ_NONE || Objects[i].type == OBJ_ROOM)
        continue;
      if (&Objects[i] == Player_object)
        continue;
      if (Objects[i].type == OBJ_PLAYER)
        continue;
      ObjDelete(i);
    }
    Cur_object_index = -1;
    World_changed = true;
    updateDialog();
  }
}

void ObjectKeypad::onPushThroughWalls(bool checked) {
  f_allow_objects_to_be_pushed_through_walls = checked;
}

void ObjectKeypad::onMoveAxis() { updateDialog(); }
void ObjectKeypad::onAxisX() { setMoveAxis(0); }
void ObjectKeypad::onAxisY() { setMoveAxis(1); }
void ObjectKeypad::onAxisZ() { setMoveAxis(2); }
void ObjectKeypad::onAxisP() { setMoveAxis(3); }
void ObjectKeypad::onAxisH() { setMoveAxis(4); }
void ObjectKeypad::onAxisB() { setMoveAxis(5); }
