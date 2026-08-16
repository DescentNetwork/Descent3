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

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "d3edit.h"
#include "pserror.h"

namespace QtEditor {

FontDialog::FontDialog(QWidget *parent) : Dialog(":/ui/font_dialog.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_NEW"))
    connect(b, &QPushButton::clicked, this, [this]() {
      OutrageMessageBox("New font: not implemented (font engine pending).");
    });
  if (QPushButton *b = find<QPushButton>("IDC_OPEN"))
    connect(b, &QPushButton::clicked, this, [this]() {
      OutrageMessageBox("Open font: not implemented (font engine pending).");
    });
  if (QPushButton *save = find<QPushButton>("IDC_SAVE"))
    connect(save, &QPushButton::clicked, this, &FontDialog::onOk);
  if (QPushButton *saveAs = find<QPushButton>("IDC_SAVEAS"))
    connect(saveAs, &QPushButton::clicked, this, &FontDialog::onOk);
  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &FontDialog::onOk);
  }

  updateDialog();
}

FontDialog::~FontDialog() = default;

void FontDialog::updateDialog() {
  if (QLineEdit *e = find<QLineEdit>("IDC_EDIT_MINASCII"))
    e->setText("32");
  if (QLineEdit *e = find<QLineEdit>("IDC_BRIGHTNESS"))
    e->setText("1.0");
  if (QLineEdit *e = find<QLineEdit>("IDC_EDIT_FONTNAME"))
    e->setText("(no font)");
}

void FontDialog::onOk() { accept(); }

}
