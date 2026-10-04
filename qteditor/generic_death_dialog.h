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

#include <QDialog>
#include "objinfo.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GenericDeathDialog; }
QT_END_NAMESPACE


// Port of CGenericDeathDialog (IDD_GENERIC_DEATHS): edits an object type's
// four death behaviors and their probabilities.
class GenericDeathDialog : public QDialog {
  Q_OBJECT
public:
  explicit GenericDeathDialog(int object_id, QWidget *parent = nullptr);
  ~GenericDeathDialog();

private slots:
  void onEdit1();
  void onEdit2();
  void onEdit3();
  void onEdit4();
  void onOk();

private:
  optref<object_info> data(void);
private:
  Ui::GenericDeathDialog *ui;
  int m_object_id;
  std::array<death_info, MAX_DEATH_TYPES> m_death_types;
  std::array<int, MAX_DEATH_TYPES> m_prob;
};

