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

#include <QComboBox>
#include <QLineEdit>

namespace QtEditor {

AddScriptDialog::AddScriptDialog(QWidget *parent) : Dialog(":/ui/addscript.ui", parent) {
  if (auto *cbox = find<QComboBox>("IDC_TYPESEL")) {
    cbox->addItem("object");
    cbox->addItem("trigger");
    cbox->setCurrentIndex(0);
  }
}

AddScriptDialog::~AddScriptDialog() = default;

QString AddScriptDialog::name() const {
  if (auto *edit = find<QLineEdit>("IDC_EDITNAME"))
    return edit->text();
  return QString();
}

QString AddScriptDialog::typeName() const {
  if (auto *cbox = find<QComboBox>("IDC_TYPESEL"))
    return cbox->currentText();
  return QString();
}

}
