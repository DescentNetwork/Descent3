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
#include "ui_dallas_vector_dialog.h"
#include "ui_dallas_generic_prompt_dialog.h"

#include <QLabel>
#include <QLineEdit>


DallasGenericPromptDialog::DallasGenericPromptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasGenericPromptDialog)
{
  ui->setupUi(this);
}

DallasGenericPromptDialog::~DallasGenericPromptDialog() { delete ui; }

void DallasGenericPromptDialog::setDialogTitle(const QString &title) {
  if (!title.isEmpty())
    this->setWindowTitle(title);
}

void DallasGenericPromptDialog::setPromptText(const QString &text) {
  if (!text.isEmpty()) {
    if (auto *label = ui->IDC_GENERIC_PROMPT_STATIC)
      label->setText(text);
  }
}

void DallasGenericPromptDialog::setPromptData(const QString &data) {
  if (auto *edit = ui->IDC_GENERIC_DATA_EDIT) {
    if (!data.isEmpty())
      edit->setText(data);
    edit->selectAll();
  }
}

void DallasGenericPromptDialog::setMaxDataLength(int length) {
  if (auto *edit = ui->IDC_GENERIC_DATA_EDIT)
    edit->setMaxLength(length);
}

QString DallasGenericPromptDialog::promptData() const {
  if (auto *edit = ui->IDC_GENERIC_DATA_EDIT)
    return edit->text();
  return QString();
}

DallasVectorPromptDialog::DallasVectorPromptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasVectorPromptDialog)
{
  ui->setupUi(this);
}

DallasVectorPromptDialog::~DallasVectorPromptDialog() { delete ui; }

void DallasVectorPromptDialog::setPromptData(float v1, float v2, float v3) {
  ui->IDC_DATA1_EDIT->setText(QString::number(v1, 'f', 6));
  ui->IDC_DATA2_EDIT->setText(QString::number(v2, 'f', 6));
  ui->IDC_DATA3_EDIT->setText(QString::number(v3, 'f', 6));
}

void DallasVectorPromptDialog::getPromptData(float *v1, float *v2, float *v3) {
  *v1 = ui->IDC_DATA1_EDIT->text().toFloat();
  *v2 = ui->IDC_DATA2_EDIT->text().toFloat();
  *v3 = ui->IDC_DATA3_EDIT->text().toFloat();
}

