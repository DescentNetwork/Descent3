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

struct physics_info;

QT_BEGIN_NAMESPACE
namespace Ui { class PropertyPhysicsDialog; }
QT_END_NAMESPACE


// Port of CPropertyPhysicsDlg (IDD_PROPPHYSICS): compact physics settings for
// an object type (flags + mass/drag/thrust).
class PropertyPhysicsDialog : public QDialog {
  Q_OBJECT
public:
  explicit PropertyPhysicsDialog(physics_info *physInfo, QWidget *parent = nullptr);
  ~PropertyPhysicsDialog();

private slots:
  void onOk();
  void onFlagToggled();

private:
  void updateDialog();
  void setFlag(uint32_t flag, const char *checkName, bool checked);

  Ui::PropertyPhysicsDialog *ui;
  physics_info *m_physInfo;
};

