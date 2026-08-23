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

#include "game.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class TerrainSoundDialog; }
QT_END_NAMESPACE


// Port of CTerrainSoundDialog (IDD_TERRAIN_SOUND_DIALOG): edits the five
// terrain sound bands (sound, low/high altitude, low/high volume) for the
// current level.
class TerrainSoundDialog : public QDialog {
  Q_OBJECT
public:
  explicit TerrainSoundDialog(QWidget *parent = nullptr);
  ~TerrainSoundDialog();

private slots:
  void onNext();
  void onPrev();
  void onSoundChanged();
  void onOk();

private:
  void updateDialog();
  void copyToControls();
  bool copyFromControls();

  Ui::TerrainSoundDialog *ui;
  terrain_sound_band m_bands[NUM_TERRAIN_SOUND_BANDS];
  int m_current = 0;
};

