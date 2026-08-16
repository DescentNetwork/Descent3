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

#include "qteditor_dialog.h"

class QListWidget;

namespace QtEditor {

// Port of CFilePageDialog (IDD_FILEPAGE): manages the level's table files
// (add/lock/checkin/delete/override).
class FilePageDialog : public Dialog {
  Q_OBJECT
public:
  explicit FilePageDialog(QWidget *parent = nullptr);
  ~FilePageDialog() override;

private slots:
  void onAddFile();
  void onDeleteFile();
  void onLockFile();
  void onCheckinFile();
  void onFilesOut();
  void onOverride();

private:
  void updateDialog();
  QListWidget *m_files;
};

}
