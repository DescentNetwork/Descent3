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

#include "osiris_status_dialog.h"
#include "ui_osiris_status.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>


OsirisStatusDialog::OsirisStatusDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::OsirisStatusDialog)
{
  ui->setupUi(this);
  connect(ui->IDC_OSIRIS_DONE, &QPushButton::clicked, this, &QDialog::accept);
  ui->IDC_OSIRIS_PROGRESS->setText("Idle");
  ui->IDC_OSIRIS_ERROR->setReadOnly(true);
}

OsirisStatusDialog::~OsirisStatusDialog() { delete ui; }

void OsirisStatusDialog::setProgress(const QString &text) {
  ui->IDC_OSIRIS_PROGRESS->setText(text);
}

void OsirisStatusDialog::appendError(const QString &text) {
  ui->IDC_OSIRIS_ERROR->append(text);
}

void OsirisStatusDialog::done() {
  ui->IDC_OSIRIS_PROGRESS->setText("Done");
}

