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
namespace Ui { class PreferencesDialog; }
QT_END_NAMESPACE


// Port of CPreferencesDialog (IDD_PREFERENCES): editor/game preferences such
// as render mode, renderer, slew speed, joystick and default pilot.
class PreferencesDialog : public QDialog {
  Q_OBJECT
public:
  explicit PreferencesDialog(QWidget *parent = nullptr);
  ~PreferencesDialog();

private slots:
  void onWindowed();
  void onFullScreenSW();
  void onFullScreenHW();
  void onIgcToggled(bool checked);
  void onOk();

private:
  void enableHardwareOptions();
  void disableHardwareOptions();
private:
  Ui::PreferencesDialog *ui;
};

