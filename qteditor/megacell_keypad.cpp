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

#include "megacell_keypad.h"
#include "ui_megakeypad.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "d3edit.h"
#include "megacell.h"


MegacellKeypad::MegacellKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::MegacellKeypad)
{
  ui->setupUi(this);
  if (QPushButton *b = ui->IDC_NEXT_MEGA_SET)
    connect(b, &QPushButton::clicked, this, &MegacellKeypad::onNextMegaSet);
  if (QPushButton *b = ui->IDC_PREV_MEGA_SET)
    connect(b, &QPushButton::clicked, this, &MegacellKeypad::onPrevMegaSet);
  if (QCheckBox *cb = ui->IDC_RANDOMIZE_MEGACELL_CHECK) {
    cb->setChecked(D3EditState.randomize_megacell);
    connect(cb, &QCheckBox::toggled, this, &MegacellKeypad::onRandomizeToggled);
  }
  if (QLineEdit *edit = ui->IDC_X_GRANULAR_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &MegacellKeypad::onXGranularEdited);
  if (QLineEdit *edit = ui->IDC_Y_GRANULAR_EDIT)
    connect(edit, &QLineEdit::editingFinished, this, &MegacellKeypad::onYGranularEdited);

  updateDialog();
}

MegacellKeypad::~MegacellKeypad() { delete ui; }

void MegacellKeypad::updateDialog() {
  if (Num_megacells < 1)
    return;
  int n = D3EditState.current_megacell;
  if (!Megacells[n].used) {
    n = GetNextMegacell(n);
    D3EditState.current_megacell = n;
  }
  if (QLabel *label = ui->IDC_MEGACELL_NAME_STATIC)
    label->setText(QString("Megacell name: %1").arg(QString::fromStdString(Megacells[n].name)));
  if (QLabel *label = ui->IDC_MEGA_WIDTH_STATIC)
    label->setText(QString("Width: %1").arg(Megacells[n].width));
  if (QLabel *label = ui->IDC_MEGA_HEIGHT_STATIC)
    label->setText(QString("Height: %1").arg(Megacells[n].height));
  if (QLineEdit *edit = ui->IDC_X_GRANULAR_EDIT)
    edit->setText(QString::number(m_xgran));
  if (QLineEdit *edit = ui->IDC_Y_GRANULAR_EDIT)
    edit->setText(QString::number(m_ygran));
}

void MegacellKeypad::onNextMegaSet() {
  D3EditState.current_megacell = GetNextMegacell(D3EditState.current_megacell);
  m_xgran = m_ygran = 1;
  updateDialog();
}

void MegacellKeypad::onPrevMegaSet() {
  D3EditState.current_megacell = GetPrevMegacell(D3EditState.current_megacell);
  m_xgran = m_ygran = 1;
  updateDialog();
}

void MegacellKeypad::onRandomizeToggled(bool checked) { D3EditState.randomize_megacell = checked; }

void MegacellKeypad::onXGranularEdited() {
  const int n = D3EditState.current_megacell;
  int val = ui->IDC_X_GRANULAR_EDIT->text().toInt();
  if (val < 1)
    val = 1;
  if (val > Megacells[n].width)
    val = Megacells[n].width;
  m_xgran = val;
  updateDialog();
}

void MegacellKeypad::onYGranularEdited() {
  const int n = D3EditState.current_megacell;
  int val = ui->IDC_Y_GRANULAR_EDIT->text().toInt();
  if (val < 1)
    val = 1;
  if (val > Megacells[n].height)
    val = Megacells[n].height;
  m_ygran = val;
  updateDialog();
}

