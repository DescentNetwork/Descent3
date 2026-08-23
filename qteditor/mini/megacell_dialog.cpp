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

#include "megacell_dialog.h"
#include "ui_megacell.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>

#include "d3edit.h"
#include "manage.h"
#include "megacell.h"



MegacellDialog::MegacellDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::MegacellDialog)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_NEW_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onNew);
  if (QPushButton *b = ui->IDC_DELETE_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onDelete);
  if (QPushButton *b = ui->IDC_LOCK_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onLock);
  if (QPushButton *b = ui->IDC_CHECKIN_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onCheckin);
  if (QPushButton *b = ui->IDC_PREVIOUS_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onPrev);
  if (QPushButton *b = ui->IDC_NEXT_MEGACELL)
    connect(b, &QPushButton::clicked, this, &MegacellDialog::onNext);

  updateDialog();
}

MegacellDialog::~MegacellDialog() { delete ui; }

void MegacellDialog::updateDialog() {
  if (Num_megacells < 1)
    return;
  const int n = D3EditState.current_megacell;
  if (auto *label = ui->IDC_MEGACELL_NAME_EDIT)
    label->setText(Megacells[n].name);
}

void MegacellDialog::onNew() {
  bool ok = false;
  const QString name = QInputDialog::getText(this, "New megacell", "Name:", QLineEdit::Normal, "", &ok);
  if (!ok || name.isEmpty())
    return;
  for (int i = 0; i < MAX_MEGACELLS; i++) {
    if (!Megacells[i].used) {
      snprintf(Megacells[i].name, sizeof(Megacells[i].name), "%s", name.toLocal8Bit().constData());
      Megacells[i].used = true;
      Num_megacells++;
      D3EditState.current_megacell = i;
      updateDialog();
      return;
    }
  }
  QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No free megacell slots.");
}

void MegacellDialog::onDelete() {
  if (Num_megacells < 1)
    return;
  const int n = D3EditState.current_megacell;
  Megacells[n].used = false;
  Num_megacells--;
  D3EditState.current_megacell = GetNextMegacell(n);
  updateDialog();
}

void MegacellDialog::onLock() {
  if (Num_megacells < 1)
    return;
  QMessageBox::information(this, "Success", "Megacell locked.");
}

void MegacellDialog::onCheckin() {
  if (Num_megacells < 1)
    return;
  QMessageBox::information(this, "Success", "Megacell checked in.");
}

void MegacellDialog::onPrev() {
  if (Num_megacells < 1)
    return;
  D3EditState.current_megacell = GetPrevMegacell(D3EditState.current_megacell);
  updateDialog();
}

void MegacellDialog::onNext() {
  if (Num_megacells < 1)
    return;
  D3EditState.current_megacell = GetNextMegacell(D3EditState.current_megacell);
  updateDialog();
}

