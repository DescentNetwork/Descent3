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

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "death_dialog.h"

namespace QtEditor {

GenericDeathDialog::GenericDeathDialog(object_info *objinfo, QWidget *parent)
    : Dialog(":/editor/generic_deaths.ui", parent), m_objinfo(objinfo) {
  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    m_death_types[i] = objinfo->death_types[i];
    m_prob[i] = objinfo->death_probabilities[i];
  }

  const char *probNames[MAX_DEATH_TYPES] = {"IDC_GENEREIC_DEATH_PROB1", "IDC_GENEREIC_DEATH_PROB2",
                                            "IDC_GENEREIC_DEATH_PROB3", "IDC_GENEREIC_DEATH_PROB4"};
  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    if (QLineEdit *edit = find<QLineEdit>(probNames[i]))
      edit->setText(QString::number(m_prob[i]));
  }

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &GenericDeathDialog::onOk);
  }
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEATH_EDIT1"))
    connect(b, &QPushButton::clicked, this, &GenericDeathDialog::onEdit1);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEATH_EDIT2"))
    connect(b, &QPushButton::clicked, this, &GenericDeathDialog::onEdit2);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEATH_EDIT3"))
    connect(b, &QPushButton::clicked, this, &GenericDeathDialog::onEdit3);
  if (QPushButton *b = find<QPushButton>("IDC_GENERIC_DEATH_EDIT4"))
    connect(b, &QPushButton::clicked, this, &GenericDeathDialog::onEdit4);
}

GenericDeathDialog::~GenericDeathDialog() = default;

void GenericDeathDialog::onEdit1() {
  DeathDialog dlg(&m_death_types[0], m_dialog);
  dlg.exec();
}
void GenericDeathDialog::onEdit2() {
  DeathDialog dlg(&m_death_types[1], m_dialog);
  dlg.exec();
}
void GenericDeathDialog::onEdit3() {
  DeathDialog dlg(&m_death_types[2], m_dialog);
  dlg.exec();
}
void GenericDeathDialog::onEdit4() {
  DeathDialog dlg(&m_death_types[3], m_dialog);
  dlg.exec();
}

void GenericDeathDialog::onOk() {
  const char *probNames[MAX_DEATH_TYPES] = {"IDC_GENEREIC_DEATH_PROB1", "IDC_GENEREIC_DEATH_PROB2",
                                            "IDC_GENEREIC_DEATH_PROB3", "IDC_GENEREIC_DEATH_PROB4"};
  int total_prob = 0;
  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    m_prob[i] = find<QLineEdit>(probNames[i])->text().toInt();
    total_prob += m_prob[i];
  }

  if (total_prob != 100 && total_prob != 0) {
    QMessageBox::warning(m_dialog, "Generic Death", "The total of all death probabilities must be 100 or 0.");
    return;
  }

  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    m_objinfo->death_types[i] = m_death_types[i];
    m_objinfo->death_probabilities[i] = m_prob[i];
  }

  m_dialog->accept();
}

}
