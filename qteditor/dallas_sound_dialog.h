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
namespace Ui { class DallasSoundDialog; }
QT_END_NAMESPACE


// Port of CDallasSoundDlg (IDD_DALLAS_SOUND_DIALOG): pick a game sound, with
// play/stop preview.
class DallasSoundDialog : public QDialog {
  Q_OBJECT
public:
  explicit DallasSoundDialog(QWidget *parent = nullptr);
  ~DallasSoundDialog();

  int selectedSound() const;

private slots:
  void onPlay();
  void onStop();

private:
  Ui::DallasSoundDialog *ui;
  QListWidget *m_list;
};

