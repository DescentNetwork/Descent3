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
  ui->IDC_RANDOMIZE_MEGACELL_CHECK->setChecked(app.randomize_megacell);

  connect(ui->IDC_NEXT_MEGA_SET, &QPushButton::clicked, this, &MegacellKeypad::onNextMegaSet);
  connect(ui->IDC_PREV_MEGA_SET, &QPushButton::clicked, this, &MegacellKeypad::onPrevMegaSet);
  connect(ui->IDC_RANDOMIZE_MEGACELL_CHECK, &QCheckBox::toggled, this, &MegacellKeypad::onRandomizeToggled);
  connect(ui->IDC_X_GRANULAR_EDIT, &QLineEdit::editingFinished, this, &MegacellKeypad::onXGranularEdited);
  connect(ui->IDC_Y_GRANULAR_EDIT, &QLineEdit::editingFinished, this, &MegacellKeypad::onYGranularEdited);

  updateDialog();
}

MegacellKeypad::~MegacellKeypad() { delete ui; }

void MegacellKeypad::updateDialog() {
  if (!GetNextMegacell(0))
    return;
  std::optional<uint32_t> nxt;

  if(Megacells.is_used(app.current_megacell))
    nxt = app.current_megacell;
  else if (nxt = GetNextMegacell(static_cast<uint32_t>(app.current_megacell)))
    app.current_megacell = static_cast<int>(*nxt);

  if(nxt)
  {
    ui->IDC_MEGACELL_NAME_STATIC->setText(QString("Megacell name: %1").arg(QString::fromStdString(Megacells[*nxt].name)));
    ui->IDC_MEGA_WIDTH_STATIC->setText(QString("Width: %1").arg(Megacells[*nxt].width));
    ui->IDC_MEGA_HEIGHT_STATIC->setText(QString("Height: %1").arg(Megacells[*nxt].height));
    ui->IDC_X_GRANULAR_EDIT->setText(QString::number(m_xgran));
    ui->IDC_Y_GRANULAR_EDIT->setText(QString::number(m_ygran));
  }
}

void MegacellKeypad::onNextMegaSet() {
  if (const auto nxt = GetNextMegacell(static_cast<uint32_t>(app.current_megacell)))
    app.current_megacell = static_cast<int>(*nxt);
  m_xgran = m_ygran = 1;
  updateDialog();
}

void MegacellKeypad::onPrevMegaSet() {
  if (const auto prv = GetPrevMegacell(static_cast<uint32_t>(app.current_megacell)))
    app.current_megacell = static_cast<int>(*prv);
  m_xgran = m_ygran = 1;
  updateDialog();
}

void MegacellKeypad::onRandomizeToggled(bool checked) { app.randomize_megacell = checked; }

void MegacellKeypad::onXGranularEdited() {
  const int n = app.current_megacell;
  Q_ASSERT(Megacells.is_used(n));
  int val = ui->IDC_X_GRANULAR_EDIT->text().toInt();
  if (val < 1)
    val = 1;
  if (val > Megacells[n].width)
    val = Megacells[n].width;
  m_xgran = val;
  updateDialog();
}

void MegacellKeypad::onYGranularEdited() {
  const int n = app.current_megacell;
  Q_ASSERT(Megacells.is_used(n));
  int val = ui->IDC_Y_GRANULAR_EDIT->text().toInt();
  if (val < 1)
    val = 1;
  if (val > Megacells[n].height)
    val = Megacells[n].height;
  m_ygran = val;
  updateDialog();
}

