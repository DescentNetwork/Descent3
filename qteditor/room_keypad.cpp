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


namespace {
const char *kFlagChecks[][2] = {
    {"IDC_TOUCHES_OUTSIDE", "RF_TOUCHES_TERRAIN"}, {"IDC_SECRET_CHECK", "RF_SECRET"},
    {"IDC_EXTERNAL_ROOM", "RF_EXTERNAL"},         {"IDC_SPECIAL_1", "RF_SPECIAL1"},
    {"IDC_SPECIAL_2", "RF_SPECIAL2"},             {"IDC_SPECIAL_3", "RF_SPECIAL3"},
    {"IDC_SPECIAL_4", "RF_SPECIAL4"},             {"IDC_SPECIAL_5", "RF_SPECIAL5"},
    {"IDC_SPECIAL_6", "RF_SPECIAL6"},             {"IDC_ROOM_SKIP_LIGHTING", "RF_NO_LIGHT"},
    {"IDC_ROOMPAD_REFUELING_CENTER", "RF_FUELCEN"}, {"IDC_ROOMPAD_GOAL1", "RF_GOAL1"},
    {"IDC_ROOMPAD_GOAL2", "RF_GOAL2"},             {"IDC_ROOMPAD_GOAL3", "RF_GOAL3"},
    {"IDC_ROOMPAD_GOAL4", "RF_GOAL4"},             {"IDC_TRIANGULATE", "RF_TRIANGULATE"},
};

uint32_t flagFor(const char *name) {
  for (const auto &c : kFlagChecks)
    if (strcmp(c[0], name) == 0) {
      if (strcmp(c[1], "RF_TOUCHES_TERRAIN") == 0)
        return RF_TOUCHES_TERRAIN;
      if (strcmp(c[1], "RF_SECRET") == 0)
        return RF_SECRET;
      if (strcmp(c[1], "RF_EXTERNAL") == 0)
        return RF_EXTERNAL;
      if (strcmp(c[1], "RF_SPECIAL1") == 0)
        return RF_SPECIAL1;
      if (strcmp(c[1], "RF_SPECIAL2") == 0)
        return RF_SPECIAL2;
      if (strcmp(c[1], "RF_SPECIAL3") == 0)
        return RF_SPECIAL3;
      if (strcmp(c[1], "RF_SPECIAL4") == 0)
        return RF_SPECIAL4;
      if (strcmp(c[1], "RF_SPECIAL5") == 0)
        return RF_SPECIAL5;
      if (strcmp(c[1], "RF_SPECIAL6") == 0)
        return RF_SPECIAL6;
      if (strcmp(c[1], "RF_NO_LIGHT") == 0)
        return RF_NO_LIGHT;
      if (strcmp(c[1], "RF_FUELCEN") == 0)
        return RF_FUELCEN;
      if (strcmp(c[1], "RF_GOAL1") == 0)
        return RF_GOAL1;
      if (strcmp(c[1], "RF_GOAL2") == 0)
        return RF_GOAL2;
      if (strcmp(c[1], "RF_GOAL3") == 0)
        return RF_GOAL3;
      if (strcmp(c[1], "RF_GOAL4") == 0)
        return RF_GOAL4;
      if (strcmp(c[1], "RF_TRIANGULATE") == 0)
        return RF_TRIANGULATE;
    }
  return 0;
}

} // namespace

RoomKeypad::RoomKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::RoomKeypad)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_MARK_ROOM)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onMarkRoom);
  if (QPushButton *b = ui->IDC_ROOMPAD_EXPAND_EDGE)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onExpandEdge);
  if (QPushButton *b = ui->IDC_ROOMPAD_CONTRACT_EDGE)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onContractEdge);
  if (QPushButton *b = ui->IDC_ROOMPAD_EXPAND_FACE)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onExpandFace);
  if (QPushButton *b = ui->IDC_ROOMPAD_CONTRACT_FACE)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onContractFace);
  if (QPushButton *b = ui->IDC_ROOMPAD_EXPAND_ROOM)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onExpandRoom);
  if (QPushButton *b = ui->IDC_ROOMPAD_CONTRACT_ROOM)
    connect(b, &QPushButton::clicked, this, &RoomKeypad::onContractRoom);

  for (const auto &c : kFlagChecks)
    if (QCheckBox *cb = findChild<QCheckBox*>(c[0]))
      connect(cb, &QCheckBox::toggled, this, &RoomKeypad::onFlagToggled);

  updateDialog();
}

RoomKeypad::~RoomKeypad() { delete ui; }

void RoomKeypad::setFlag(uint32_t flag, const char *checkName, bool checked) {
  if (Curroomp == nullptr)
    return;
  if (checked)
    Curroomp->flags |= flag;
  else
    Curroomp->flags &= ~flag;
}

void RoomKeypad::updateDialog() {
  // Win32 disables the room editing controls when no room is current.
  const bool active = (Curroomp != nullptr && Curroomp->used);
  const QList<QWidget *> all = this->findChildren<QWidget *>();
  for (QWidget *w : all)
    if (w->objectName().startsWith("IDC_"))
      w->setEnabled(active);
  if (!active)
    return;

  room *rp = Curroomp;
  if (QLabel *label = ui->IDC_ROOM_NAME)
    label->setText(rp->name ? rp->name : QString("<room %1>").arg(ROOMNUM(rp)));
  if (QLabel *label = ui->IDC_VERTEX_COUNT)
    label->setText(QString("Verts: %1").arg(rp->num_verts));
  if (QLabel *label = ui->IDC_FACE_COUNT)
    label->setText(QString("Faces: %1").arg(rp->num_faces));
  if (QLabel *label = ui->IDC_PORTAL_COUNT)
    label->setText(QString("Portals: %1").arg(rp->num_portals));

  for (const auto &c : kFlagChecks) {
    if (QCheckBox *cb = findChild<QCheckBox*>(c[0]))
      cb->setChecked(rp->flags & flagFor(c[0]));
  }
}

void RoomKeypad::onMarkRoom() {
  if (Curroomp != nullptr)
    Markedroomp = Curroomp;
}

void RoomKeypad::onFlagToggled() {
  QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
  if (cb == nullptr)
    return;
  setFlag(flagFor(cb->objectName().toLatin1().constData()),
          cb->objectName().toLatin1().constData(), cb->isChecked());
  World_changed = 1;
}

void RoomKeypad::expandGeometry(float scale) {
  if (Curroomp == nullptr)
    return;
  room *rp = Curroomp;
  for (int v = 0; v < rp->num_verts; v++)
    rp->verts[v] *= scale;
  World_changed = 1;
}

void RoomKeypad::onExpandEdge() { expandGeometry(1.02f); }
void RoomKeypad::onContractEdge() { expandGeometry(1.0f / 1.02f); }
void RoomKeypad::onExpandFace() { expandGeometry(1.05f); }
void RoomKeypad::onContractFace() { expandGeometry(1.0f / 1.05f); }
void RoomKeypad::onExpandRoom() { expandGeometry(1.1f); }
void RoomKeypad::onContractRoom() { expandGeometry(1.0f / 1.1f); }

