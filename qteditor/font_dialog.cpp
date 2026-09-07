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

#include "font_dialog.h"
#include "ui_font_dialog.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>


FontDialog::FontDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FontDialog)
{
  ui->setupUi(this);
      connect(ui->IDC_NEW, &QPushButton::clicked, this, [this]() {
    QMessageBox::critical(this, "Unimplemented functionality", "New font: not implemented (font engine pending).");
    });
      connect(ui->IDC_OPEN, &QPushButton::clicked, this, [this]() {
    QMessageBox::critical(this, "Unimplemented functionality", "Open font: not implemented (font engine pending).");
    });
  connect(ui->IDC_SAVE, &QPushButton::clicked, this, &FontDialog::onOk);
  connect(ui->IDC_SAVEAS, &QPushButton::clicked, this, &FontDialog::onOk);

  connect(this, &QDialog::accept, this, &FontDialog::onOk);

  updateDialog();
}

FontDialog::~FontDialog() { delete ui; }

void FontDialog::updateDialog() {
  ui->IDC_EDIT_MINASCII->setText("32");
  ui->IDC_BRIGHTNESS->setText("1.0");
  ui->IDC_EDIT_FONTNAME->setText("(no font)");
}

void FontDialog::onOk() { accept(); }

