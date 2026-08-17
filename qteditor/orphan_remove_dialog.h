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
namespace Ui { class OrphanRemoveDialog; }
QT_END_NAMESPACE


// Port of COrphanRemoveDlg (IDD_ORPHANHUNTER): scans objects for orphans
// (references to missing object types) and lists them.
class OrphanRemoveDialog : public QDialog {
  Q_OBJECT
public:
  explicit OrphanRemoveDialog(QWidget *parent = nullptr);
  ~OrphanRemoveDialog();

private slots:
  void onScan();

private:
  Ui::OrphanRemoveDialog *ui;
  QListWidget *m_list;
};

