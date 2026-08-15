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

class QTableWidget;

namespace QtEditor {

// Port of CHogDialog (IDD_HOGDIALOG): displays the contents of a hog file.
// The MFC original's hogfile helpers are commented out; this currently sets up
// the file table (Filename/Date/Length/Attributes columns).
class HogDialog : public Dialog {
  Q_OBJECT
public:
  explicit HogDialog(QWidget *parent = nullptr);
  ~HogDialog() override;

  void addFile(const QString &filename, const QString &date, qint64 length,
               const QString &attributes);
  void clearFiles();
  void setProgress(int percent);
  void setStatusText(const QString &text);

private:
  QTableWidget *m_table;
};

}
