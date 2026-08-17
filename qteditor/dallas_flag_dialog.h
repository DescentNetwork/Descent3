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

QT_BEGIN_NAMESPACE
namespace Ui { class DallasFlagDialog; }
QT_END_NAMESPACE

class QListWidget;


// Port of CDallasFlagDlg (IDD_DALLAS_FLAG_DIALOG): multi-select a set of
// flags for a Dallas (script) value.
class DallasFlagDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasFlagDialog(const QString &prompt, const QStringList &flags,
                            const QStringList &checked = {}, QWidget *parent = nullptr);
  ~DallasFlagDialog();

  QStringList checkedFlags() const;

private:
  Ui::DallasFlagDialog *ui;
  QListWidget *m_list;
};

