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

#include "object.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class PhysicsDialog; }
QT_END_NAMESPACE


// Port of CPhysicsDlg (IDD_PHYSICS): edits a physics_info struct.
class PhysicsDialog : public QDialog {
  Q_OBJECT
public:
  explicit PhysicsDialog(physics_info *physInfo, QWidget *parent = nullptr);
  ~PhysicsDialog();

private slots:
  void onOk();
  void onWiggleCheck();
  void onGravityRadio();
  void onNoGravityRadio();
  void onReverseGravityRadio();
  void onTurnRollCheck();
  void onBouncyCheck();
  void onThrustsCheck();
  void onTerminalEditChanged();
  void onCopy();
  void onPaste();

private:
  void updateTerminalText();
  void enableDisableFullPhysics();
  void enableDisableWiggle();
  void enableDisableBounce();
  void enableDisableTurnRoll();
  void enableDisableThrust();
  void setWidgetEnabled(const char *name, bool enabled);

  void setPhysicsData(const physics_info *physInfo);
  void getPhysicsData(physics_info *physInfo) const;

  Ui::PhysicsDialog *ui;
  physics_info *m_physInfo;
  int m_gravityFlag;
};

