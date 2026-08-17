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
namespace Ui { class LevelKeypad; }
QT_END_NAMESPACE


// Port of CLevelDialog (IDD_LEVELKEYPAD): global level physics settings
// (gravity, ceiling height, damage-per-second, default weapon). The Win32
// original is a passive display, so this binds the editable fields to the
// level globals.
class LevelKeypad : public QDialog {
  Q_OBJECT
public:
  explicit LevelKeypad(QWidget *parent = nullptr);
  ~LevelKeypad();

private slots:
  void onGravityEdited();
  void onCeilingEdited();

private:
  void updateDialog();

  template <typename T>
  T *find(const QString &name) const { return findChild<T *>(name); }

  Ui::LevelKeypad *ui;
};

