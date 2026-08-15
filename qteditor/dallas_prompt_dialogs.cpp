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

#include "dallas_prompt_dialogs.h"

#include <QLabel>
#include <QLineEdit>

namespace QtEditor {

DallasGenericPromptDialog::DallasGenericPromptDialog(QWidget *parent)
    : Dialog(":/ui/dallas_generic_prompt_dialog.ui", parent) {}

DallasGenericPromptDialog::~DallasGenericPromptDialog() = default;

void DallasGenericPromptDialog::setDialogTitle(const QString &title) {
  if (!title.isEmpty())
    m_dialog->setWindowTitle(title);
}

void DallasGenericPromptDialog::setPromptText(const QString &text) {
  if (!text.isEmpty()) {
    if (auto *label = find<QLabel>("IDC_GENERIC_PROMPT_STATIC"))
      label->setText(text);
  }
}

void DallasGenericPromptDialog::setPromptData(const QString &data) {
  if (auto *edit = find<QLineEdit>("IDC_GENERIC_DATA_EDIT")) {
    if (!data.isEmpty())
      edit->setText(data);
    edit->selectAll();
  }
}

void DallasGenericPromptDialog::setMaxDataLength(int length) {
  if (auto *edit = find<QLineEdit>("IDC_GENERIC_DATA_EDIT"))
    edit->setMaxLength(length);
}

QString DallasGenericPromptDialog::promptData() const {
  if (auto *edit = find<QLineEdit>("IDC_GENERIC_DATA_EDIT"))
    return edit->text();
  return QString();
}

DallasVectorPromptDialog::DallasVectorPromptDialog(QWidget *parent)
    : Dialog(":/ui/dallas_vector_dialog.ui", parent) {}

DallasVectorPromptDialog::~DallasVectorPromptDialog() = default;

void DallasVectorPromptDialog::setPromptData(float v1, float v2, float v3) {
  find<QLineEdit>("IDC_DATA1_EDIT")->setText(QString::number(v1, 'f', 6));
  find<QLineEdit>("IDC_DATA2_EDIT")->setText(QString::number(v2, 'f', 6));
  find<QLineEdit>("IDC_DATA3_EDIT")->setText(QString::number(v3, 'f', 6));
}

void DallasVectorPromptDialog::getPromptData(float *v1, float *v2, float *v3) {
  *v1 = find<QLineEdit>("IDC_DATA1_EDIT")->text().toFloat();
  *v2 = find<QLineEdit>("IDC_DATA2_EDIT")->text().toFloat();
  *v3 = find<QLineEdit>("IDC_DATA3_EDIT")->text().toFloat();
}

}
