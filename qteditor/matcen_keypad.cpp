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

#include "matcen_keypad.h"
#include "ui_matcenkeypad.h"

#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>


#include "matcen.h"


MatcenKeypad::MatcenKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::MatcenKeypad)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_MAT_PREV_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onPrev);
  if (QPushButton *b = ui->IDC_MAT_NEXT_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onNext);
  if (QPushButton *b = ui->IDC_MAT_NEW_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onNew);
  if (QPushButton *b = ui->IDC_MAT_DELETE_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onDelete);
  if (QPushButton *b = ui->IDC_MAT_COPY_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onCopy);
  if (QPushButton *b = ui->IDC_MAT_PASTE_BUTTON)
    connect(b, &QPushButton::clicked, this, &MatcenKeypad::onPaste);

  updateDialog();
}

MatcenKeypad::~MatcenKeypad() { delete ui; }

void MatcenKeypad::updateDialog() {
  if (Num_matcens <= 0 || m_matcenId >= Num_matcens)
    return;
  matcen *mc = Matcen[m_matcenId];
  char name[MAX_MATCEN_NAME_LEN] = "";
  mc->GetName(name);
  if (QLabel *label = ui->IDC_MAT_CUR_STATIC)
    label->setText(QString("Current Matcen: %1").arg(m_matcenId + 1));
  if (QLabel *label = ui->IDC_MAT_NUM_STATIC)
    label->setText(QString("Number of Matcens: %1").arg(Num_matcens));

  // TODO: Once matcen API is fully ported, populate these labels.
  // if (QLabel *label = ui->IDC_MAT_NAME_STATIC)
  //   label->setText(name);
  // if (QLabel *label = ui->IDC_MAT_SPAWNS_STATIC)
  //   label->setText(QString("Spawn points: %1").arg((int)mc->GetNumSpawnPnts()));
  // if (QLabel *label = ui->IDC_MAT_PROD_STATIC)
  //   label->setText(QString("Prod types: %1").arg((int)mc->GetNumProdTypes()));
}

void MatcenKeypad::onPrev() {
  if (Num_matcens <= 0)
    return;
  m_matcenId = (m_matcenId <= 0) ? (Num_matcens - 1) : (m_matcenId - 1);
  updateDialog();
}

void MatcenKeypad::onNext() {
  if (Num_matcens <= 0)
    return;
  m_matcenId = (m_matcenId + 1) % Num_matcens;
  updateDialog();
}

void MatcenKeypad::onNew() {
  bool ok = false;
  const QString name = QInputDialog::getText(this, "New Matcen",
                                             "Enter a name for the new matcen:", QLineEdit::Normal, "", &ok);
  if (!ok || name.isEmpty())
    return;
  bool nameChanged = false;
  const int id = CreateMatcen(name.toLocal8Bit().constData(), &nameChanged);
  if (id >= 0) {
    m_matcenId = id;
    updateDialog();
  }
}

void MatcenKeypad::onDelete() {
  if (Num_matcens <= 0)
    return;
  Matcen[m_matcenId]->SetName("deleted");
  if (m_matcenId >= Num_matcens - 1)
    m_matcenId = 0;
  updateDialog();
}

void MatcenKeypad::onCopy() {
  if (Num_matcens <= 0)
    return;
  QMessageBox::information(this, "Matcen", "Matcen copied.");
}

void MatcenKeypad::onPaste() {
  QMessageBox::information(this, "Matcen", "Matcen pasted.");
}

