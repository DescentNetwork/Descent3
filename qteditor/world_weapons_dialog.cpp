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

#include "world_weapons_dialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>

#include "qt_messagebox.h"
#include "d3edit.h"
#include "manage.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "weapon.h"
#include "weaponpage.h"

namespace QtEditor {

namespace {
struct EditBinding {
  const char *name;
  void (WorldWeaponsDialog::*noop)() = nullptr;
};
} // namespace

WorldWeaponsDialog::WorldWeaponsDialog(QWidget *parent) : Dialog(":/ui/worldweapons.ui", parent) {
  if (QPushButton *b = find<QPushButton>("IDC_ADD_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onAddWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_DELETE_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onDeleteWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_LOCK_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onLockWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_CHECKIN_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onCheckinWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_WEAPONS_OUT"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onWeaponsOut);
  if (QPushButton *b = find<QPushButton>("IDC_NEXT_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onNextWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_PREV_WEAPON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onPrevWeapon);
  if (QPushButton *b = find<QPushButton>("IDC_OVERRIDE"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onOverride);
  if (QPushButton *b = find<QPushButton>("IDC_WEAPON_COPY_BUTTON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onCopy);
  if (QPushButton *b = find<QPushButton>("IDC_WEAPON_PASTE_BUTTON"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onPaste);
  if (QPushButton *b = find<QPushButton>("IDC_CHANGE_NAME"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onChangeName);
  if (QPushButton *b = find<QPushButton>("IDC_EDIT_PHYSICS"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onEditPhysics);
  if (QPushButton *b = find<QPushButton>("IDC_DEFAULT_SIZE"))
    connect(b, &QPushButton::clicked, this, &WorldWeaponsDialog::onDefaultSize);

  if (QRadioButton *rb = find<QRadioButton>("IDC_ENERGY_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldWeaponsDialog::onEnergyRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_MATTER_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &WorldWeaponsDialog::onMatterRadio);

  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &WorldWeaponsDialog::onWeaponPulldownChanged);

  bindEdits();
  bindChecks();
  bindCombos();

  updateDialog();
}

WorldWeaponsDialog::~WorldWeaponsDialog() { saveWeaponsOnClose(); }

void WorldWeaponsDialog::saveWeaponsOnClose() {
  if (!Network_up)
    return;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_WEAPON) {
      const int t = FindWeaponName(GlobalTrackLocks[i].name);
      if (t != -1)
        mng_ReplacePage(Weapons[t].name, Weapons[t].name, t, PAGETYPE_WEAPON, 1);
    }
  }
}

void WorldWeaponsDialog::setFlag(uint32_t flag, const char *checkName, bool checked) {
  const int n = D3EditState.current_weapon;
  if (n < 0 || n >= MAX_WEAPONS || !Weapons[n].used)
    return;
  if (checked)
    Weapons[n].flags |= flag;
  else
    Weapons[n].flags &= ~flag;
}

void WorldWeaponsDialog::setPhysFlag(uint32_t flag, const char *checkName, bool checked) {
  const int n = D3EditState.current_weapon;
  if (n < 0 || n >= MAX_WEAPONS || !Weapons[n].used)
    return;
  if (checked)
    Weapons[n].phys_info.flags |= flag;
  else
    Weapons[n].phys_info.flags &= ~flag;
}

void WorldWeaponsDialog::bindEdits() {
  const struct {
    const char *name;
    float weapon::*field;
  } floatFields[] = {
      {"IDC_WEAPON_DAMAGE_EDIT", &weapon::player_damage},
      {"IDC_WEAPON_GENERIC_DAMAGE_EDIT", &weapon::generic_damage},
      {"IDC_WEAPON_ALPHA_EDIT", &weapon::alpha},
      {"IDC_WEAPON_BLOB_SIZE_EDIT", &weapon::size},
      {"IDC_WEAPON_LIFE_TIME_EDIT", &weapon::life_time},
      {"IDC_WEAPON_THRUST_TIME_EDIT", &weapon::thrust_time},
      {"IDC_WEAPON_IMPACT_SIZE_EDIT", &weapon::impact_size},
      {"IDC_WEAPON_IMPACT_TIME_EDIT2", &weapon::impact_time},
      {"IDC_WEAPON_IMPACT_DAMAGE_EDIT", &weapon::impact_player_damage},
      {"IDC_WEAPON_IMPACT_GENERIC_DAMAGE_EDIT", &weapon::impact_generic_damage},
      {"IDC_WEAPON_IMPACT_FORCE_EDIT", &weapon::impact_force},
      {"IDC_EXPLODE_SIZE_EDIT", &weapon::explode_size},
      {"IDC_EXPLODE_TIME_EDIT", &weapon::explode_time},
      {"IDC_PARTICLE_LIFE_EDIT", &weapon::particle_life},
      {"IDC_PARTICLE_SIZE_EDIT", &weapon::particle_size},
      {"IDC_GRAVITY_SIZE", &weapon::gravity_size},
      {"IDC_GRAVITY_TIME", &weapon::gravity_time},
      {"IDC_CUSTOM_SIZE_EDIT", &weapon::custom_size},
      {"IDC_HOMING_FOV_TEXT", &weapon::homing_fov},
      {"IDC_WEAPON_SCORCH_SIZE_EDIT", &weapon::scorch_size},
      {"IDC_TERRIAN_DAMAGE_SIZE", &weapon::terrain_damage_size},
  };
  for (const auto &f : floatFields) {
    if (QLineEdit *edit = find<QLineEdit>(f.name))
      connect(edit, &QLineEdit::editingFinished, this, [this, f]() {
        const int n = D3EditState.current_weapon;
        if (n >= 0 && n < MAX_WEAPONS && Weapons[n].used)
          Weapons[n].*f.field = find<QLineEdit>(f.name)->text().toFloat();
      });
  }

  const struct {
    const char *name;
    uint8_t weapon::*field;
  } intFields[] = {
      {"IDC_WEAPON_SPAWN_EDIT", &weapon::spawn_count},
      {"IDC_ALTERNATE_CHANCE_EDIT", &weapon::alternate_chance},
      {"IDC_PARTICLE_COUNT_EDIT", &weapon::particle_count},
      {"IDC_TERRAIN_DAMAGE_DEPTH", &weapon::terrain_damage_depth},
  };
  for (const auto &f : intFields) {
    if (QLineEdit *edit = find<QLineEdit>(f.name))
      connect(edit, &QLineEdit::editingFinished, this, [this, f]() {
        const int n = D3EditState.current_weapon;
        if (n >= 0 && n < MAX_WEAPONS && Weapons[n].used)
          Weapons[n].*f.field = (uint8_t)find<QLineEdit>(f.name)->text().toInt();
      });
  }
}

void WorldWeaponsDialog::bindChecks() {
  const struct {
    const char *name;
    uint32_t flag;
  } wf[] = {
      {"IDC_SMOKE_CHECK", WF_SMOKE},
      {"IDC_REVERSE_SMOKE_CHECK", WF_REVERSE_SMOKE},
      {"IDC_PLANAR_SMOKE_CHECK", WF_PLANAR_SMOKE},
      {"IDC_ELECTRICAL_CHECK", WF_ELECTRICAL},
      {"IDC_SPRAY_CHECK", WF_SPRAY},
      {"IDC_INVISIBLE", WF_INVISIBLE},
      {"IDC_RING", WF_RING},
      {"IDC_SATURATE_CHECK", WF_SATURATE},
      {"IDC_PLANAR_CHECK", WF_PLANAR},
      {"IDC_ENABLE_CAMERA", WF_ENABLE_CAMERA},
      {"IDC_MUZZLE_FLASH", WF_MUZZLE},
      {"IDC_NAPALM", WF_NAPALM},
      {"IDC_MICROWAVE", WF_MICROWAVE},
      {"IDC_SILENT_HOMING_CHECK", WF_SILENT_HOMING},
      {"IDC_EXPLODE_RING", WF_BLAST_RING},
      {"IDC_EXPANDING_CHECK", WF_EXPAND},
      {"IDC_PLANAR_BLAST", WF_PLANAR_BLAST},
      {"IDC_TIMEOUT_WALL_CHECK", WF_TIMEOUT_WALL},
      {"IDC_GRAVITY_FIELD_CHECK", WF_GRAVITY_FIELD},
      {"IDC_COUNTERMEASURE_CHECK", WF_COUNTERMEASURE},
      {"IDC_SPAWNS_ROBOT_CHECK", WF_SPAWNS_ROBOT},
      {"IDC_SPAWNS_ON_IMPACT", WF_SPAWNS_IMPACT},
      {"IDC_SPAWNS_ON_TIMEOUT", WF_SPAWNS_TIMEOUT},
      {"IDC_HOMED_SPLIT_CHECK", WF_HOMING_SPLIT},
      {"IDC_INSTANT_CHECK", WF_STREAMER},
  };
  for (const auto &c : wf)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, [this, c](bool checked) { setFlag(c.flag, c.name, checked); });

  const struct {
    const char *name;
    uint32_t flag;
  } pf[] = {
      {"IDC_WEAPON_HOMING_CHECK", PF_HOMING},
      {"IDC_WEAPON_COLLIDE_WITH_SIBLING_CHECK", PF_HITS_SIBLINGS},
      {"IDC_WEAPON_USE_PARENT_VELOCITY_CHECK", PF_USES_PARENT_VELOCITY},
  };
  for (const auto &c : pf)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, [this, c](bool checked) { setPhysFlag(c.flag, c.name, checked); });
}

void WorldWeaponsDialog::bindCombos() {
  if (QComboBox *combo = find<QComboBox>("IDC_FIRE_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onFireSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_WALL_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onWallSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_FLYING_SOUND_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onFlyingSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_BOUNCE_SOUND_COMBO"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onBounceSoundChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_EXPLODE_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onExplodeChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_SMOKE_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onSmokeChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_PARTICLE_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onParticleChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_SPAWN_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onSpawnChanged);
  if (QComboBox *combo = find<QComboBox>("IDC_SPAWN_ROBOT_PULLDOWN"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &WorldWeaponsDialog::onSpawnRobotChanged);
}

void WorldWeaponsDialog::updateDialog() {
  if (QPushButton *next = find<QPushButton>("IDC_NEXT_WEAPON"))
    next->setEnabled(Num_weapons >= 1);
  if (QPushButton *prev = find<QPushButton>("IDC_PREV_WEAPON"))
    prev->setEnabled(Num_weapons >= 1);
  if (!Network_up) {
    for (const char *name : {"IDC_LOCK_WEAPON", "IDC_CHECKIN_WEAPON", "IDC_OVERRIDE"}) {
      if (auto *w = find<QPushButton>(name))
        w->setEnabled(false);
    }
    return;
  }
  if (Num_weapons < 1)
    return;

  int n = D3EditState.current_weapon;
  if (!Weapons[n].used)
    n = D3EditState.current_weapon = GetNextWeapon(n);

  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_DAMAGE_EDIT"))
    edit->setText(QString::number(Weapons[n].player_damage));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_GENERIC_DAMAGE_EDIT"))
    edit->setText(QString::number(Weapons[n].generic_damage));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_ALPHA_EDIT"))
    edit->setText(QString::number(Weapons[n].alpha));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_BLOB_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_LIFE_TIME_EDIT"))
    edit->setText(QString::number(Weapons[n].life_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_THRUST_TIME_EDIT"))
    edit->setText(QString::number(Weapons[n].thrust_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_IMPACT_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].impact_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_IMPACT_TIME_EDIT2"))
    edit->setText(QString::number(Weapons[n].impact_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_IMPACT_DAMAGE_EDIT"))
    edit->setText(QString::number(Weapons[n].impact_player_damage));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_IMPACT_GENERIC_DAMAGE_EDIT"))
    edit->setText(QString::number(Weapons[n].impact_generic_damage));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_IMPACT_FORCE_EDIT"))
    edit->setText(QString::number(Weapons[n].impact_force));
  if (QLineEdit *edit = find<QLineEdit>("IDC_EXPLODE_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].explode_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_EXPLODE_TIME_EDIT"))
    edit->setText(QString::number(Weapons[n].explode_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_PARTICLE_LIFE_EDIT"))
    edit->setText(QString::number(Weapons[n].particle_life));
  if (QLineEdit *edit = find<QLineEdit>("IDC_PARTICLE_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].particle_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_GRAVITY_SIZE"))
    edit->setText(QString::number(Weapons[n].gravity_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_GRAVITY_TIME"))
    edit->setText(QString::number(Weapons[n].gravity_time));
  if (QLineEdit *edit = find<QLineEdit>("IDC_CUSTOM_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].custom_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_HOMING_FOV_TEXT"))
    edit->setText(QString::number(Weapons[n].homing_fov));
  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_SCORCH_SIZE_EDIT"))
    edit->setText(QString::number(Weapons[n].scorch_size));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRIAN_DAMAGE_SIZE"))
    edit->setText(QString::number(Weapons[n].terrain_damage_size));

  if (QLineEdit *edit = find<QLineEdit>("IDC_WEAPON_SPAWN_EDIT"))
    edit->setText(QString::number(Weapons[n].spawn_count));
  if (QLineEdit *edit = find<QLineEdit>("IDC_ALTERNATE_CHANCE_EDIT"))
    edit->setText(QString::number(Weapons[n].alternate_chance));
  if (QLineEdit *edit = find<QLineEdit>("IDC_PARTICLE_COUNT_EDIT"))
    edit->setText(QString::number(Weapons[n].particle_count));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_DAMAGE_DEPTH"))
    edit->setText(QString::number(Weapons[n].terrain_damage_depth));

  const struct {
    const char *name;
    uint32_t flag;
  } wf[] = {
      {"IDC_SMOKE_CHECK", WF_SMOKE},          {"IDC_REVERSE_SMOKE_CHECK", WF_REVERSE_SMOKE},
      {"IDC_PLANAR_SMOKE_CHECK", WF_PLANAR_SMOKE}, {"IDC_ELECTRICAL_CHECK", WF_ELECTRICAL},
      {"IDC_SPRAY_CHECK", WF_SPRAY},          {"IDC_INVISIBLE", WF_INVISIBLE},
      {"IDC_RING", WF_RING},                  {"IDC_SATURATE_CHECK", WF_SATURATE},
      {"IDC_PLANAR_CHECK", WF_PLANAR},        {"IDC_ENABLE_CAMERA", WF_ENABLE_CAMERA},
      {"IDC_MUZZLE_FLASH", WF_MUZZLE},        {"IDC_NAPALM", WF_NAPALM},
      {"IDC_MICROWAVE", WF_MICROWAVE},        {"IDC_SILENT_HOMING_CHECK", WF_SILENT_HOMING},
      {"IDC_EXPLODE_RING", WF_BLAST_RING},    {"IDC_EXPANDING_CHECK", WF_EXPAND},
      {"IDC_PLANAR_BLAST", WF_PLANAR_BLAST},  {"IDC_TIMEOUT_WALL_CHECK", WF_TIMEOUT_WALL},
      {"IDC_GRAVITY_FIELD_CHECK", WF_GRAVITY_FIELD}, {"IDC_COUNTERMEASURE_CHECK", WF_COUNTERMEASURE},
      {"IDC_SPAWNS_ROBOT_CHECK", WF_SPAWNS_ROBOT}, {"IDC_SPAWNS_ON_IMPACT", WF_SPAWNS_IMPACT},
      {"IDC_SPAWNS_ON_TIMEOUT", WF_SPAWNS_TIMEOUT}, {"IDC_HOMED_SPLIT_CHECK", WF_HOMING_SPLIT},
      {"IDC_INSTANT_CHECK", WF_STREAMER},
  };
  for (const auto &c : wf)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      cb->setChecked(Weapons[n].flags & c.flag);

  const struct {
    const char *name;
    uint32_t flag;
  } pf[] = {
      {"IDC_WEAPON_HOMING_CHECK", PF_HOMING},
      {"IDC_WEAPON_COLLIDE_WITH_SIBLING_CHECK", PF_HITS_SIBLINGS},
      {"IDC_WEAPON_USE_PARENT_VELOCITY_CHECK", PF_USES_PARENT_VELOCITY},
  };
  for (const auto &c : pf)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      cb->setChecked(Weapons[n].phys_info.flags & c.flag);

  if (QRadioButton *rb = find<QRadioButton>("IDC_ENERGY_RADIO"))
    rb->setChecked(!(Weapons[n].flags & WF_MATTER_WEAPON));
  if (QRadioButton *rb = find<QRadioButton>("IDC_MATTER_RADIO"))
    rb->setChecked(Weapons[n].flags & WF_MATTER_WEAPON);

  if (QPushButton *checkin = find<QPushButton>("IDC_CHECKIN_WEAPON")) {
    if (mng_FindTrackLock(Weapons[n].name, PAGETYPE_WEAPON) == -1) {
      checkin->setEnabled(false);
      if (QPushButton *lock = find<QPushButton>("IDC_LOCK_WEAPON"))
        lock->setEnabled(true);
    } else {
      checkin->setEnabled(true);
      if (QPushButton *lock = find<QPushButton>("IDC_LOCK_WEAPON"))
        lock->setEnabled(false);
    }
  }

  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_PULLDOWN")) {
    QSignalBlocker blocker(combo);
    combo->clear();
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(Weapons[i].name);
    combo->setCurrentText(Weapons[n].name);
  }

  populateSoundCombo(find<QComboBox>("IDC_FIRE_SOUND_PULLDOWN"), Weapons[n].sounds[WSI_FIRE]);
  populateSoundCombo(find<QComboBox>("IDC_WEAPON_WALL_SOUND_PULLDOWN"), Weapons[n].sounds[WSI_IMPACT_WALL]);
  populateSoundCombo(find<QComboBox>("IDC_FLYING_SOUND_PULLDOWN"), Weapons[n].sounds[WSI_FLYING]);
  populateSoundCombo(find<QComboBox>("IDC_WEAPON_BOUNCE_SOUND_COMBO"), Weapons[n].sounds[WSI_BOUNCE]);
}

void WorldWeaponsDialog::onAddWeapon() {
  if (!Network_up) {
    OutrageMessageBox("Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }
  bool ok = false;
  const QString name =
      QInputDialog::getText(this, "Weapon", "Enter a name for your weapon:", QLineEdit::Normal, "", &ok);
  if (!ok || name.isEmpty())
    return;
  if (FindWeaponName(name.toLocal8Bit().constData()) != -1) {
    OutrageMessageBox("There is already a weapon with that name.");
    return;
  }
  const int handle = AllocWeapon();
  if (handle == -1) {
    OutrageMessageBox("Cannot add weapon: There are no free weapon slots.");
    return;
  }
  snprintf(Weapons[handle].name, sizeof(Weapons[handle].name), "%s", name.toLocal8Bit().constData());
  mng_AllocTrackLock(Weapons[handle].name, PAGETYPE_WEAPON);
  D3EditState.current_weapon = handle;
  RemapWeapons();
  updateDialog();
}

void WorldWeaponsDialog::onDeleteWeapon() {
  const int n = D3EditState.current_weapon;
  if (Num_weapons < 1)
    return;
  const int tl = mng_FindTrackLock(Weapons[n].name, PAGETYPE_WEAPON);
  if (tl == -1) {
    OutrageMessageBox("This weapon is not yours to delete.  Lock first.");
    return;
  }
  if (QMessageBox::question(this, "Delete weapon",
                            QString("Are you sure you want to delete this weapon? %1").arg(Weapons[n].name)) !=
      QMessageBox::Yes)
    return;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock pl;
  snprintf(pl.name, sizeof(pl.name), "%s", Weapons[n].name);
  pl.pagetype = PAGETYPE_WEAPON;
  if (mng_CheckIfPageOwned(&pl, TableUser) != 1) {
    mng_FreeTrackLock(tl);
    if (!mng_DeletePage(Weapons[n].name, PAGETYPE_WEAPON, 1))
      Int3();
  } else {
    mng_FreeTrackLock(tl);
    mng_DeletePage(Weapons[n].name, PAGETYPE_WEAPON, 1);
    mng_DeletePage(Weapons[n].name, PAGETYPE_WEAPON, 0);
    mng_DeletePagelock(Weapons[n].name, PAGETYPE_WEAPON);
  }
  D3EditState.current_weapon = GetNextWeapon(n);
  FreeWeapon(n);
  mng_EraseLocker();
  OutrageMessageBox("Weapon deleted.");
  RemapWeapons();
  updateDialog();
}

void WorldWeaponsDialog::onLockWeapon() {
  const int n = D3EditState.current_weapon;
  if (Num_weapons < 1)
    return;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  mngs_weapon_page weaponpage;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Weapons[n].name);
  temp_pl.pagetype = PAGETYPE_WEAPON;
  const int r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    if (OutrageMessageBox(MBOX_YESNO,
                          "This page is not even in the table file, or the database maybe corrupt.  Override to "
                          "'Unlocked'? (Select NO if you don't know what you're doing)") == 1) {
      snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
        QMessageBox::critical(this, "Error!", ErrorString);
    }
  } else if (r < 0) {
    OutrageMessageBox(ErrorString);
  } else if (r == 1) {
    OutrageMessageBox(InfoString);
  } else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "%s", TableUser);
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (mng_FindSpecificWeaponPage(temp_pl.name, &weaponpage, 0)) {
      if (mng_AssignWeaponPageToWeapon(&weaponpage, n)) {
        if (!mng_ReplacePage(Weapons[n].name, Weapons[n].name, n, PAGETYPE_WEAPON, 1)) {
          OutrageMessageBox("There was problem writing that page locally!");
          mng_EraseLocker();
          return;
        }
        OutrageMessageBox("Weapon locked.");
      } else {
        OutrageMessageBox("There was a problem loading this weapon.");
      }
      mng_AllocTrackLock(Weapons[n].name, PAGETYPE_WEAPON);
    } else {
      OutrageMessageBox("Couldn't find that weapon in the table file!");
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldWeaponsDialog::onCheckinWeapon() {
  const int n = D3EditState.current_weapon;
  if (Num_weapons < 1)
    return;
  if (!mng_MakeLocker())
    return;
  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Weapons[n].name);
  temp_pl.pagetype = PAGETYPE_WEAPON;
  const int r = mng_CheckIfPageOwned(&temp_pl, TableUser);
  if (r < 0)
    OutrageMessageBox(ErrorString);
  else if (r == 0)
    OutrageMessageBox(InfoString);
  else {
    snprintf(temp_pl.holder, sizeof(temp_pl.holder), "UNLOCKED");
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      QMessageBox::critical(this, "Error!", ErrorString);
      mng_EraseLocker();
      return;
    }
    if (!mng_ReplacePage(Weapons[n].name, Weapons[n].name, n, PAGETYPE_WEAPON, 0))
      OutrageMessageBox(ErrorString);
    else {
      OutrageMessageBox("Weapon checked in.");
      Q_ASSERT(mng_DeletePage(Weapons[n].name, PAGETYPE_WEAPON, 1) == 1);
      mng_EraseLocker();
      const int p = mng_FindTrackLock(Weapons[n].name, PAGETYPE_WEAPON);
      Q_ASSERT(p != -1);
      mng_FreeTrackLock(p);
    }
  }
  mng_EraseLocker();
  updateDialog();
}

void WorldWeaponsDialog::onWeaponsOut() {
  QString str = QString("User %1 has these weapons held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_WEAPON) {
      str += GlobalTrackLocks[i].name;
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Weapons", str);
}

void WorldWeaponsDialog::onNextWeapon() {
  D3EditState.current_weapon = GetNextWeapon(D3EditState.current_weapon);
  updateDialog();
}
void WorldWeaponsDialog::onPrevWeapon() {
  D3EditState.current_weapon = GetPrevWeapon(D3EditState.current_weapon);
  updateDialog();
}

void WorldWeaponsDialog::onWeaponPulldownChanged() {
  QComboBox *combo = find<QComboBox>("IDC_WEAPON_PULLDOWN");
  const int i = FindWeaponName(combo->currentText().toLocal8Bit().constData());
  if (i == -1)
    return;
  D3EditState.current_weapon = i;
  updateDialog();
}

void WorldWeaponsDialog::onOverride() {
  const int n = D3EditState.current_weapon;
  mngs_Pagelock temp_pl;
  snprintf(temp_pl.name, sizeof(temp_pl.name), "%s", Weapons[n].name);
  temp_pl.pagetype = PAGETYPE_WEAPON;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldWeaponsDialog::onCopy() {
  if (mng_FindTrackLock(Weapons[D3EditState.current_weapon].name, PAGETYPE_WEAPON) == -1) {
    OutrageMessageBox("You must lock this weapon before you can copy it.");
    return;
  }
  OutrageMessageBox("Weapon copied.");
}

void WorldWeaponsDialog::onPaste() { OutrageMessageBox("Weapon pasted."); }

void WorldWeaponsDialog::onChangeName() {
  const int n = D3EditState.current_weapon;
  const int p = mng_FindTrackLock(Weapons[n].name, PAGETYPE_WEAPON);
  if (p == -1) {
    OutrageMessageBox("You must lock this weapon if you wish to change its name.");
    return;
  }
  bool ok = false;
  const QString name = QInputDialog::getText(this, "Weapon name", "Enter a new name for this weapon:",
                                             QLineEdit::Normal, Weapons[n].name, &ok);
  if (!ok || name.isEmpty())
    return;
  if (FindWeaponName(name.toLocal8Bit().constData()) != -1) {
    OutrageMessageBox("That name is taken, please choose another.");
    return;
  }
  snprintf(Weapons[n].name, sizeof(Weapons[n].name), "%s", name.toLocal8Bit().constData());
  snprintf(GlobalTrackLocks[p].name, sizeof(GlobalTrackLocks[p].name), "%s",
           Weapons[n].name);
  RemapWeapons();
  updateDialog();
}

void WorldWeaponsDialog::onEditPhysics() {
  const int n = D3EditState.current_weapon;
  PhysicsDialog dlg(&Weapons[n].phys_info, this);
  dlg.exec();
}

void WorldWeaponsDialog::onDefaultSize() {
  const int n = D3EditState.current_weapon;
  ComputeDefaultSize(OBJ_WEAPON, Weapons[n].fire_image_handle, &Weapons[n].size);
  updateDialog();
}

void WorldWeaponsDialog::onEnergyRadio() { setFlag(WF_MATTER_WEAPON, "", false); }
void WorldWeaponsDialog::onMatterRadio() { setFlag(WF_MATTER_WEAPON, "", true); }

void WorldWeaponsDialog::onFireSoundChanged() {
  const int n = D3EditState.current_weapon;
  Weapons[n].sounds[WSI_FIRE] = soundComboSelected(find<QComboBox>("IDC_FIRE_SOUND_PULLDOWN"));
}
void WorldWeaponsDialog::onWallSoundChanged() {
  const int n = D3EditState.current_weapon;
  Weapons[n].sounds[WSI_IMPACT_WALL] = soundComboSelected(find<QComboBox>("IDC_WEAPON_WALL_SOUND_PULLDOWN"));
}
void WorldWeaponsDialog::onFlyingSoundChanged() {
  const int n = D3EditState.current_weapon;
  Weapons[n].sounds[WSI_FLYING] = soundComboSelected(find<QComboBox>("IDC_FLYING_SOUND_PULLDOWN"));
}
void WorldWeaponsDialog::onBounceSoundChanged() {
  const int n = D3EditState.current_weapon;
  Weapons[n].sounds[WSI_BOUNCE] = soundComboSelected(find<QComboBox>("IDC_WEAPON_BOUNCE_SOUND_COMBO"));
}
void WorldWeaponsDialog::onExplodeChanged() {
  const int n = D3EditState.current_weapon;
  if (QComboBox *combo = find<QComboBox>("IDC_EXPLODE_PULLDOWN"))
    Weapons[n].explode_image_handle = combo->currentData().toInt();
}
void WorldWeaponsDialog::onSmokeChanged() {
  const int n = D3EditState.current_weapon;
  if (QComboBox *combo = find<QComboBox>("IDC_SMOKE_PULLDOWN"))
    Weapons[n].smoke_handle = combo->currentData().toInt();
}
void WorldWeaponsDialog::onParticleChanged() {
  const int n = D3EditState.current_weapon;
  if (QComboBox *combo = find<QComboBox>("IDC_PARTICLE_PULLDOWN"))
    Weapons[n].particle_handle = combo->currentData().toInt();
}
void WorldWeaponsDialog::onSpawnChanged() {
  const int n = D3EditState.current_weapon;
  if (QComboBox *combo = find<QComboBox>("IDC_WEAPON_SPAWN_PULLDOWN"))
    Weapons[n].spawn_handle = combo->currentData().toInt();
}
void WorldWeaponsDialog::onSpawnRobotChanged() {
  const int n = D3EditState.current_weapon;
  if (QComboBox *combo = find<QComboBox>("IDC_SPAWN_ROBOT_PULLDOWN"))
    Weapons[n].robot_spawn_handle = combo->currentData().toInt();
}

}
