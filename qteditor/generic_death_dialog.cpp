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

#include "generic_death_dialog.h"
#include "ui_generic_deaths.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "death_dialog.h"


GenericDeathDialog::GenericDeathDialog(object_info *objinfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::GenericDeathDialog), m_objinfo(objinfo)
{
  ui->setupUi(this);
  m_death_types[0] = objinfo->death_types[0];
  m_prob[0] = objinfo->death_probabilities[0];
  m_death_types[1] = objinfo->death_types[1];
  m_prob[1] = objinfo->death_probabilities[1];
  m_death_types[2] = objinfo->death_types[2];
  m_prob[2] = objinfo->death_probabilities[2];
  m_death_types[3] = objinfo->death_types[3];
  m_prob[3] = objinfo->death_probabilities[3];

  ui->IDC_GENEREIC_DEATH_PROB1->setText(QString::number(m_prob[0]));
  ui->IDC_GENEREIC_DEATH_PROB2->setText(QString::number(m_prob[1]));
  ui->IDC_GENEREIC_DEATH_PROB3->setText(QString::number(m_prob[2]));
  ui->IDC_GENEREIC_DEATH_PROB4->setText(QString::number(m_prob[3]));

  connect(this, &QDialog::accept, this, &GenericDeathDialog::onOk);
  connect(ui->IDC_GENERIC_DEATH_EDIT1, &QPushButton::clicked, this, &GenericDeathDialog::onEdit1);
  connect(ui->IDC_GENERIC_DEATH_EDIT2, &QPushButton::clicked, this, &GenericDeathDialog::onEdit2);
  connect(ui->IDC_GENERIC_DEATH_EDIT3, &QPushButton::clicked, this, &GenericDeathDialog::onEdit3);
  connect(ui->IDC_GENERIC_DEATH_EDIT4, &QPushButton::clicked, this, &GenericDeathDialog::onEdit4);
}

GenericDeathDialog::~GenericDeathDialog() { delete ui; }

void GenericDeathDialog::onEdit1() {
  DeathDialog dlg(&m_death_types[0], this);
  dlg.exec();
}
void GenericDeathDialog::onEdit2() {
  DeathDialog dlg(&m_death_types[1], this);
  dlg.exec();
}
void GenericDeathDialog::onEdit3() {
  DeathDialog dlg(&m_death_types[2], this);
  dlg.exec();
}
void GenericDeathDialog::onEdit4() {
  DeathDialog dlg(&m_death_types[3], this);
  dlg.exec();
}

void GenericDeathDialog::onOk() {
  int total_prob = 0;
  m_prob[0] = ui->IDC_GENEREIC_DEATH_PROB1->text().toInt();
  total_prob += m_prob[0];
  m_prob[1] = ui->IDC_GENEREIC_DEATH_PROB2->text().toInt();
  total_prob += m_prob[1];
  m_prob[2] = ui->IDC_GENEREIC_DEATH_PROB3->text().toInt();
  total_prob += m_prob[2];
  m_prob[3] = ui->IDC_GENEREIC_DEATH_PROB4->text().toInt();
  total_prob += m_prob[3];

  if (total_prob != 100 && total_prob != 0) {
    QMessageBox::warning(this, "Generic Death", "The total of all death probabilities must be 100 or 0.");
    return;
  }

  m_objinfo->death_types[0] = m_death_types[0];
  m_objinfo->death_probabilities[0] = m_prob[0];
  m_objinfo->death_types[1] = m_death_types[1];
  m_objinfo->death_probabilities[1] = m_prob[1];
  m_objinfo->death_types[2] = m_death_types[2];
  m_objinfo->death_probabilities[2] = m_prob[2];
  m_objinfo->death_types[3] = m_death_types[3];
  m_objinfo->death_probabilities[3] = m_prob[3];

  accept();
}

