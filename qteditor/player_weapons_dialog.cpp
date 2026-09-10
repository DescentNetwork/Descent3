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

#include "player_weapons_dialog.h"
#include "ui_player_weapons.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>


#include "objinfo.h"
#include "polymodel.h"
#include "robotfirestruct_external.h"
#include "ship.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "weapon.h"


namespace {
const char *const *weaponListText() { return Static_weapon_names; }
} // namespace

PlayerWeaponsDialog::PlayerWeaponsDialog(int current_ship, QWidget *parent)
    : QDialog(parent), ui(new Ui::PlayerWeaponsDialog), m_current_ship(current_ship)
{
  ui->setupUi(this);
  connect(ui->IDC_EDIT_WB_BUTTON, &QPushButton::clicked, this, &PlayerWeaponsDialog::onEditWbButton);
      connect(ui->IDC_CURRENT_WEAPON_BATTERY_COMBO, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &PlayerWeaponsDialog::onCurrentWeaponChanged);
  connect(ui->IDC_FIRES_FUSION, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onFiresFusion);
  connect(ui->IDC_ONOFF, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onOnOff);
  connect(ui->IDC_ZOOM, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onZoom);
  connect(ui->IDC_CONTINUOUS_FIRING_SOUND, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onContinuousFiringSound);
  connect(ui->IDC_FIRING_RELEASE_SOUND, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onFiringReleaseSound);
  connect(ui->IDC_SHOW_TENTHS, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onShowTenths);
      connect(ui->IDC_FIRING_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &PlayerWeaponsDialog::onFiringSoundChanged);
      connect(ui->IDC_RELEASE_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &PlayerWeaponsDialog::onReleaseSoundChanged);
      connect(ui->IDC_SPEW_POWERUP_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &PlayerWeaponsDialog::onSpewPowerupChanged);
  connect(ui->IDC_MAX_AMMO, &QLineEdit::editingFinished, this, &PlayerWeaponsDialog::onMaxAmmoEdited);

  QComboBox *wb = ui->IDC_CURRENT_WEAPON_BATTERY_COMBO;
  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    if (strcmp("", weaponListText()[i]) == 0)
      break;
    wb->addItem(weaponListText()[i]);
  }
  if (wb->count() > 0)
    m_current_wb_text = wb->itemText(0);

  // Firing/release sound lists: "<none>" + used sounds.
  populateSoundCombo(ui->IDC_FIRING_SOUND_PULLDOWN, 0);
  populateSoundCombo(ui->IDC_RELEASE_SOUND_PULLDOWN, 0);

  QComboBox *spew = ui->IDC_SPEW_POWERUP_PULLDOWN;
  spew->addItem("<none>", -1);
  for (int i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == OBJ_POWERUP)
      spew->addItem(QString::fromStdString(Object_info[i].name), i);
  }

  updateDialog();
}

PlayerWeaponsDialog::~PlayerWeaponsDialog() { delete ui; }

int PlayerWeaponsDialog::currentWBIndex() const {
  int i;
  for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    if (strcmp(weaponListText()[i], m_current_wb_text.toLocal8Bit().constData()) == 0)
      break;
  }
  if (i >= MAX_PLAYER_WEAPONS)
    i = 0;
  return i;
}

void PlayerWeaponsDialog::updateDialog() {
  const int index = currentWBIndex();
  ship *shp = &Ships[m_current_ship];
  const int fire_flags = Ships[m_current_ship].fire_flags[index];

  ui->IDC_FIRES_FUSION->setChecked(fire_flags & SFF_FUSION);
  ui->IDC_ONOFF->setChecked(shp->static_wb[index].flags.on_off);
  ui->IDC_ZOOM->setChecked(fire_flags & SFF_ZOOM);
  ui->IDC_SHOW_TENTHS->setChecked(fire_flags & SFF_TENTHS);

  const int firing_sound = Ships[m_current_ship].firing_sound[index];
  const int release_sound = Ships[m_current_ship].firing_release_sound[index];

  ui->IDC_CONTINUOUS_FIRING_SOUND->setChecked(firing_sound != -1);
  ui->IDC_FIRING_SOUND_PULLDOWN->setEnabled(firing_sound != -1);
  ui->IDC_FIRING_RELEASE_SOUND->setChecked(release_sound != -1);
  ui->IDC_RELEASE_SOUND_PULLDOWN->setEnabled(release_sound != -1);

  {
    QComboBox *combo = ui->IDC_FIRING_SOUND_PULLDOWN;
    QSignalBlocker blocker(combo);
    setSoundComboSelected(combo, firing_sound);
  }
  {
    QComboBox *combo = ui->IDC_RELEASE_SOUND_PULLDOWN;
    QSignalBlocker blocker(combo);
    setSoundComboSelected(combo, release_sound);
  }

  {
    QComboBox *combo = ui->IDC_SPEW_POWERUP_PULLDOWN;
    QSignalBlocker blocker(combo);
    const int spew = Ships[m_current_ship].spew_powerup[index];
    if (spew == -1)
      combo->setCurrentIndex(0);
    else
      combo->setCurrentIndex(combo->findData(spew));
  }

  ui->IDC_MAX_AMMO->setText(QString::number(shp->max_ammo[index]));
}

void PlayerWeaponsDialog::onEditWbButton() {
  const int i = currentWBIndex();
  extern void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent);
  editRobotWeapons(&Ships[m_current_ship].static_wb[i], GetPolymodelPointer(Ships[m_current_ship].model_handle),
                   this);
}

void PlayerWeaponsDialog::onCurrentWeaponChanged() {
  {
    QComboBox *combo = ui->IDC_CURRENT_WEAPON_BATTERY_COMBO;
    m_current_wb_text = combo->currentText();
    updateDialog();
  }
}

void PlayerWeaponsDialog::onFiresFusion() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~SFF_ZOOM;
  Ships[m_current_ship].static_wb[i].flags.on_off = false;
  Ships[m_current_ship].fire_flags[i] |= SFF_FUSION;
  updateDialog();
}

void PlayerWeaponsDialog::onContinuousFiringSound(bool checked) {
  const int i = currentWBIndex();
  Ships[m_current_ship].firing_sound[i] = checked ? 0 : -1;
  updateDialog();
}

void PlayerWeaponsDialog::onFiringReleaseSound(bool checked) {
  const int i = currentWBIndex();
  Ships[m_current_ship].firing_release_sound[i] = checked ? 0 : -1;
  updateDialog();
}

void PlayerWeaponsDialog::onFiringSoundChanged() {
  const int i = currentWBIndex();
  const int s = soundComboSelected(ui->IDC_FIRING_SOUND_PULLDOWN);
  if (s >= 0)
    Ships[m_current_ship].firing_sound[i] = s;
}

void PlayerWeaponsDialog::onReleaseSoundChanged() {
  const int i = currentWBIndex();
  const int s = soundComboSelected(ui->IDC_RELEASE_SOUND_PULLDOWN);
  if (s >= 0)
    Ships[m_current_ship].firing_release_sound[i] = s;
}

void PlayerWeaponsDialog::onSpewPowerupChanged() {
  QComboBox *combo = ui->IDC_SPEW_POWERUP_PULLDOWN;
  const int i = currentWBIndex();
  Ships[m_current_ship].spew_powerup[i] = combo->currentData().toInt();
}

void PlayerWeaponsDialog::onOnOff() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~(SFF_FUSION | SFF_ZOOM);
  Ships[m_current_ship].static_wb[i].flags.on_off = true;
  updateDialog();
}

void PlayerWeaponsDialog::onZoom() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~SFF_FUSION;
  Ships[m_current_ship].static_wb[i].flags.on_off = false;
  Ships[m_current_ship].fire_flags[i] |= SFF_ZOOM;
  updateDialog();
}

void PlayerWeaponsDialog::onMaxAmmoEdited() {
  const int i = currentWBIndex();
  Ships[m_current_ship].max_ammo[i] = ui->IDC_MAX_AMMO->text().toInt();
}

void PlayerWeaponsDialog::onShowTenths(bool checked) {
  const int i = currentWBIndex();
  if (checked)
    Ships[m_current_ship].fire_flags[i] |= SFF_TENTHS;
  else
    Ships[m_current_ship].fire_flags[i] &= ~SFF_TENTHS;
  updateDialog();
}

void editPlayerWeapons(int shipHandle, QWidget *parent) {
  PlayerWeaponsDialog dlg(shipHandle, parent);
  dlg.exec();
}

