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

class QListWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class DallasUserTypesDialog; }
QT_END_NAMESPACE


// Port of CDallasUserTypesDlg (IDD_DALLAS_USERTYPES_DIALOG): manage Dallas
// user type enum values (add/change/delete).
class DallasUserTypesDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasUserTypesDialog(QWidget *parent = nullptr);
  ~DallasUserTypesDialog();

private slots:
  void onAdd();
  void onChange();
  void onDelete();

private:
  void updateDialog();
  Ui::DallasUserTypesDialog *ui;
  QListWidget *m_list;
};

