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

#include "file_page_dialog.h"

#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "manage.h"
#include "pserror.h"

namespace QtEditor {

FilePageDialog::FilePageDialog(QWidget *parent) : Dialog(":/ui/filepage.ui", parent), m_files(nullptr) {
  m_files = find<QListWidget>("IDC_FILELIST");
  if (QPushButton *b = find<QPushButton>("IDC_ADD_FILE"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onAddFile);
  if (QPushButton *b = find<QPushButton>("IDC_DELETE_FILE"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onDeleteFile);
  if (QPushButton *b = find<QPushButton>("IDC_LOCK_FILE"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onLockFile);
  if (QPushButton *b = find<QPushButton>("IDC_CHECKIN_FILE"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onCheckinFile);
  if (QPushButton *b = find<QPushButton>("IDC_FILES_OUT"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onFilesOut);
  if (QPushButton *b = find<QPushButton>("IDC_OVERRIDE"))
    connect(b, &QPushButton::clicked, this, &FilePageDialog::onOverride);

  updateDialog();
}

FilePageDialog::~FilePageDialog() = default;

void FilePageDialog::updateDialog() {
  if (m_files == nullptr)
    return;
  m_files->clear();
  // List the local table files managed by the pagelock system.
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used)
      m_files->addItem(GlobalTrackLocks[i].name);
  }
}

void FilePageDialog::onAddFile() {
  if (!Network_up) {
    OutrageMessageBox("Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }
  QMessageBox::information(this, "Add file", "Select a table file to add (file browser not yet ported).");
}

void FilePageDialog::onDeleteFile() { updateDialog(); }
void FilePageDialog::onLockFile() { updateDialog(); }
void FilePageDialog::onCheckinFile() { updateDialog(); }
void FilePageDialog::onFilesOut() {
  QString str = "Files held locally:\n";
  for (int i = 0; i < MAX_TRACKLOCKS; i++)
    if (GlobalTrackLocks[i].used)
      str += QString("  %1\n").arg(GlobalTrackLocks[i].name);
  QMessageBox::information(this, "Files out", str);
}
void FilePageDialog::onOverride() { updateDialog(); }

}
