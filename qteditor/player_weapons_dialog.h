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

namespace QtEditor {

// Port of the MFC PlayerWeaponsDialog (IDD_PLAYER_WEAPONS): edits the weapon
// batteries of a player ship.
class PlayerWeaponsDialog : public Dialog {
  Q_OBJECT
public:
  explicit PlayerWeaponsDialog(int current_ship, QWidget *parent = nullptr);
  ~PlayerWeaponsDialog() override;

private slots:
  void onEditWbButton();
  void onCurrentWeaponChanged();
  void onFiresFusion();
  void onContinuousFiringSound(bool checked);
  void onFiringReleaseSound(bool checked);
  void onFiringSoundChanged();
  void onReleaseSoundChanged();
  void onSpewPowerupChanged();
  void onOnOff();
  void onZoom();
  void onMaxAmmoEdited();
  void onShowTenths(bool checked);

private:
  int currentWBIndex() const;
  void updateDialog();

  int m_current_ship;
  QString m_current_wb_text;
};

// Opens the PlayerWeaponsDialog for the given ship (used by the world objects
// player dialog's "Edit weapons" button).
void editPlayerWeapons(int shipHandle, QWidget *parent);

}
