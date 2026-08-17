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

#pragma once

#include "objinfo.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class GenericDeathDialog; }
QT_END_NAMESPACE


// Port of CGenericDeathDialog (IDD_GENERIC_DEATHS): edits an object type's
// four death behaviors and their probabilities.
class GenericDeathDialog : public QDialog {
  Q_OBJECT
public:
  explicit GenericDeathDialog(object_info *objinfo, QWidget *parent = nullptr);
  ~GenericDeathDialog();

private slots:
  void onEdit1();
  void onEdit2();
  void onEdit3();
  void onEdit4();
  void onOk();

private:
  Ui::GenericDeathDialog *ui;
  object_info *m_objinfo;
  death_info m_death_types[MAX_DEATH_TYPES];
  int m_prob[MAX_DEATH_TYPES];
};

