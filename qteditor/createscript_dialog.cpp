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

#include "createscript_dialog.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

namespace QtEditor {

CreateNewScriptDialog::CreateNewScriptDialog(QWidget *parent)
    : Dialog(":/ui/createscript.ui", parent) {
  if (auto *level = find<QRadioButton>("IDC_LEVEL"))
    level->setChecked(true);

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &CreateNewScriptDialog::onOk);
  }
}

CreateNewScriptDialog::~CreateNewScriptDialog() = default;

QString CreateNewScriptDialog::filename() const {
  if (auto *edit = find<QLineEdit>("IDC_FILENAME"))
    return edit->text();
  return QString();
}

int CreateNewScriptDialog::scriptType() const {
  if (auto *level = find<QRadioButton>("IDC_LEVEL"))
    return level->isChecked() ? 0 : 1;
  return 0;
}

void CreateNewScriptDialog::onOk() {
  if (auto *edit = find<QLineEdit>("IDC_FILENAME")) {
    QString name = edit->text();
    if (name.isEmpty()) {
      QMessageBox::warning(this, "Error", "You must specify a filename");
      return;
    }
    if (name.length() <= 4 || !name.endsWith(".cpp", Qt::CaseInsensitive)) {
      name += ".cpp";
      edit->setText(name);
    }
  }
  accept();
}

}
