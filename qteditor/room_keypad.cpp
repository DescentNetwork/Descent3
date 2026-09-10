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

#include "room_keypad.h"
#include "ui_roomkeypad.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <cstring>
#include <QLineEdit>
#include <QPushButton>

#include "d3edit.h"
#include "room_external.h"
#include "room.h"

RoomKeypad::RoomKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::RoomKeypad)
{
  ui->setupUi(this);
  connect(ui->IDC_MARK_ROOM, &QPushButton::clicked, this, &RoomKeypad::onMarkRoom);
  connect(ui->IDC_ROOMPAD_EXPAND_EDGE, &QPushButton::clicked, this, &RoomKeypad::onExpandEdge);
  connect(ui->IDC_ROOMPAD_CONTRACT_EDGE, &QPushButton::clicked, this, &RoomKeypad::onContractEdge);
  connect(ui->IDC_ROOMPAD_EXPAND_FACE, &QPushButton::clicked, this, &RoomKeypad::onExpandFace);
  connect(ui->IDC_ROOMPAD_CONTRACT_FACE, &QPushButton::clicked, this, &RoomKeypad::onContractFace);
  connect(ui->IDC_ROOMPAD_EXPAND_ROOM, &QPushButton::clicked, this, &RoomKeypad::onExpandRoom);
  connect(ui->IDC_ROOMPAD_CONTRACT_ROOM, &QPushButton::clicked, this, &RoomKeypad::onContractRoom);

  room *rp = Curroomp;
  connect(ui->IDC_TOUCHES_OUTSIDE, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.touches_terrain = checked; World_changed = true; });
  connect(ui->IDC_SECRET_CHECK, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.secret = checked; World_changed = true; });
  connect(ui->IDC_EXTERNAL_ROOM, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.external = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_1, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special1 = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_2, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special2 = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_3, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special3 = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_4, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special4 = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_5, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special5 = checked; World_changed = true; });
  connect(ui->IDC_SPECIAL_6, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.special6 = checked; World_changed = true; });
  connect(ui->IDC_ROOM_SKIP_LIGHTING, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.no_light = checked; World_changed = true; });
  connect(ui->IDC_ROOMPAD_REFUELING_CENTER, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.fuelcen = checked; World_changed = true; });
  connect(ui->IDC_ROOMPAD_GOAL1, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.goal1 = checked; World_changed = true; });
  connect(ui->IDC_ROOMPAD_GOAL2, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.goal2 = checked; World_changed = true; });
  connect(ui->IDC_ROOMPAD_GOAL3, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.goal3 = checked; World_changed = true; });
  connect(ui->IDC_ROOMPAD_GOAL4, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.goal4 = checked; World_changed = true; });
  connect(ui->IDC_TRIANGULATE, &QCheckBox::toggled, this, [rp](bool checked){ rp->flags.triangulate = checked; World_changed = true; });

  updateDialog();
}

RoomKeypad::~RoomKeypad() { delete ui; }

void RoomKeypad::updateDialog() {
  // Win32 disables the room editing controls when no room is current.
  const bool active = (Curroomp != nullptr && Curroomp->used);
  for (QWidget *w : findChildren<QWidget *>())
    if (w->objectName().startsWith("IDC_"))
      w->setEnabled(active);
  if (active)
  {
    room *rp = Curroomp;
    ui->IDC_ROOM_NAME->setText(rp->name.empty() ? QString("<room %1>").arg(ROOMNUM(rp)) : QString::fromStdString(rp->name));
    ui->IDC_VERTEX_COUNT->setText(QString("Verts: %1").arg(rp->num_verts));
    ui->IDC_FACE_COUNT->setText(QString("Faces: %1").arg(rp->num_faces));
    ui->IDC_PORTAL_COUNT->setText(QString("Portals: %1").arg(rp->num_portals));

    ui->IDC_TOUCHES_OUTSIDE->setChecked(rp->flags.touches_terrain);
    ui->IDC_SECRET_CHECK->setChecked(rp->flags.secret);
    ui->IDC_EXTERNAL_ROOM->setChecked(rp->flags.external);
    ui->IDC_SPECIAL_1->setChecked(rp->flags.special1);
    ui->IDC_SPECIAL_2->setChecked(rp->flags.special2);
    ui->IDC_SPECIAL_3->setChecked(rp->flags.special3);
    ui->IDC_SPECIAL_4->setChecked(rp->flags.special4);
    ui->IDC_SPECIAL_5->setChecked(rp->flags.special5);
    ui->IDC_SPECIAL_6->setChecked(rp->flags.special6);
    ui->IDC_ROOM_SKIP_LIGHTING->setChecked(rp->flags.no_light);
    ui->IDC_ROOMPAD_REFUELING_CENTER->setChecked(rp->flags.fuelcen);
    ui->IDC_ROOMPAD_GOAL1->setChecked(rp->flags.goal1);
    ui->IDC_ROOMPAD_GOAL2->setChecked(rp->flags.goal2);
    ui->IDC_ROOMPAD_GOAL3->setChecked(rp->flags.goal3);
    ui->IDC_ROOMPAD_GOAL4->setChecked(rp->flags.goal4);
    ui->IDC_TRIANGULATE->setChecked(rp->flags.triangulate);
  }

}

void RoomKeypad::onMarkRoom() {
  if (Curroomp != nullptr)
    Markedroomp = Curroomp;
}

void RoomKeypad::expandGeometry(float scale) {
  if (Curroomp == nullptr)
    return;
  room *rp = Curroomp;
  for (int v = 0; v < rp->num_verts; v++)
    rp->verts[v] *= scale;
  World_changed = true;
}

void RoomKeypad::onExpandEdge() { expandGeometry(1.02f); }
void RoomKeypad::onContractEdge() { expandGeometry(1.0f / 1.02f); }
void RoomKeypad::onExpandFace() { expandGeometry(1.05f); }
void RoomKeypad::onContractFace() { expandGeometry(1.0f / 1.05f); }
void RoomKeypad::onExpandRoom() { expandGeometry(1.1f); }
void RoomKeypad::onContractRoom() { expandGeometry(1.0f / 1.1f); }

