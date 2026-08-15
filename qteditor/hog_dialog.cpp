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

#include <cstdio>
#include <ctime>

#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>

#include "posix_stream.h"
#include "hog2_format.h"

namespace QtEditor {

namespace {

// hog2::archive_t's entry_t::timestamp is unix seconds in newer hogs and
// DOS-seconds-since-1980 in older ones; render both as yyyy-MM-dd HH:mm:ss
// in the Date column.
QString formatTimestamp(uint32_t ts) {
  const qint64 unix_seconds = (ts < 7300U * 24U * 3600U)
                                  ? static_cast<qint64>(ts) + 315532800LL
                                  : static_cast<qint64>(ts);
  const time_t secs = static_cast<time_t>(unix_seconds);
  std::tm tm{};
  gmtime_r(&secs, &tm);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
  return QString::fromLatin1(buf);
}

} // namespace

HogDialog::HogDialog(QWidget *parent) : Dialog(":/ui/hogdialog.ui", parent) {
  m_table = find<QTableWidget>("IDC_HOGLIST");
  if (m_table) {
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Filename", "Date", "Length", "Attributes"});
  }
}

HogDialog::~HogDialog() = default;

bool HogDialog::loadHogFile(const QString &hogname) {
  if (m_table == nullptr)
    return false;
  clearFiles();
  if (hogname.isEmpty())
    return false;

  const std::string path = hogname.toStdString();
  posix_istream input;
  errno = 0;
  if (!input.open(path, std::ios_base::in | std::ios_base::binary)) {
    setStatusText(QString("Failed to open %1").arg(hogname));
    return false;
  }

  hog2::archive_t archive;
  try {
    input >> archive;
  } catch (const std::invalid_argument &) {
    setStatusText(QString("Not a hog file: %1").arg(hogname));
    return false;
  }

  const std::size_t total = std::distance(archive.begin(), archive.end());
  if (total == 0) {
    setStatusText(QString("Empty hog file: %1").arg(hogname));
    return false;
  }
  setStatusText(QString("Loaded %1 (%2 entries)")
                    .arg(QFileInfo(hogname).fileName())
                    .arg(static_cast<qulonglong>(total)));
  std::size_t i = 0;
  for (auto it = archive.begin(); it != archive.end(); ++it, ++i) {
    const QString name = QString::fromStdString(std::string(it->name.string()));
    const QString date =
        (it->timestamp != 0) ? formatTimestamp(it->timestamp) : QStringLiteral("-");
    addFile(name, date, it->len, QString::number(it->flags));
    if (total > 0)
      setProgress(static_cast<int>((i + 1) * 100 / total));
  }
  if (total > 0)
    setProgress(100);
  return true;
}

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
