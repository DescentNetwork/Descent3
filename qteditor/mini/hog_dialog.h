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

#include <QString>
#include <QDialog>

class QTableWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class HogDialog; }
QT_END_NAMESPACE


// Port of CHogDialog (IDD_HOGDIALOG): shows a .hog file's table of contents.
// Mirrors the Win32 dialog, which sets up Filename / Date / Length /
// Attributes columns and displays the entries of an opened hog file. The
// higher-level hog library helpers are not part of this dialog - they were
// intentionally left commented out in the Win32 source and the column widget
// is read-only. Callers can drive the table incrementally via addFile() /
// clearFiles(), or load an entire hog file in one shot via loadHogFile().
class HogDialog : public QDialog {
  Q_OBJECT
public:
  explicit HogDialog(QWidget *parent = nullptr);
  ~HogDialog();

  // Returns true if loadHogFile() was able to read the header and at least
  // the first entry from the file. False on open / format / read errors.
  bool loadHogFile(const QString &hogname);

  void addFile(const QString &filename, const QString &date, qint64 length,
               const QString &attributes);
  void clearFiles();
  void setProgress(int percent);
  void setStatusText(const QString &text);

private:
  Ui::HogDialog *ui;
  QTableWidget *m_table;
};

