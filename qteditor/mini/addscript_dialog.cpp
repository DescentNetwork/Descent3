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

#include "addscript_dialog.h"
#include "ui_addscript.h"

#include <QComboBox>
#include <QLineEdit>


// The Win32 CAddScriptDialog enforces DDV_MaxChars(pDX, m_Name, 32) at dismiss
// time. Mirror it on the Qt side as a hard input cap so the dialog can't
// accept a longer name in the first place.
static constexpr int kNameMaxLength = 32;

AddScriptDialog::AddScriptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddScriptDialog)
{
  ui->setupUi(this);
  if (auto *cbox = ui->IDC_TYPESEL) {
    cbox->addItem("object");
    cbox->addItem("trigger");
    cbox->setCurrentIndex(0);
  }
  if (auto *edit = ui->IDC_EDITNAME)
    edit->setMaxLength(kNameMaxLength);
}

AddScriptDialog::~AddScriptDialog() { delete ui; }

QString AddScriptDialog::name() const {
  if (auto *edit = ui->IDC_EDITNAME)
    return edit->text();
  return QString();
}

QString AddScriptDialog::typeName() const {
  if (auto *cbox = ui->IDC_TYPESEL)
    return cbox->currentText();
  return QString();
}

