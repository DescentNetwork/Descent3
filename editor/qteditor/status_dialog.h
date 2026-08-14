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

#include <cstdint>

#include "fix/fix.h"
#include "qteditor_dialog.h"

namespace QtEditor {

// Port of CStatusDlg (IDD_STATUSDLG): modal-style progress dialog with a
// progress bar and status text.
class StatusDialog : public Dialog {
  Q_OBJECT
public:
  explicit StatusDialog(QWidget *parent = nullptr);
  ~StatusDialog() override;

  int step();
  void init(int min, int max, int delta);
  void text(const QString &s);
  void setTo(int value);

private:
  int m_step = 1;
};

// Port of the CProgress helper around CStatusDlg.
class Progress {
public:
  Progress() = default;
  ~Progress();

  bool initProgress(fix min, fix max, int32_t iterations, QWidget *parent = nullptr);
  bool initProgress(QWidget *parent = nullptr);
  void destroyProgress();
  bool increaseProgress();
  void setProgressText(const QString &string);
  void setProgressPercentage(int percent);
  void setProgressPercentage(float percent);

private:
  int m_Max = 100;
  int m_Min = 0;
  StatusDialog *m_statusDlg = nullptr;
};

}
