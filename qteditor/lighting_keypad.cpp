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

#include "lighting_keypad.h"
#include "ui_lightingkeypad.h"

#include <QCheckBox>

#include "d3edit.h"
#include "lighting.h"

// Editor-side lighting globals. Outline_lightmaps is a bool in Descent3Core
// (render.cpp); the others are provided in d3_editor_state.cpp.
extern bool Outline_lightmaps;
extern int BestFit;
extern int Shoot_from_patch;


LightingKeypad::LightingKeypad(QWidget *parent)
    : QDialog(parent), ui(new Ui::LightingKeypad)
{
  ui->setupUi(this);
  if (QCheckBox *cb = ui->IDC_SHOW_LIGHTMAPS)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onShowLightmaps);
  if (QCheckBox *cb = ui->IDC_BESTFIT_CHECK)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onBestFit);
  if (QCheckBox *cb = ui->IDC_HEMICUBE_CHECK)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onHemicube);
  if (QCheckBox *cb = ui->IDC_ELEMENT_CHECK)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onElement);
  if (QCheckBox *cb = ui->IDC_IGNORE_TERRAIN)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onIgnoreTerrain);
  if (QCheckBox *cb = ui->IDC_IGNORE_SATELLITES)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onIgnoreSatellites);
  if (QCheckBox *cb = ui->IDC_USE_BSP_CHECK)
    connect(cb, &QCheckBox::toggled, this, &LightingKeypad::onUseBsp);

  updateDialog();
}

LightingKeypad::~LightingKeypad() { delete ui; }

void LightingKeypad::updateDialog() {
  if (QCheckBox *cb = ui->IDC_SHOW_LIGHTMAPS)
    cb->setChecked(Outline_lightmaps != 0);
  if (QCheckBox *cb = ui->IDC_BESTFIT_CHECK)
    cb->setChecked(BestFit != 0);
  if (QCheckBox *cb = ui->IDC_HEMICUBE_CHECK)
    cb->setChecked(D3EditState.hemicube_radiosity != 0);
  if (QCheckBox *cb = ui->IDC_ELEMENT_CHECK)
    cb->setChecked(Shoot_from_patch == 0);
}

void LightingKeypad::onShowLightmaps(bool checked) {
  Outline_lightmaps = checked ? 1 : 0;
  State_changed = true;
}
void LightingKeypad::onBestFit(bool checked) {
  BestFit = checked ? 1 : 0;
  State_changed = true;
}
void LightingKeypad::onHemicube(bool checked) {
  D3EditState.hemicube_radiosity = checked ? 1 : 0;
  State_changed = true;
}
void LightingKeypad::onElement(bool checked) { Shoot_from_patch = checked ? 0 : 1; }
void LightingKeypad::onIgnoreTerrain(bool) {}
void LightingKeypad::onIgnoreSatellites(bool) {}
void LightingKeypad::onUseBsp(bool) {}

