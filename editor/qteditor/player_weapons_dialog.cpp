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

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

#include "d3edit.h"
#include "objinfo.h"
#include "polymodel.h"
#include "robotfirestruct_external.h"
#include "ship.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "weapon.h"

namespace QtEditor {

namespace {
const char *const *weaponListText() { return Static_weapon_names; }
} // namespace

PlayerWeaponsDialog::PlayerWeaponsDialog(int current_ship, QWidget *parent)
    : Dialog(":/editor/player_weapons.ui", parent), m_current_ship(current_ship) {
  if (QPushButton *b = find<QPushButton>("IDC_EDIT_WB_BUTTON"))
    connect(b, &QPushButton::clicked, this, &PlayerWeaponsDialog::onEditWbButton);
  if (QComboBox *combo = find<QComboBox>("IDC_CURRENT_WEAPON_BATTERY_COMBO"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &PlayerWeaponsDialog::onCurrentWeaponChanged);
  if (QCheckBox *cb = find<QCheckBox>("IDC_FIRES_FUSION"))
    connect(cb, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onFiresFusion);
  if (QCheckBox *cb = find<QCheckBox>("IDC_ONOFF"))
    connect(cb, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onOnOff);
  if (QCheckBox *cb = find<QCheckBox>("IDC_ZOOM"))
    connect(cb, &QCheckBox::clicked, this, &PlayerWeaponsDialog::onZoom);
  if (QCheckBox *cb = find<QCheckBox>("IDC_CONTINUOUS_FIRING_SOUND"))
    connect(cb, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onContinuousFiringSound);
  if (QCheckBox *cb = find<QCheckBox>("IDC_FIRING_RELEASE_SOUND"))
    connect(cb, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onFiringReleaseSound);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SHOW_TENTHS"))
    connect(cb, &QCheckBox::toggled, this, &PlayerWeaponsDialog::onShowTenths);
  if (QComboBox *combo = find<QComboBox>("IDC_FIRING_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &PlayerWeaponsDialog::onFiringSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_RELEASE_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &PlayerWeaponsDialog::onReleaseSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_SPEW_POWERUP_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &PlayerWeaponsDialog::onSpewPowerupChanged);
  if (QLineEdit *edit = find<QLineEdit>("IDC_MAX_AMMO"))
    connect(edit, &QLineEdit::editingFinished, this, &PlayerWeaponsDialog::onMaxAmmoEdited);

  QComboBox *wb = find<QComboBox>("IDC_CURRENT_WEAPON_BATTERY_COMBO");
  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    if (strcmp("", weaponListText()[i]) == 0)
      break;
    wb->addItem(weaponListText()[i]);
  }
  if (wb->count() > 0)
    m_current_wb_text = wb->itemText(0);

  // Firing/release sound lists: "<none>" + used sounds.
  populateSoundCombo(find<QComboBox>("IDC_FIRING_SOUND_PULLDOWN"), 0);
  populateSoundCombo(find<QComboBox>("IDC_RELEASE_SOUND_PULLDOWN"), 0);

  QComboBox *spew = find<QComboBox>("IDC_SPEW_POWERUP_PULLDOWN");
  spew->addItem("<none>", -1);
  for (int i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == OBJ_POWERUP)
      spew->addItem(Object_info[i].name, i);
  }

  updateDialog();
}

PlayerWeaponsDialog::~PlayerWeaponsDialog() = default;

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

  if (QCheckBox *cb = find<QCheckBox>("IDC_FIRES_FUSION"))
    cb->setChecked(fire_flags & SFF_FUSION);
  if (QCheckBox *cb = find<QCheckBox>("IDC_ONOFF"))
    cb->setChecked(shp->static_wb[index].flags & WBF_ON_OFF);
  if (QCheckBox *cb = find<QCheckBox>("IDC_ZOOM"))
    cb->setChecked(fire_flags & SFF_ZOOM);
  if (QCheckBox *cb = find<QCheckBox>("IDC_SHOW_TENTHS"))
    cb->setChecked(fire_flags & SFF_TENTHS);

  const int firing_sound = Ships[m_current_ship].firing_sound[index];
  const int release_sound = Ships[m_current_ship].firing_release_sound[index];

  if (QCheckBox *cb = find<QCheckBox>("IDC_CONTINUOUS_FIRING_SOUND"))
    cb->setChecked(firing_sound != -1);
  if (QWidget *w = find<QWidget>("IDC_FIRING_SOUND_PULLDOWN"))
    w->setEnabled(firing_sound != -1);
  if (QCheckBox *cb = find<QCheckBox>("IDC_FIRING_RELEASE_SOUND"))
    cb->setChecked(release_sound != -1);
  if (QWidget *w = find<QWidget>("IDC_RELEASE_SOUND_PULLDOWN"))
    w->setEnabled(release_sound != -1);

  if (QComboBox *combo = find<QComboBox>("IDC_FIRING_SOUND_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    setSoundComboSelected(combo, firing_sound);
  }
  if (QComboBox *combo = find<QComboBox>("IDC_RELEASE_SOUND_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    setSoundComboSelected(combo, release_sound);
  }

  if (QComboBox *combo = find<QComboBox>("IDC_SPEW_POWERUP_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    const int spew = Ships[m_current_ship].spew_powerup[index];
    if (spew == -1)
      combo->setCurrentIndex(0);
    else
      combo->setCurrentIndex(combo->findData(spew));
  }

  if (QLineEdit *edit = find<QLineEdit>("IDC_MAX_AMMO"))
    edit->setText(QString::number(shp->max_ammo[index]));
}

void PlayerWeaponsDialog::onEditWbButton() {
  const int i = currentWBIndex();
  extern void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent);
  editRobotWeapons(&Ships[m_current_ship].static_wb[i], GetPolymodelPointer(Ships[m_current_ship].model_handle),
                   m_dialog);
}

void PlayerWeaponsDialog::onCurrentWeaponChanged() {
  if (QComboBox *combo = find<QComboBox>("IDC_CURRENT_WEAPON_BATTERY_COMBO")) {
    m_current_wb_text = combo->currentText();
    updateDialog();
  }
}

void PlayerWeaponsDialog::onFiresFusion() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~SFF_ZOOM;
  Ships[m_current_ship].static_wb[i].flags &= ~WBF_ON_OFF;
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
  const int s = soundComboSelected(find<QComboBox>("IDC_FIRING_SOUND_PULLDOWN"));
  if (s >= 0)
    Ships[m_current_ship].firing_sound[i] = s;
}

void PlayerWeaponsDialog::onReleaseSoundChanged() {
  const int i = currentWBIndex();
  const int s = soundComboSelected(find<QComboBox>("IDC_RELEASE_SOUND_PULLDOWN"));
  if (s >= 0)
    Ships[m_current_ship].firing_release_sound[i] = s;
}

void PlayerWeaponsDialog::onSpewPowerupChanged() {
  QComboBox *combo = find<QComboBox>("IDC_SPEW_POWERUP_PULLDOWN");
  const int i = currentWBIndex();
  Ships[m_current_ship].spew_powerup[i] = combo->currentData().toInt();
}

void PlayerWeaponsDialog::onOnOff() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~(SFF_FUSION | SFF_ZOOM);
  Ships[m_current_ship].static_wb[i].flags |= WBF_ON_OFF;
  updateDialog();
}

void PlayerWeaponsDialog::onZoom() {
  const int i = currentWBIndex();
  Ships[m_current_ship].fire_flags[i] &= ~SFF_FUSION;
  Ships[m_current_ship].static_wb[i].flags &= ~WBF_ON_OFF;
  Ships[m_current_ship].fire_flags[i] |= SFF_ZOOM;
  updateDialog();
}

void PlayerWeaponsDialog::onMaxAmmoEdited() {
  const int i = currentWBIndex();
  Ships[m_current_ship].max_ammo[i] = find<QLineEdit>("IDC_MAX_AMMO")->text().toInt();
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

}
