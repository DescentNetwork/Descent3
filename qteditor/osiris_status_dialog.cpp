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

#include "osiris_status_dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

namespace QtEditor {

OsirisStatusDialog::OsirisStatusDialog(QWidget *parent) : Dialog(":/ui/osiris_status.ui", parent) {
  if (QPushButton *done = find<QPushButton>("IDC_OSIRIS_DONE"))
    connect(done, &QPushButton::clicked, m_dialog, &QDialog::accept);
  if (QLabel *progress = find<QLabel>("IDC_OSIRIS_PROGRESS"))
    progress->setText("Idle");
  if (QTextEdit *err = find<QTextEdit>("IDC_OSIRIS_ERROR"))
    err->setReadOnly(true);
}

OsirisStatusDialog::~OsirisStatusDialog() = default;

void OsirisStatusDialog::setProgress(const QString &text) {
  if (QLabel *progress = find<QLabel>("IDC_OSIRIS_PROGRESS"))
    progress->setText(text);
}

void OsirisStatusDialog::appendError(const QString &text) {
  if (QTextEdit *err = find<QTextEdit>("IDC_OSIRIS_ERROR"))
    err->append(text);
}

void OsirisStatusDialog::done() {
  if (QLabel *progress = find<QLabel>("IDC_OSIRIS_PROGRESS"))
    progress->setText("Done");
}

}
