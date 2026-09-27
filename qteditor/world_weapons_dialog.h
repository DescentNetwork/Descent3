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

#include "weapon.h" // weapon_flags_t
#include "object_external_struct.h" // physics_flags_t
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorldWeaponsDialog; }
QT_END_NAMESPACE


// Port of CWorldWeaponsDialog (IDD_WORLDWEAPONS): edits the weapon table
// (damage, size, life, sounds, flags, spawns, particles, gravity, etc.).
class WorldWeaponsDialog : public QDialog {
  Q_OBJECT
public:
  explicit WorldWeaponsDialog(QWidget *parent = nullptr);
  ~WorldWeaponsDialog();

private slots:
  void onAddWeapon();
  void onDeleteWeapon();
  void onLockWeapon();
  void onCheckinWeapon();
  void onWeaponsOut();
  void onNextWeapon();
  void onPrevWeapon();
  void onWeaponPulldownChanged();
  void onOverride();
  void onCopy();
  void onChangeName();
  void onEditPhysics();

private:
  void updateDialog();
  void saveWeaponsOnClose();
  void bindEdits();
  void bindChecks();
  void bindCombos();
  optref<weapon> data(void);
private:
  Ui::WorldWeaponsDialog *ui;
};

