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

#include "editline_dialog.h"
#include "ui_editlinedlg.h"

#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <cstdio>
#include <cstring>


EditLineDialog::EditLineDialog(const QString &title, const QString &caption,
                               const QString &initial, bool numeric, QWidget *parent)
    : QDialog(parent), ui(new Ui::EditlineDialog)
{
  ui->setupUi(this);
  this->setWindowTitle(title);
  if (auto *prompt = ui->IDC_PROMPT)
    prompt->setText(caption);
  if (auto *edit = ui->IDC_EDIT) {
    if (!initial.isEmpty())
      edit->setText(initial);
    if (numeric)
      edit->setValidator(new QIntValidator(edit));
  }
}

EditLineDialog::EditLineDialog(const QString &caption, QWidget *parent)
    : EditLineDialog(caption, caption, QString(), false, parent)
{
}

EditLineDialog::~EditLineDialog() { delete ui; }

QString EditLineDialog::text() const {
  if (auto *edit = ui->IDC_EDIT)
    return edit->text();
  return QString();
}

bool InputString(char *buf, int maxsize, const char *title, const char *prompt,
                 QWidget *wnd) {
  EditLineDialog dlg(title, prompt, buf, false, wnd);
  if (dlg.exec() == QDialog::Accepted) {
    std::strncpy(buf, dlg.text().toLocal8Bit().constData(), maxsize);
    buf[maxsize - 1] = 0;
    return true;
  }
  return false;
}

bool InputNumber(int *n, const char *title, const char *prompt, QWidget *wnd) {
  EditLineDialog dlg(title, prompt, QString(), true, wnd);
  if (dlg.exec() == QDialog::Accepted) {
    *n = dlg.text().toInt();
    return true;
  }
  return false;
}

