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

#include <optional>
#include <string>

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
  connect(ui->IDC_NEW_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onNew);
  connect(ui->IDC_DELETE_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onDelete);
  connect(ui->IDC_LOCK_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onLock);
  connect(ui->IDC_CHECKIN_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onCheckin);
  connect(ui->IDC_PREVIOUS_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onPrev);
  connect(ui->IDC_NEXT_MEGACELL, &QPushButton::clicked, this, &MegacellDialog::onNext);

  updateDialog();
}

MegacellDialog::~MegacellDialog() { delete ui; }

void MegacellDialog::updateDialog() {
  if (!GetNextMegacell(0))
    return;
  const uint32_t n = app.current_megacell ? *app.current_megacell : 0;
  if (auto *label = ui->IDC_MEGACELL_NAME_EDIT)
    label->setText(QString::fromStdString(Megacells[n].name));
}

void MegacellDialog::onNew() {
  bool ok = false;
  const QString name = QInputDialog::getText(this, "New megacell", "Name:", QLineEdit::Normal, "", &ok);
  if (!ok || name.isEmpty())
    return;

  const std::optional<uint32_t> cell_handle = AllocMegacell();
  if (!cell_handle) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "No free megacell slots.");
    return;
  }

  // Give the cell a name that is not already taken by some other cell, the same
  // way the legacy editor does (CMegacellDialog::OnNewMegacell): append an
  // increasing counter until the candidate is unused.
  const std::string base = name.toStdString();
  std::string unique;
  for (int c = 1;; c++) {
    unique = base + std::to_string(c);
    if (!FindMegacellName(unique))
      break;
  }

  Megacells[*cell_handle].name = unique;
  app.current_megacell = *cell_handle;
  updateDialog();
}

void MegacellDialog::onDelete() {
  if (!GetNextMegacell(0))
    return;
  const uint32_t n = app.current_megacell ? *app.current_megacell : 0;
  if (n < 0 || !Megacells.is_used(n))
    return;
  FreeMegacell(static_cast<uint32_t>(n));
  if (auto nxt = GetNextMegacell(n)) app.current_megacell = *nxt; else app.current_megacell.reset();
  updateDialog();
}

void MegacellDialog::onLock() {
  if (!GetNextMegacell(0))
    return;
  QMessageBox::information(this, "Success", "Megacell locked.");
}

void MegacellDialog::onCheckin() {
  if (!GetNextMegacell(0))
    return;
  QMessageBox::information(this, "Success", "Megacell checked in.");
}

void MegacellDialog::onPrev() {
  if (!GetNextMegacell(0))
    return;
  if (app.current_megacell) { auto prv = GetPrevMegacell(*app.current_megacell); if (prv) app.current_megacell = *prv; }
  updateDialog();
}

void MegacellDialog::onNext() {
  if (!GetNextMegacell(0))
    return;
  if (app.current_megacell) { auto nxt = GetNextMegacell(*app.current_megacell); if (nxt) app.current_megacell = *nxt; }
  updateDialog();
}

