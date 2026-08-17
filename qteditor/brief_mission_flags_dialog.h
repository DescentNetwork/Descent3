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
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BriefMissionFlagsDialog; }
QT_END_NAMESPACE


// Port of CBriefMissionFlagsDlg (IDD_BRIEFMISSIONFLAGS): picks which of the 32
// mission-flag bits must be set / must be unset / are irrelevant.
class BriefMissionFlagsDialog : public QDialog {
  Q_OBJECT
public:
  explicit BriefMissionFlagsDialog(uint32_t setflags, uint32_t unsetflags,
                                   QWidget *parent = nullptr);
  ~BriefMissionFlagsDialog();

  uint32_t setFlags() const { return m_set; }
  uint32_t unsetFlags() const { return m_unset; }

private slots:
  void onOk();

private:
  Ui::BriefMissionFlagsDialog *ui;

  uint32_t m_set;
  uint32_t m_unset;
};

