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


#include "pserror.h"


FontDialog::FontDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FontDialog)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_NEW)
    connect(b, &QPushButton::clicked, this, [this]() {
      OutrageMessageBox("New font: not implemented (font engine pending).");
    });
  if (QPushButton *b = ui->IDC_OPEN)
    connect(b, &QPushButton::clicked, this, [this]() {
      OutrageMessageBox("Open font: not implemented (font engine pending).");
    });
  if (QPushButton *save = ui->IDC_SAVE)
    connect(save, &QPushButton::clicked, this, &FontDialog::onOk);
  if (QPushButton *saveAs = ui->IDC_SAVEAS)
    connect(saveAs, &QPushButton::clicked, this, &FontDialog::onOk);

  connect(this, &QDialog::accept, this, &FontDialog::onOk);

  updateDialog();
}

FontDialog::~FontDialog() { delete ui; }

void FontDialog::updateDialog() {
  if (QLineEdit *e = ui->IDC_EDIT_MINASCII)
    e->setText("32");
  if (QLineEdit *e = ui->IDC_BRIGHTNESS)
    e->setText("1.0");
  if (QLineEdit *e = ui->IDC_EDIT_FONTNAME)
    e->setText("(no font)");
}

void FontDialog::onOk() { accept(); }

