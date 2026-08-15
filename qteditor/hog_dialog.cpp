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

#include "hog_dialog.h"

#include <QProgressBar>
#include <QLabel>
#include <QTableWidget>

namespace QtEditor {

HogDialog::HogDialog(QWidget *parent) : Dialog(":/editor/hogdialog.ui", parent) {
  m_table = find<QTableWidget>("IDC_HOGLIST");
  if (m_table) {
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Filename", "Date", "Length", "Attributes"});
  }
}

HogDialog::~HogDialog() = default;

void HogDialog::addFile(const QString &filename, const QString &date, qint64 length,
                        const QString &attributes) {
  if (!m_table)
    return;
  const int row = m_table->rowCount();
  m_table->insertRow(row);
  m_table->setItem(row, 0, new QTableWidgetItem(filename));
  m_table->setItem(row, 1, new QTableWidgetItem(date));
  m_table->setItem(row, 2, new QTableWidgetItem(QString::number(length)));
  m_table->setItem(row, 3, new QTableWidgetItem(attributes));
}

void HogDialog::clearFiles() {
  if (m_table)
    m_table->setRowCount(0);
}

void HogDialog::setProgress(int percent) {
  if (auto *bar = find<QProgressBar>("IDC_HOGPROGRESS"))
    bar->setValue(percent);
}

void HogDialog::setStatusText(const QString &text) {
  if (auto *label = find<QLabel>("IDC_STATUSTEXT"))
    label->setText(text);
}

}
