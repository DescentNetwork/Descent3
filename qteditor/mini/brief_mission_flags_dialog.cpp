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

#include "brief_mission_flags_dialog.h"
#include "ui_briefmissionflags.h"

#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>


BriefMissionFlagsDialog::BriefMissionFlagsDialog(uint32_t setflags, uint32_t unsetflags,
                                                 QWidget *parent)
    : QDialog(parent), ui(new Ui::BriefMissionFlagsDialog), m_set(setflags), m_unset(unsetflags)
{
  ui->setupUi(this);
  uint32_t bit = 0x1;
  for (int i = 0; i < 32; i++, bit <<= 1) {
    QRadioButton *d = findChild<QRadioButton*>(QString("IDC_D_FLAG%1").arg(i + 1));
    QRadioButton *s = findChild<QRadioButton*>(QString("IDC_S_FLAG%1").arg(i + 1));
    QRadioButton *n = findChild<QRadioButton*>(QString("IDC_N_FLAG%1").arg(i + 1));
    if (!d || !s || !n)
      continue;
    auto *group = new QButtonGroup(this);
    group->addButton(d, 0);
    group->addButton(s, 1);
    group->addButton(n, 2);
    if (m_set & bit)
      s->setChecked(true);
    else if (m_unset & bit)
      n->setChecked(true);
    else
      d->setChecked(true);
  }

  connect(this, &QDialog::accept, this, &BriefMissionFlagsDialog::onOk);
}

BriefMissionFlagsDialog::~BriefMissionFlagsDialog() { delete ui; }

void BriefMissionFlagsDialog::onOk() {
  m_set = 0;
  m_unset = 0;
  uint32_t bit = 0x1;
  for (int i = 0; i < 32; i++, bit <<= 1) {
    auto *s = findChild<QRadioButton*>(QString("IDC_S_FLAG%1").arg(i + 1));
    auto *n = findChild<QRadioButton*>(QString("IDC_N_FLAG%1").arg(i + 1));
    if (s && s->isChecked())
      m_set |= bit;
    else if (n && n->isChecked())
      m_unset |= bit;
  }
  accept();
}

