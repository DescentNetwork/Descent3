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

#include "keypad_dialog.h"

namespace QtEditor {

// Port of CLightingDialog (IDD_LIGHTINGKEYPAD): lightmap/radiosity settings
// for the lighting pass.
class LightingKeypad : public Keypad {
  Q_OBJECT
public:
  explicit LightingKeypad(QWidget *parent = nullptr);
  ~LightingKeypad() override;

private slots:
  void onShowLightmaps(bool checked);
  void onBestFit(bool checked);
  void onHemicube(bool checked);
  void onElement(bool checked);
  void onIgnoreTerrain(bool checked);
  void onIgnoreSatellites(bool checked);
  void onUseBsp(bool checked);

private:
  void updateDialog();
};

}
