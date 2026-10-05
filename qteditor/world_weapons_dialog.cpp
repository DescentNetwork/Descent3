#include <QtGlobal>
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
#include "ui_worldweapons.h"

#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <cstring>



#include "manage.h"
#include "physics_dialog.h"
#include "polymodel.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "weapon.h"
#include "weaponpage.h"
#include "d3edit.h"

optref<weapon> WorldWeaponsDialog::data(void)
{
  if (!app.current_weapon || *app.current_weapon >= static_cast<uint32_t>(Weapons.size())) return std::nullopt;
  return Weapons[*app.current_weapon];
}

WorldWeaponsDialog::WorldWeaponsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WorldWeaponsDialog) {
  ui->setupUi(this);
  connect(ui->IDC_ADD_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onAddWeapon);
  connect(ui->IDC_DELETE_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onDeleteWeapon);
  connect(ui->IDC_LOCK_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onLockWeapon);
  connect(ui->IDC_CHECKIN_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onCheckinWeapon);
  connect(ui->IDC_WEAPONS_OUT, &QPushButton::clicked, this, &WorldWeaponsDialog::onWeaponsOut);
  connect(ui->IDC_NEXT_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onNextWeapon);
  connect(ui->IDC_PREV_WEAPON, &QPushButton::clicked, this, &WorldWeaponsDialog::onPrevWeapon);
  connect(ui->IDC_OVERRIDE, &QPushButton::clicked, this, &WorldWeaponsDialog::onOverride);
  connect(ui->IDC_WEAPON_COPY_BUTTON, &QPushButton::clicked, this, &WorldWeaponsDialog::onCopy);
  connect(ui->IDC_WEAPON_PASTE_BUTTON, &QPushButton::clicked, [this]() {
    QMessageBox::critical(nullptr, "onPaste failure", "Weapon pasted.");
  });
  connect(ui->IDC_CHANGE_NAME, &QPushButton::clicked, this, &WorldWeaponsDialog::onChangeName);
  connect(ui->IDC_EDIT_PHYSICS, &QPushButton::clicked, this, &WorldWeaponsDialog::onEditPhysics);
  connect(ui->IDC_DEFAULT_SIZE, &QPushButton::clicked, [this]() {
    if (auto w = data()) ComputeDefaultSize(object_type::weapon, w->fire_image_handle, w->size), updateDialog();
  });

  connect(ui->IDC_ENERGY_RADIO, &QRadioButton::clicked, [this]() { if (auto w = data()) w->flags.matter_weapon = false; });
  connect(ui->IDC_MATTER_RADIO, &QRadioButton::clicked, [this]() { if (auto w = data()) w->flags.matter_weapon = true; });

  connect(ui->IDC_WEAPON_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), this,
    &WorldWeaponsDialog::onWeaponPulldownChanged);

  bindEdits();
  bindChecks();
  bindCombos();

  updateDialog();
}

WorldWeaponsDialog::~WorldWeaponsDialog() { saveWeaponsOnClose(); }

void WorldWeaponsDialog::saveWeaponsOnClose() {
  if (Network_up)
    for (int i = 0; i < MAX_TRACKLOCKS; i++)
      if (GlobalTrackLocks[i].used == 1 && GlobalTrackLocks[i].pagetype == PAGETYPE_WEAPON) {
        if (auto t = FindWeaponName(GlobalTrackLocks[i].name))
          mng_ReplacePage(Weapons[*t].name, Weapons[*t].name, *t, PAGETYPE_WEAPON, 1);
      }
}

void WorldWeaponsDialog::bindEdits()
{
  connect(ui->IDC_WEAPON_DAMAGE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->player_damage = ui->IDC_WEAPON_DAMAGE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_GENERIC_DAMAGE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->generic_damage = ui->IDC_WEAPON_GENERIC_DAMAGE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_ALPHA_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->alpha = ui->IDC_WEAPON_ALPHA_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_BLOB_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->size = ui->IDC_WEAPON_BLOB_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_LIFE_TIME_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->life_time = ui->IDC_WEAPON_LIFE_TIME_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_THRUST_TIME_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->thrust_time = ui->IDC_WEAPON_THRUST_TIME_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_IMPACT_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->impact_size = ui->IDC_WEAPON_IMPACT_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_IMPACT_TIME_EDIT2, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->impact_time = ui->IDC_WEAPON_IMPACT_TIME_EDIT2->text().toFloat();
  });
  connect(ui->IDC_WEAPON_IMPACT_DAMAGE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->impact_player_damage = ui->IDC_WEAPON_IMPACT_DAMAGE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_IMPACT_GENERIC_DAMAGE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->impact_generic_damage = ui->IDC_WEAPON_IMPACT_GENERIC_DAMAGE_EDIT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_IMPACT_FORCE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->impact_force = ui->IDC_WEAPON_IMPACT_FORCE_EDIT->text().toFloat();
  });
  connect(ui->IDC_EXPLODE_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->explode_size = ui->IDC_EXPLODE_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_EXPLODE_TIME_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->explode_time = ui->IDC_EXPLODE_TIME_EDIT->text().toFloat();
  });
  connect(ui->IDC_PARTICLE_LIFE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->particle_life = ui->IDC_PARTICLE_LIFE_EDIT->text().toFloat();
  });
  connect(ui->IDC_PARTICLE_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->particle_size = ui->IDC_PARTICLE_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_GRAVITY_SIZE, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->gravity_size = ui->IDC_GRAVITY_SIZE->text().toFloat();
  });
  connect(ui->IDC_GRAVITY_TIME, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->gravity_time = ui->IDC_GRAVITY_TIME->text().toFloat();
  });
  connect(ui->IDC_CUSTOM_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->custom_size = ui->IDC_CUSTOM_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_HOMING_FOV_TEXT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->homing_fov = ui->IDC_HOMING_FOV_TEXT->text().toFloat();
  });
  connect(ui->IDC_WEAPON_SCORCH_SIZE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->scorch_size = ui->IDC_WEAPON_SCORCH_SIZE_EDIT->text().toFloat();
  });
  connect(ui->IDC_TERRIAN_DAMAGE_SIZE, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->terrain_damage_size = ui->IDC_TERRIAN_DAMAGE_SIZE->text().toFloat();
  });
  connect(ui->IDC_WEAPON_SPAWN_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->spawn_count = (uint8_t)ui->IDC_WEAPON_SPAWN_EDIT->text().toInt();
  });
  connect(ui->IDC_ALTERNATE_CHANCE_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->alternate_chance = (uint8_t)ui->IDC_ALTERNATE_CHANCE_EDIT->text().toInt();
  });
  connect(ui->IDC_PARTICLE_COUNT_EDIT, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->particle_count = (uint8_t)ui->IDC_PARTICLE_COUNT_EDIT->text().toInt();
  });
  connect(ui->IDC_TERRAIN_DAMAGE_DEPTH, &QLineEdit::editingFinished, [this]() {
      if(auto w = data()) w->terrain_damage_depth = (uint8_t)ui->IDC_TERRAIN_DAMAGE_DEPTH->text().toInt();
  });
}

void WorldWeaponsDialog::bindChecks() {
  if(data())
  {
    auto wf = [this]() -> weapon_flags_t& { static weapon_flags_t dummy_wf; return data() ? data()->flags : dummy_wf; };
    auto pf = [this]() -> physics_flags_t& { static physics_flags_t dummy_pf; return data() ? data()->phys_info.flags : dummy_pf; };

    connect(ui->IDC_SMOKE_CHECK, &QCheckBox::toggled, [&wf](bool checked) { wf().smoke = checked; });
    connect(ui->IDC_REVERSE_SMOKE_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().reverse_smoke = checked; });
    connect(ui->IDC_PLANAR_SMOKE_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().planar_smoke = checked; });
    connect(ui->IDC_ELECTRICAL_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().electrical = checked; });
    connect(ui->IDC_SPRAY_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().spray = checked; });
    connect(ui->IDC_INVISIBLE,&QCheckBox::toggled,[&wf](bool checked){ wf().invisible = checked; });
    connect(ui->IDC_RING,&QCheckBox::toggled,[&wf](bool checked){ wf().ring = checked; });
    connect(ui->IDC_SATURATE_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().saturate = checked; });
    connect(ui->IDC_PLANAR_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().planar = checked; });
    connect(ui->IDC_ENABLE_CAMERA,&QCheckBox::toggled,[&wf](bool checked){ wf().enable_camera = checked; });
    connect(ui->IDC_MUZZLE_FLASH,&QCheckBox::toggled,[&wf](bool checked){ wf().muzzle = checked; });
    connect(ui->IDC_NAPALM,&QCheckBox::toggled,[&wf](bool checked){ wf().napalm = checked; });
    connect(ui->IDC_MICROWAVE,&QCheckBox::toggled,[&wf](bool checked){ wf().microwave = checked; });
    connect(ui->IDC_SILENT_HOMING_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().silent_homing = checked; });
    connect(ui->IDC_EXPLODE_RING,&QCheckBox::toggled,[&wf](bool checked){ wf().blast_ring = checked; });
    connect(ui->IDC_EXPANDING_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().expand = checked; });
    connect(ui->IDC_PLANAR_BLAST,&QCheckBox::toggled,[&wf](bool checked){ wf().planar_blast = checked; });
    connect(ui->IDC_TIMEOUT_WALL_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().timeout_wall = checked; });
    connect(ui->IDC_GRAVITY_FIELD_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().gravity_field = checked; });
    connect(ui->IDC_COUNTERMEASURE_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().countermeasure = checked; });
    connect(ui->IDC_SPAWNS_ROBOT_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().spawns_robot = checked; });
    connect(ui->IDC_SPAWNS_ON_IMPACT,&QCheckBox::toggled,[&wf](bool checked){ wf().spawns_impact = checked; });
    connect(ui->IDC_SPAWNS_ON_TIMEOUT,&QCheckBox::toggled,[&wf](bool checked){ wf().spawns_timeout = checked; });
    connect(ui->IDC_HOMED_SPLIT_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().homing_split = checked; });
    connect(ui->IDC_INSTANT_CHECK,&QCheckBox::toggled,[&wf](bool checked){ wf().streamer = checked; });

    connect(ui->IDC_WEAPON_HOMING_CHECK,&QCheckBox::toggled,[&pf](bool checked){ pf().homing = checked; });
    connect(ui->IDC_WEAPON_COLLIDE_WITH_SIBLING_CHECK,&QCheckBox::toggled,[&pf](bool checked){ pf().hits_siblings = checked; });
    connect(ui->IDC_WEAPON_USE_PARENT_VELOCITY_CHECK,&QCheckBox::toggled,[&pf](bool checked){ pf().uses_parent_velocity = checked; });
  }
}

void WorldWeaponsDialog::bindCombos() {
  connect(ui->IDC_FIRE_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->sounds[WSI_FIRE] = ui->IDC_FIRE_SOUND_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_WEAPON_WALL_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->sounds[WSI_IMPACT_WALL] = ui->IDC_WEAPON_WALL_SOUND_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_FLYING_SOUND_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->sounds[WSI_FLYING] = ui->IDC_FLYING_SOUND_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_WEAPON_BOUNCE_SOUND_COMBO, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->sounds[WSI_BOUNCE] = ui->IDC_WEAPON_BOUNCE_SOUND_COMBO->currentData().toInt();
  });
  connect(ui->IDC_EXPLODE_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->explode_image_handle = ui->IDC_EXPLODE_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_SMOKE_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->smoke_handle = ui->IDC_SMOKE_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_PARTICLE_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->particle_handle = ui->IDC_PARTICLE_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_WEAPON_SPAWN_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->spawn_handle = ui->IDC_WEAPON_SPAWN_PULLDOWN->currentData().toInt();
  });
  connect(ui->IDC_SPAWN_ROBOT_PULLDOWN, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
    if (auto w = data()) w->robot_spawn_handle = ui->IDC_SPAWN_ROBOT_PULLDOWN->currentData().toInt();
  });
}

void WorldWeaponsDialog::updateDialog()
{
  ui->IDC_NEXT_WEAPON->setEnabled(static_cast<int>(Weapons.size()) >= 1);
  ui->IDC_PREV_WEAPON->setEnabled(static_cast<int>(Weapons.size()) >= 1);

  if (!Network_up)
  {
    ui->IDC_LOCK_WEAPON->setEnabled(false);
    ui->IDC_CHECKIN_WEAPON->setEnabled(false);
    ui->IDC_OVERRIDE->setEnabled(false);
  }
  else if(data())
  {
    auto weapon = data();
    const weapon_flags_t& wf = weapon->flags;
    const physics_flags_t& pf = weapon->phys_info.flags;

    ui->IDC_WEAPON_DAMAGE_EDIT->setText(QString::number(weapon->player_damage));
    ui->IDC_WEAPON_GENERIC_DAMAGE_EDIT->setText(QString::number(weapon->generic_damage));
    ui->IDC_WEAPON_ALPHA_EDIT->setText(QString::number(weapon->alpha));
    ui->IDC_WEAPON_BLOB_SIZE_EDIT->setText(QString::number(weapon->size));
    ui->IDC_WEAPON_LIFE_TIME_EDIT->setText(QString::number(weapon->life_time));
    ui->IDC_WEAPON_THRUST_TIME_EDIT->setText(QString::number(weapon->thrust_time));
    ui->IDC_WEAPON_IMPACT_SIZE_EDIT->setText(QString::number(weapon->impact_size));
    ui->IDC_WEAPON_IMPACT_TIME_EDIT2->setText(QString::number(weapon->impact_time));
    ui->IDC_WEAPON_IMPACT_DAMAGE_EDIT->setText(QString::number(weapon->impact_player_damage));
    ui->IDC_WEAPON_IMPACT_GENERIC_DAMAGE_EDIT->setText(QString::number(weapon->impact_generic_damage));
    ui->IDC_WEAPON_IMPACT_FORCE_EDIT->setText(QString::number(weapon->impact_force));
    ui->IDC_EXPLODE_SIZE_EDIT->setText(QString::number(weapon->explode_size));
    ui->IDC_EXPLODE_TIME_EDIT->setText(QString::number(weapon->explode_time));
    ui->IDC_PARTICLE_LIFE_EDIT->setText(QString::number(weapon->particle_life));
    ui->IDC_PARTICLE_SIZE_EDIT->setText(QString::number(weapon->particle_size));
    ui->IDC_GRAVITY_SIZE->setText(QString::number(weapon->gravity_size));
    ui->IDC_GRAVITY_TIME->setText(QString::number(weapon->gravity_time));
    ui->IDC_CUSTOM_SIZE_EDIT->setText(QString::number(weapon->custom_size));
    ui->IDC_HOMING_FOV_TEXT->setText(QString::number(weapon->homing_fov));
    ui->IDC_WEAPON_SCORCH_SIZE_EDIT->setText(QString::number(weapon->scorch_size));
    ui->IDC_TERRIAN_DAMAGE_SIZE->setText(QString::number(weapon->terrain_damage_size));

    ui->IDC_WEAPON_SPAWN_EDIT->setText(QString::number(weapon->spawn_count));
    ui->IDC_ALTERNATE_CHANCE_EDIT->setText(QString::number(weapon->alternate_chance));
    ui->IDC_PARTICLE_COUNT_EDIT->setText(QString::number(weapon->particle_count));
    ui->IDC_TERRAIN_DAMAGE_DEPTH->setText(QString::number(weapon->terrain_damage_depth));

    ui->IDC_SMOKE_CHECK->setChecked(wf.smoke);
    ui->IDC_REVERSE_SMOKE_CHECK->setChecked(wf.reverse_smoke);
    ui->IDC_PLANAR_SMOKE_CHECK->setChecked(wf.planar_smoke);
    ui->IDC_ELECTRICAL_CHECK->setChecked(wf.electrical);
    ui->IDC_SPRAY_CHECK->setChecked(wf.spray);
    ui->IDC_INVISIBLE->setChecked(wf.invisible);
    ui->IDC_RING->setChecked(wf.ring);
    ui->IDC_SATURATE_CHECK->setChecked(wf.saturate);
    ui->IDC_PLANAR_CHECK->setChecked(wf.planar);
    ui->IDC_ENABLE_CAMERA->setChecked(wf.enable_camera);
    ui->IDC_MUZZLE_FLASH->setChecked(wf.muzzle);
    ui->IDC_NAPALM->setChecked(wf.napalm);
    ui->IDC_MICROWAVE->setChecked(wf.microwave);
    ui->IDC_SILENT_HOMING_CHECK->setChecked(wf.silent_homing);
    ui->IDC_EXPLODE_RING->setChecked(wf.blast_ring);
    ui->IDC_EXPANDING_CHECK->setChecked(wf.expand);
    ui->IDC_PLANAR_BLAST->setChecked(wf.planar_blast);
    ui->IDC_TIMEOUT_WALL_CHECK->setChecked(wf.timeout_wall);
    ui->IDC_GRAVITY_FIELD_CHECK->setChecked(wf.gravity_field);
    ui->IDC_COUNTERMEASURE_CHECK->setChecked(wf.countermeasure);
    ui->IDC_SPAWNS_ROBOT_CHECK->setChecked(wf.spawns_robot);
    ui->IDC_SPAWNS_ON_IMPACT->setChecked(wf.spawns_impact);
    ui->IDC_SPAWNS_ON_TIMEOUT->setChecked(wf.spawns_timeout);
    ui->IDC_HOMED_SPLIT_CHECK->setChecked(wf.homing_split);
    ui->IDC_INSTANT_CHECK->setChecked(wf.streamer);

    ui->IDC_WEAPON_HOMING_CHECK->setChecked(pf.homing);
    ui->IDC_WEAPON_COLLIDE_WITH_SIBLING_CHECK->setChecked(pf.hits_siblings);
    ui->IDC_WEAPON_USE_PARENT_VELOCITY_CHECK->setChecked(pf.uses_parent_velocity);

    ui->IDC_ENERGY_RADIO->setChecked(!wf.matter_weapon);
    ui->IDC_MATTER_RADIO->setChecked(wf.matter_weapon);

    if (!mng_FindTrackLock(weapon->name, PAGETYPE_WEAPON)) {
      ui->IDC_CHECKIN_WEAPON->setEnabled(false);
      ui->IDC_LOCK_WEAPON->setEnabled(true);
    } else {
      ui->IDC_CHECKIN_WEAPON->setEnabled(true);
      ui->IDC_LOCK_WEAPON->setEnabled(false);
    }


    {
      QComboBox *combo = ui->IDC_WEAPON_PULLDOWN;
      QSignalBlocker blocker(combo);
      combo->clear();
      for (int i = 0; i < static_cast<int>(Weapons.size()); i++)
        if (Weapons.is_used(i))
          combo->addItem(QString::fromStdString(Weapons[i].name));
      combo->setCurrentText(QString::fromStdString(weapon->name));
    }

    populateSoundCombo(ui->IDC_FIRE_SOUND_PULLDOWN, weapon->sounds[WSI_FIRE]);
    populateSoundCombo(ui->IDC_WEAPON_WALL_SOUND_PULLDOWN, weapon->sounds[WSI_IMPACT_WALL]);
    populateSoundCombo(ui->IDC_FLYING_SOUND_PULLDOWN, weapon->sounds[WSI_FLYING]);
    populateSoundCombo(ui->IDC_WEAPON_BOUNCE_SOUND_COMBO, weapon->sounds[WSI_BOUNCE]);
  }
}

void WorldWeaponsDialog::onAddWeapon() {
  if (!Network_up) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }
  bool ok = false;
  const QString name =
      QInputDialog::getText(this, "Weapon", "Enter a name for your weapon:", QLineEdit::Normal, "", &ok);
  if (!ok || name.isEmpty())
    return;
  if (FindWeaponName(name.toStdString())) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There is already a weapon with that name.");
    return;
  }
  const index_t handle = AllocWeapon();
  if (!handle) {
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Cannot add weapon: There are no free weapon slots.");
    return;
  }
  Weapons[*handle].name = name.toStdString();
  mng_AllocTrackLock(Weapons[*handle].name, PAGETYPE_WEAPON);
  app.current_weapon = *handle;
  RemapWeapons();
  updateDialog();

}

void WorldWeaponsDialog::onDeleteWeapon() {
  if(auto w = data())
  {
    const uint32_t n = app.current_weapon ? *app.current_weapon : 0;
    const int tl = mng_FindTrackLock(w->name, PAGETYPE_WEAPON).value_or(-1);
    if (tl == -1) {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "This weapon is not yours to delete.  Lock first.");
      return;
    }
    if (QMessageBox::question(this, "Delete weapon",
                              QString("Are you sure you want to delete this weapon? %1").arg(QString::fromStdString(w->name))) !=
        QMessageBox::Yes)
      return;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock pl;
    pl.name = w->name;
    pl.pagetype = PAGETYPE_WEAPON;
    if (mng_CheckIfPageOwned(&pl, TableUser.toStdString()) != 1) {
      mng_FreeTrackLock(tl);
      Q_ASSERT(mng_DeletePage(w->name, PAGETYPE_WEAPON, 1));
    } else {
      mng_FreeTrackLock(tl);
      mng_DeletePage(w->name, PAGETYPE_WEAPON, 1);
      mng_DeletePage(w->name, PAGETYPE_WEAPON, 0);
      mng_DeletePagelock(w->name, PAGETYPE_WEAPON);
    }
    if (const index_t next = GetNextWeapon(n))
      if (next) app.current_weapon = *next;
    FreeWeapon(n);
    mng_EraseLocker();
    QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Weapon deleted.");
    RemapWeapons();
    updateDialog();
  }
}

void WorldWeaponsDialog::onLockWeapon()
{
  if(auto w = data())
  {
    const uint32_t n = app.current_weapon ? *app.current_weapon : 0;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock temp_pl;
    mngs_weapon_page weaponpage;
    temp_pl.name = w->name;
    temp_pl.pagetype = PAGETYPE_WEAPON;
    const int r = mng_CheckIfPageLocked(&temp_pl);
    if (r == 2) {
      if (QMessageBox::question(this, "Are you sure?",
                            "This page is not even in the table file, or the database maybe corrupt.  Override to "
                                "'Unlocked'? (Select NO if you don't know what you're doing)") == QMessageBox::Yes) {
        temp_pl.holder = "UNLOCKED";
        if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
          QMessageBox::critical(this, "Error!", ErrorString);
      }
    } else if (r < 0) {
      QMessageBox::critical(this, "Error!", ErrorString);
    } else if (r == 1) {
      QMessageBox::information(this, "Information", InfoString);
    } else {
      temp_pl.holder = TableUser.toStdString();
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
        QMessageBox::critical(this, "Error!", ErrorString);
        mng_EraseLocker();
        return;
      }
      if (mng_FindSpecificWeaponPage(temp_pl.name, &weaponpage, 0)) {
        if (mng_AssignWeaponPageToWeapon(&weaponpage, n)) {
          if (!mng_ReplacePage(w->name, w->name, n, PAGETYPE_WEAPON, 1)) {
            QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was problem writing that page locally!");
            mng_EraseLocker();
            return;
          }
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Weapon locked.");
        } else {
          QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "There was a problem loading this weapon.");
        }
        mng_AllocTrackLock(w->name, PAGETYPE_WEAPON);
      } else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Couldn't find that weapon in the table file!");
      }
    }
    mng_EraseLocker();
    updateDialog();
  }
}

void WorldWeaponsDialog::onCheckinWeapon() {
  if(auto w = data())
  {
    const uint32_t n = app.current_weapon ? *app.current_weapon : 0;
    if (!mng_MakeLocker())
      return;
    mngs_Pagelock temp_pl;
    temp_pl.name = w->name;
    temp_pl.pagetype = PAGETYPE_WEAPON;
    const int r = mng_CheckIfPageOwned(&temp_pl, TableUser.toStdString());
    if (r < 0)
      QMessageBox::critical(this, "Error!", ErrorString);
    else if (r == 0)
      QMessageBox::information(this, "Information", InfoString);
    else {
      temp_pl.holder = "UNLOCKED";
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
        QMessageBox::critical(this, "Error!", ErrorString);
        mng_EraseLocker();
        return;
      }
      if (!mng_ReplacePage(w->name, w->name, n, PAGETYPE_WEAPON, 0))
        QMessageBox::critical(this, "Error!", ErrorString);
      else {
        QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "Weapon checked in.");
        Q_ASSERT(mng_DeletePage(w->name, PAGETYPE_WEAPON, 1) == 1);
        mng_EraseLocker();
        const int p = mng_FindTrackLock(w->name, PAGETYPE_WEAPON).value_or(-1);
        Q_ASSERT(p != -1);
        mng_FreeTrackLock(p);
      }
    }
    mng_EraseLocker();
    updateDialog();
  }
}

void WorldWeaponsDialog::onWeaponsOut() {
  QString str = QString("User %1 has these weapons held locally:\n\n").arg(TableUser);
  int total = 0;
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_WEAPON) {
      str += QString::fromStdString(GlobalTrackLocks[i].name);
      str += "\n";
      total++;
    }
  }
  if (total != 0)
    QMessageBox::information(this, "Weapons", str);
}

void WorldWeaponsDialog::onNextWeapon() {
  if (app.current_weapon) { auto next = GetNextWeapon(*app.current_weapon); if (next) app.current_weapon = *next; }
  updateDialog();
}
void WorldWeaponsDialog::onPrevWeapon() {
  if (app.current_weapon) { auto prev = GetPrevWeapon(*app.current_weapon); if (prev) app.current_weapon = *prev; }
  updateDialog();
}

void WorldWeaponsDialog::onWeaponPulldownChanged()
{
  if (const index_t i = FindWeaponName(ui->IDC_WEAPON_PULLDOWN->currentText().toStdString()); i)
  {
    app.current_weapon = *i;
    updateDialog();
  }
}

void WorldWeaponsDialog::onOverride() {
  mngs_Pagelock temp_pl;
  temp_pl.name = data()->name;
  temp_pl.pagetype = PAGETYPE_WEAPON;
  mng_OverrideToUnlocked(&temp_pl);
}

void WorldWeaponsDialog::onCopy() {
  if (auto w = data())
  {
      if(!mng_FindTrackLock(w->name, PAGETYPE_WEAPON)) {
      QMessageBox::warning(this, "Unable to copy", "You must lock this weapon before you can copy it.");
      return;
    }
    QMessageBox::information(this, "Success", "Weapon copied.");
  }
}

void WorldWeaponsDialog::onChangeName()
{
  if(auto w = data())
  {
    const index_t p = mng_FindTrackLock(w->name, PAGETYPE_WEAPON);
    if (!p) {
      QMessageBox::warning(this, "Unable to rename", "You must lock this weapon if you wish to change its name.");
      return;
    }
    bool ok = false;
    const QString name = QInputDialog::getText(this, "Weapon name", "Enter a new name for this weapon:",
                                               QLineEdit::Normal, QString::fromStdString(w->name), &ok);
    if (!ok || name.isEmpty())
      return;
    if (FindWeaponName(name.toStdString()))
    {
      QMessageBox::critical(nullptr, QString("%1 failure").arg(__func__), "That name is taken, please choose another.");
      return;
    }
    w->name = name.toStdString();
    GlobalTrackLocks[*p].name = w->name;
    RemapWeapons();
    updateDialog();
  }
}

void WorldWeaponsDialog::onEditPhysics() {
  if(auto w = data())
  {
    PhysicsDialog dlg(this);
    dlg.setData(w->phys_info);
    if(dlg.exec() == QDialog::Accepted)
      w->phys_info = dlg.getData();
  }
}

