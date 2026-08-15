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

struct otype_wb_info;
struct poly_model;

namespace QtEditor {

// Port of RobotEditWeaponsDialog (IDD_ROBOT_WEAPON_DIALOG): edits a gun
// battery (firing masks, gun points, aiming, anim and usage).
class RobotEditWeaponsDialog : public Dialog {
  Q_OBJECT
public:
  explicit RobotEditWeaponsDialog(otype_wb_info *static_wb, poly_model *pm, QWidget *parent = nullptr);
  ~RobotEditWeaponsDialog() override;

  void getData();

private:
  void loadData();
  void updateDialog();

  otype_wb_info *m_wb;
  poly_model *m_pm;
};

// Opens the RobotEditWeaponsDialog for a weapon battery (used by the player
// weapons dialog).
void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent);

}
