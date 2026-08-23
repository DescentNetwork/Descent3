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

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class SelectRangeDialog; }
QT_END_NAMESPACE


class SelectRangeDialog : public QDialog {
  Q_OBJECT
public:
  explicit SelectRangeDialog(QWidget *parent = nullptr);
  ~SelectRangeDialog();

private slots:
  void onLowerBoundChanged();
  void onUpperBoundChanged();
  void onSlopeChanged();
  void onSelectSlopeToggled(bool enabled);
  void onOk();

private:
  void clampAndStore(const char *editName, int *target);
  void runSelection();

  Ui::SelectRangeDialog *ui;
  int m_lowerBound = 0;
  int m_upperBound = 0;
  int m_slopeAngle = 0;
};

