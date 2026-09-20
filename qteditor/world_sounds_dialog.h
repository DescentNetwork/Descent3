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
#include "ssl_lib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WorldSoundsDialog; }
QT_END_NAMESPACE


// Port of CWorldSoundsDialog (IDD_WORLDSOUNDS): edits the sound table
// (sample, distances, cone, loop points, flags, pagelock operations).
class WorldSoundsDialog : public QDialog {
  Q_OBJECT
public:
  explicit WorldSoundsDialog(QWidget *parent = nullptr);
  ~WorldSoundsDialog();

private slots:
  void onAddSound();
  void onLoadSound();
  void onNextSound();
  void onPrevSound();
  void onDeleteSound();
  void onLockSound();
  void onCheckinSound();
  void onPlaysound();
  void onKillsounds();
  void onOverride();
  void onChangeName();
  void onSoundPulldownChanged();

  void onMaxDistEdited();
  void onMinDistEdited();
  void onInnerConeEdited();
  void onOuterConeAngleEdited();
  void onOuterConeVolEdited();
  void onLoopStartEdited();
  void onLoopEndEdited();
  void onImportVolumeEdited();

private:
  void updateDialog();
  void saveSoundsOnClose();
private:
  Ui::WorldSoundsDialog *ui;
  sound_info* m_snd = nullptr;
  sound_flags_t m_sf;
};

