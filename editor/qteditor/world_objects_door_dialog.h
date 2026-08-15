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

// Port of CWorldObjectsDoorDialog (IDD_WORLDOBJECTSDOOR): edits the predefined
// door table (open/stay/close times, transparency, blastability, hit points,
// open/close sounds and script module).
class WorldObjectsDoorDialog : public Dialog {
  Q_OBJECT
public:
  explicit WorldObjectsDoorDialog(QWidget *parent = nullptr);
  ~WorldObjectsDoorDialog() override;

private slots:
  void onAddDoor();
  void onDeleteDoor();
  void onLockDoor();
  void onCheckinDoor();
  void onDoorsOut();
  void onDoorNext();
  void onDoorPrev();
  void onDoorPulldownChanged();
  void onKillfocusOpenTime();
  void onKillfocusStaysOpen();
  void onKillfocusCloseTime();
  void onKillfocusHitpoints();
  void onTransparencyToggled(bool checked);
  void onBlastableToggled(bool checked);
  void onOpenSoundChanged();
  void onCloseSoundChanged();
  void onBrowse();
  void onKillfocusScriptname();
  void onChangeName();

private:
  void updateDialog();
};

}
