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
#include "ui_filepage.h"

#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "manage.h"



FilePageDialog::FilePageDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FilePageDialog), m_files(nullptr)
{
  ui->setupUi(this);
  m_files = ui->IDC_FILELIST;
  connect(ui->IDC_ADD_FILE, &QPushButton::clicked, this, &FilePageDialog::onAddFile);
  connect(ui->IDC_DELETE_FILE, &QPushButton::clicked, this, &FilePageDialog::onDeleteFile);
  connect(ui->IDC_LOCK_FILE, &QPushButton::clicked, this, &FilePageDialog::onLockFile);
  connect(ui->IDC_CHECKIN_FILE, &QPushButton::clicked, this, &FilePageDialog::onCheckinFile);
  connect(ui->IDC_FILES_OUT, &QPushButton::clicked, this, &FilePageDialog::onFilesOut);
  connect(ui->IDC_OVERRIDE, &QPushButton::clicked, this, &FilePageDialog::onOverride);

  updateDialog();
}

FilePageDialog::~FilePageDialog() { delete ui; }

void FilePageDialog::updateDialog() {
  if (m_files == nullptr)
    return;
  m_files->clear();
  // List the local table files managed by the pagelock system.
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used)
      m_files->addItem(QString::fromStdString(GlobalTrackLocks[i].name));
  }
}

void FilePageDialog::onAddFile() {
  QMessageBox::information(this, "Add file", "Select a table file to add (file browser not yet ported).");
}

void FilePageDialog::onDeleteFile() { updateDialog(); }
void FilePageDialog::onLockFile() { updateDialog(); }
void FilePageDialog::onCheckinFile() { updateDialog(); }
void FilePageDialog::onFilesOut() {
  QString str = "Files held locally:\n";
  for (int i = 0; i < MAX_TRACKLOCKS; i++)
    if (GlobalTrackLocks[i].used)
      str += QString("  %1\n").arg(QString::fromStdString(GlobalTrackLocks[i].name));
  QMessageBox::information(this, "Files out", str);
}
void FilePageDialog::onOverride() { updateDialog(); }

