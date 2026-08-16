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

#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include "d3edit.h"
#include "objinfo.h"
#include "object.h"
#include "room.h"

namespace QtEditor {

ObjectKeypad::ObjectKeypad(QWidget *parent) : Keypad(":/ui/objectkeypad.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_OBJPAD_PLACEOBJ"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onPlaceObject);
  if (QPushButton *b = find<QPushButton>("IDC_OBJ_DELOBJ"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onDeleteObject);
  if (QPushButton *b = find<QPushButton>("IDC_OBJPAD_NEXTOBJ"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onNextObject);
  if (QPushButton *b = find<QPushButton>("IDC_OBJPAD_FLIPOBJ"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onFlipObject);
  if (QPushButton *b = find<QPushButton>("IDC_RESET_OBJECTS"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onResetObjects);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEX"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisX);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEY"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisY);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEZ"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisZ);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEP"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisP);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEH"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisH);
  if (QPushButton *b = find<QPushButton>("IDC_OBJMOVEB"))
    connect(b, &QPushButton::clicked, this, &ObjectKeypad::onAxisB);

  updateDialog();
}

ObjectKeypad::~ObjectKeypad() = default;

void ObjectKeypad::setMoveAxis(int axis) {
  D3EditState.object_move_axis = axis;
  updateDialog();
}

void ObjectKeypad::updateDialog() {
  const bool hasObject = (Cur_object_index >= 0 && Cur_object_index <= Highest_object_index &&
                          Objects[Cur_object_index].type != OBJ_NONE);
  const char *names[] = {"IDC_OBJPAD_FLIPOBJ", "IDC_OBJ_DELOBJ", "IDC_OBJPAD_NEXTOBJ"};
  for (const char *name : names)
    if (QWidget *w = find<QWidget>(name))
      w->setEnabled(hasObject);

  // Move axis buttons reflect the current selection axis.
  const struct {
    const char *name;
    int axis;
  } axes[] = {{"IDC_OBJMOVEX", 0}, {"IDC_OBJMOVEY", 1}, {"IDC_OBJMOVEZ", 2},
              {"IDC_OBJMOVEP", 3}, {"IDC_OBJMOVEH", 4}, {"IDC_OBJMOVEB", 5}};
  for (const auto &a : axes)
    if (QPushButton *b = find<QPushButton>(a.name))
      b->setChecked(D3EditState.object_move_axis == a.axis);
}

void ObjectKeypad::onPlaceObject() {
  // Object placement requires a current room (or terrain); the engine's
  // PlaceObject equivalent operates on Cur_object_index.
  if (Curroomp == nullptr)
    return;
  Mine_changed = 1;
}

void ObjectKeypad::onDeleteObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  if (Objects[Cur_object_index].type == OBJ_NONE)
    return;
  ObjDelete(Cur_object_index);
  Cur_object_index = -1;
  Mine_changed = 1;
}

void ObjectKeypad::onNextObject() {
  if (Cur_object_index < 0)
    return;
  for (int i = Cur_object_index + 1; i <= Highest_object_index; i++) {
    if (Objects[i].type != OBJ_NONE && Objects[i].type != OBJ_ROOM) {
      Cur_object_index = i;
      return;
    }
  }
  for (int i = 0; i <= Cur_object_index; i++) {
    if (Objects[i].type != OBJ_NONE && Objects[i].type != OBJ_ROOM) {
      Cur_object_index = i;
      return;
    }
  }
}

void ObjectKeypad::onFlipObject() {
  if (Cur_object_index < 0 || Cur_object_index > Highest_object_index)
    return;
  object *obj = &Objects[Cur_object_index];
  matrix m = obj->orient;
  obj->orient.rvec = -m.rvec;
  obj->orient.uvec = -m.uvec;
  Viewer_moved = 1;
  Mine_changed = 1;
}

void ObjectKeypad::onResetObjects() {
  // Reset all object heights/flags to their type defaults.
  for (int i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type == OBJ_NONE)
      continue;
    const int type = Objects[i].type;
    if (type < 0 || type >= MAX_OBJECT_TYPES || Object_info[type].type == OBJ_NONE)
      continue;
    Objects[i].flags = Object_info[type].flags;
    Objects[i].size = Object_info[type].size;
  }
  Mine_changed = 1;
}

void ObjectKeypad::onMoveAxis() { updateDialog(); }
void ObjectKeypad::onAxisX() { setMoveAxis(0); }
void ObjectKeypad::onAxisY() { setMoveAxis(1); }
void ObjectKeypad::onAxisZ() { setMoveAxis(2); }
void ObjectKeypad::onAxisP() { setMoveAxis(3); }
void ObjectKeypad::onAxisH() { setMoveAxis(4); }
void ObjectKeypad::onAxisB() { setMoveAxis(5); }

}
