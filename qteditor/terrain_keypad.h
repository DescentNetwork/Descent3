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
namespace Ui { class TerrainKeypad; }
QT_END_NAMESPACE

class QLabel;

class TerrainKeypad : public QDialog {
  Q_OBJECT
public:
  explicit TerrainKeypad(QWidget *parent = nullptr);
  ~TerrainKeypad();

private slots:
  void onMoveUp();
  void onMoveDown();
  void onRaise10();
  void onLower10();
  void onSelectNone();
  void onSelectAll();
  void onMakeMax();
  void onMakeMin();
  void onMakeZero();
  void onFillArea();
  void onMoreMoons();
  void onLessMoons();
  void onNextMoon();
  void onPrevMoon();
  void onMoveSatUp();
  void onMoveSatDown();
  void onMoveSatLeft();
  void onMoveSatRight();
  void onMoveMoonCloser();
  void onMoveMoonFarther();
  void onRenormalize();
  void onRotTexture();
  void onPyramid();
  void onPancakes();
  void onSmoothTerrain();
  void onDropTerrain();
  void onRandomizeSky();
  void onSkyNearer();
  void onSkyFarther();
  void onTileMore();
  void onTileLess();
  void onRedoTopmap();
  void onTerrainOcclusion();
  void onToggleVisibility();

  void onSkyRadio();
  void onHorizonRadio();
  void onFogRadio();
  void onSatelliteRadio();
  void onSkyRedEdited();
  void onSkyGreenEdited();
  void onSkyBlueEdited();
  void onFogDistanceEdited();
  void onPixelErrorEdited();
  void onFogScalarEdited();
  void onDamagePerSecEdited();
  void onRotateSpeedEdited();

  void onStarsToggled(bool checked);
  void onSatelliteToggled(bool checked);
  void onTexturedSkyToggled(bool checked);
  void onUseFogToggled(bool checked);
  void onUseHaloToggled(bool checked);
  void onUseAtmosphereToggled(bool checked);
  void onRotateStarsToggled(bool checked);
  void onRotateSkyToggled(bool checked);
  void onFastTerrainToggled(bool checked);
  void onShowTerrainToggled(bool checked);
  void onFlatShadeToggled(bool checked);
  void onNoLodToggled(bool checked);
  void onTerrain2dToggled(bool checked);
  void onShowInvisibleToggled(bool checked);
  void onNoExtRoomsObjsToggled(bool checked);

private:
  void updateDialog();
  void changeSelectedHeights(int delta, bool toAbsolute = false, int absoluteValue = 0);
  void moveSat(int pitch, int heading);
  int currentSat() const;

  template <typename T>
  T *findChild_(const QString &name) const { return findChild<T *>(name); }

  Ui::TerrainKeypad *ui;
  int m_currentSatellite = 0;
  int m_ccMode = 0; // 0=sky, 1=horizon, 2=fog, 3=satellite
};
