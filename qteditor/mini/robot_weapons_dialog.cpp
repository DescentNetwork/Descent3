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

#include "robot_weapons_dialog.h"
#include "ui_robot_weapon_dialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include <cmath>

#include "polymodel.h"
#include "robotfirestruct_external.h"
#include "robotfirestruct.h"
#include "sound_combo.h"
#include "ssl_lib.h"
#include "weapon.h"


namespace {
const char *kNumMasksEdit = "IDC_GB_NUM_MASKS_EDIT";
const char *kMaskLatencyEdit[MAX_WB_FIRING_MASKS] = {
    "IDC_GB_MASK_LATENCY_0_EDIT", "IDC_GB_MASK_LATENCY_1_EDIT", "IDC_GB_MASK_LATENCY_2_EDIT",
    "IDC_GB_MASK_LATENCY_3_EDIT", "IDC_GB_MASK_LATENCY_4_EDIT", "IDC_GB_MASK_LATENCY_5_EDIT",
    "IDC_GB_MASK_LATENCY_6_EDIT", "IDC_GB_MASK_LATENCY_7_EDIT"};
const char *kFireSoundCombo[MAX_WB_FIRING_MASKS] = {
    "IDC_WB_FIRE0_SOUND_PULLDOWN", "IDC_WB_FIRE1_SOUND_PULLDOWN", "IDC_WB_FIRE2_SOUND_PULLDOWN",
    "IDC_WB_FIRE3_SOUND_PULLDOWN", "IDC_WB_FIRE4_SOUND_PULLDOWN", "IDC_WB_FIRE5_SOUND_PULLDOWN",
    "IDC_WB_FIRE6_SOUND_PULLDOWN", "IDC_WB_FIRE7_SOUND_PULLDOWN"};
const char *kStartTickEdit[MAX_WB_FIRING_MASKS] = {
    "IDC_WB_FIRE0_START_TICK", "IDC_WB_FIRE1_START_TICK", "IDC_WB_FIRE2_START_TICK", "IDC_WB_FIRE3_START_TICK",
    "IDC_WB_FIRE4_START_TICK", "IDC_WB_FIRE5_START_TICK", "IDC_WB_FIRE6_START_TICK", "IDC_WB_FIRE7_START_TICK"};
const char *kFireTickEdit[MAX_WB_FIRING_MASKS] = {
    "IDC_WB_FIRE0_FIRE_TICK", "IDC_WB_FIRE1_FIRE_TICK", "IDC_WB_FIRE2_FIRE_TICK", "IDC_WB_FIRE3_FIRE_TICK",
    "IDC_WB_FIRE4_FIRE_TICK", "IDC_WB_FIRE5_FIRE_TICK", "IDC_WB_FIRE6_FIRE_TICK", "IDC_WB_FIRE7_FIRE_TICK"};
const char *kEndTickEdit[MAX_WB_FIRING_MASKS] = {
    "IDC_WB_FIRE0_END_TICK", "IDC_WB_FIRE1_END_TICK", "IDC_WB_FIRE2_END_TICK", "IDC_WB_FIRE3_END_TICK",
    "IDC_WB_FIRE4_END_TICK", "IDC_WB_FIRE5_END_TICK", "IDC_WB_FIRE6_END_TICK", "IDC_WB_FIRE7_END_TICK"};
const char *kCycleTimeEdit[MAX_WB_FIRING_MASKS] = {
    "IDC_WB_FIRE0_CYCLE_TIME", "IDC_WB_FIRE1_CYCLE_TIME", "IDC_WB_FIRE2_CYCLE_TIME", "IDC_WB_FIRE3_CYCLE_TIME",
    "IDC_WB_FIRE4_CYCLE_TIME", "IDC_WB_FIRE5_CYCLE_TIME", "IDC_WB_FIRE6_CYCLE_TIME", "IDC_WB_FIRE7_CYCLE_TIME"};
const char *kBarrelCombo[MAX_WB_GUNPOINTS] = {
    "IDC_BARREL_0_COMBO", "IDC_BARREL_1_COMBO", "IDC_BARREL_2_COMBO", "IDC_BARREL_3_COMBO",
    "IDC_BARREL_4_COMBO", "IDC_BARREL_5_COMBO", "IDC_BARREL_6_COMBO", "IDC_BARREL_7_COMBO"};
const char *kBarrelRadio[MAX_WB_GUNPOINTS] = {
    "IDC_BARREL_0_RADIO", "IDC_BARREL_1_RADIO", "IDC_BARREL_2_RADIO", "IDC_BARREL_3_RADIO",
    "IDC_BARREL_4_RADIO", "IDC_BARREL_5_RADIO", "IDC_BARREL_6_RADIO", "IDC_BARREL_7_RADIO"};
const char *kQuadCheck[MAX_WB_GUNPOINTS] = {
    "IDC_GP_MASK_Q_0", "IDC_GP_MASK_Q_1", "IDC_GP_MASK_Q_2", "IDC_GP_MASK_Q_3",
    "IDC_GP_MASK_Q_4", "IDC_GP_MASK_Q_5", "IDC_GP_MASK_Q_6", "IDC_GP_MASK_Q_7"};

QString maskCheckName(int mask, int gp) { return QString("IDC_GP_MASK_%1_%2").arg(mask).arg(gp); }

QLineEdit *lineEdit(QDialog *w, const QString &name) { return w->findChild<QLineEdit *>(name); }
QCheckBox *checkBox(QDialog *w, const QString &name) { return w->findChild<QCheckBox *>(name); }
} // namespace

RobotEditWeaponsDialog::RobotEditWeaponsDialog(otype_wb_info *static_wb, poly_model *pm, QWidget *parent)
    : QDialog(parent), ui(new Ui::RobotWeaponsDialog), m_wb(static_wb), m_pm(pm)
{
  ui->setupUi(this);
  // Barrel weapon combos: list used weapons.
  for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
    if (QComboBox *combo = findChild<QComboBox*>(kBarrelCombo[gp])) {
      for (int i = 0; i < MAX_WEAPONS; i++)
        if (Weapons[i].used)
          combo->addItem(Weapons[i].name, i);
    }
  }

  // Fire sound combos.
  for (int m = 0; m < MAX_WB_FIRING_MASKS; m++)
    populateSoundCombo(findChild<QComboBox*>(kFireSoundCombo[m]), -1);

  loadData();

  connect(this, &QDialog::accept, this, [this]() { getData(); accept(); });
}

RobotEditWeaponsDialog::~RobotEditWeaponsDialog() { delete ui; }

void RobotEditWeaponsDialog::loadData() {
  otype_wb_info *wb = m_wb;

  if (QLineEdit *edit = lineEdit(this, kNumMasksEdit))
    edit->setText(QString::number(wb->num_masks));

  for (int m = 0; m < MAX_WB_FIRING_MASKS; m++) {
    if (QLineEdit *edit = lineEdit(this, kMaskLatencyEdit[m]))
      edit->setText(QString::number(wb->gp_fire_wait[m]));
    if (QLineEdit *edit = lineEdit(this, kStartTickEdit[m]))
      edit->setText(QString::number(wb->anim_start_frame[m]));
    if (QLineEdit *edit = lineEdit(this, kFireTickEdit[m]))
      edit->setText(QString::number(wb->anim_fire_frame[m]));
    if (QLineEdit *edit = lineEdit(this, kEndTickEdit[m]))
      edit->setText(QString::number(wb->anim_end_frame[m]));
    if (QLineEdit *edit = lineEdit(this, kCycleTimeEdit[m]))
      edit->setText(QString::number(wb->anim_time[m]));
    if (QComboBox *combo = findChild<QComboBox*>(kFireSoundCombo[m]))
      setSoundComboSelected(combo, wb->fm_fire_sound_index[m]);

    for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
      if (QCheckBox *cb = checkBox(this, maskCheckName(m, gp)))
        cb->setChecked((wb->gp_fire_masks[m] & (1 << gp)) != 0);
    }
  }

  for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
    if (QComboBox *combo = findChild<QComboBox*>(kBarrelCombo[gp]))
      combo->setCurrentIndex(combo->findData(wb->gp_weapon_index[gp]));
    if (QCheckBox *cb = checkBox(this, kQuadCheck[gp]))
      cb->setChecked((wb->gp_quad_fire_mask & (1 << gp)) != 0);
    if (QRadioButton *radio = findChild<QRadioButton*>(kBarrelRadio[gp]))
      radio->setChecked(wb->aiming_gp_index == gp);
  }

  const float view_cone = wb->aiming_3d_dot > 1.0f ? 0.0f : acos(wb->aiming_3d_dot) * (360.0 / PI);
  const float xz_angle = wb->aiming_XZ_dot > 1.0f ? 0.0f : acos(wb->aiming_XZ_dot) * (360.0 / PI);
  if (QLineEdit *edit = ui->IDC_VIEW_CONE_ANGLE_EDIT)
    edit->setText(QString::number(view_cone));
  if (QLineEdit *edit = ui->IDC_MAX_DISTANCE_EDIT)
    edit->setText(QString::number(wb->aiming_3d_dist));
  if (QLineEdit *edit = ui->IDC_XZ_PLANE_ANGLE_EDIT)
    edit->setText(QString::number(xz_angle));

  if (QLineEdit *edit = ui->IDC_ENERGY_USAGE_EDIT)
    edit->setText(QString::number(wb->energy_usage));
  if (QLineEdit *edit = ui->IDC_AMMO_USAGE_EDIT)
    edit->setText(QString::number(wb->ammo_usage));

  if (QCheckBox *cb = ui->IDC_SPRAY_CHECK)
    cb->setChecked(wb->flags & WBF_SPRAY);
  if (QCheckBox *cb = ui->IDC_GUIDED_CHECK)
    cb->setChecked(wb->flags & WBF_GUIDED);
  if (QCheckBox *cb = ui->IDC_ONOFF_CHECK)
    cb->setChecked(wb->flags & WBF_ON_OFF);
  if (QCheckBox *cb = ui->IDC_WB_RANDOM_CHECK)
    cb->setChecked(wb->flags & WBF_RANDOM_FIRE_ORDER);
  if (QCheckBox *cb = ui->IDC_USER_TIMEOUT_CHECK)
    cb->setChecked(wb->flags & WBF_USER_TIMEOUT);
  if (QCheckBox *cb = ui->IDC_FORCE_TO_FVEC_CHECK)
    cb->setChecked(wb->flags & WBF_FIRE_FVEC);
  if (QCheckBox *cb = ui->IDC_FORCE_TO_TARGET_CHECK)
    cb->setChecked(wb->flags & WBF_FIRE_TARGET);
  if (QCheckBox *cb = ui->IDC_WBAIMFORWARD_CHECK)
    cb->setChecked(wb->flags & WBF_AIM_FVEC);
  if (QCheckBox *cb = ui->IDC_VIEW_CONE_ANGLE_CHECK)
    cb->setChecked(wb->flags & WBF_USE_CUSTOM_FOV);
  if (QCheckBox *cb = ui->IDC_MAX_DISTANCE_CHECK)
    cb->setChecked(wb->flags & WBF_USE_CUSTOM_MAX_DIST);

  const int anim_type = wb->flags & WBF_ANIM_MASKS;
  if (QRadioButton *radio = ui->IDC_WB_NO_ANIM_RADIO)
    radio->setChecked(anim_type == 0);
  if (QRadioButton *radio = ui->IDC_WB_LOCAL_ANIM_RADIO)
    radio->setChecked(anim_type == WBF_ANIM_LOCAL);
  if (QRadioButton *radio = ui->IDC_WB_MODEL_ANIM_RADIO)
    radio->setChecked(anim_type == WBF_ANIM_FULL);

  updateDialog();
}

void RobotEditWeaponsDialog::updateDialog() {
  otype_wb_info *wb = m_wb;
  int num_gps = 0;
  if (m_pm != nullptr && m_pm->num_wbs > 0)
    num_gps = m_pm->poly_wb[0].num_gps;

  const int num_masks = lineEdit(this, kNumMasksEdit)->text().toInt();

  for (int m = 0; m < MAX_WB_FIRING_MASKS; m++) {
    for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
      if (QCheckBox *cb = checkBox(this, maskCheckName(m, gp)))
        cb->setEnabled(m < num_masks && gp < num_gps);
    }
    if (QWidget *w = findChild<QWidget*>(kMaskLatencyEdit[m]))
      w->setEnabled(m < num_masks);
  }

  for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
    const bool has_gp = gp < num_gps;
    if (QWidget *w = findChild<QWidget*>(kBarrelCombo[gp]))
      w->setEnabled(has_gp);
    if (QWidget *w = findChild<QWidget*>(kBarrelRadio[gp]))
      w->setEnabled(has_gp);
    if (QCheckBox *cb = checkBox(this, kQuadCheck[gp]))
      cb->setEnabled(has_gp);
  }

  const bool custom_fov = ui->IDC_VIEW_CONE_ANGLE_CHECK->isChecked();
  const bool custom_dist = ui->IDC_MAX_DISTANCE_CHECK->isChecked();
  if (QWidget *w = ui->IDC_VIEW_CONE_ANGLE_EDIT)
    w->setEnabled(custom_fov);
  if (QWidget *w = ui->IDC_XZ_PLANE_ANGLE_EDIT)
    w->setEnabled(custom_fov);
  if (QWidget *w = ui->IDC_MAX_DISTANCE_EDIT)
    w->setEnabled(custom_dist);

  // Turret info (static read-only display).
  if (m_pm != nullptr && m_pm->num_wbs > 0 && m_pm->poly_wb[0].num_turrets > 0) {
    const int turret = m_pm->poly_wb[0].turret_index[0];
    if (QLabel *label = ui->IDC_TURRET_SOBJ_STATIC)
      label->setText(QString::number(turret));
    if (QLabel *label = ui->IDC_TURRET_FOV_STATIC)
      label->setText(QString::number(m_pm->submodel[turret].fov * 720.0));
    if (QLabel *label = ui->IDC_TURRET_REACTION_TIME_STATIC)
      label->setText(QString::number(m_pm->submodel[turret].think_interval, 'f', 2));
    if (QLabel *label = ui->IDC_TURRET_SPR_STATIC)
      label->setText(QString::number(1.0f / m_pm->submodel[turret].rps, 'f', 2));
  }

  (void)wb;
}

void RobotEditWeaponsDialog::getData() {
  otype_wb_info *wb = m_wb;

  wb->num_masks = lineEdit(this, kNumMasksEdit)->text().toInt();
  if (wb->num_masks < 1)
    wb->num_masks = 1;
  if (wb->num_masks > 8)
    wb->num_masks = 8;

  for (int m = 0; m < MAX_WB_FIRING_MASKS; m++) {
    wb->gp_fire_wait[m] = lineEdit(this, kMaskLatencyEdit[m])->text().toFloat();
    wb->anim_start_frame[m] = lineEdit(this, kStartTickEdit[m])->text().toFloat();
    wb->anim_fire_frame[m] = lineEdit(this, kFireTickEdit[m])->text().toFloat();
    wb->anim_end_frame[m] = lineEdit(this, kEndTickEdit[m])->text().toFloat();
    wb->anim_time[m] = lineEdit(this, kCycleTimeEdit[m])->text().toFloat();
    wb->fm_fire_sound_index[m] = soundComboSelected(findChild<QComboBox*>(kFireSoundCombo[m]));

    wb->gp_fire_masks[m] = 0;
    for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++)
      if (checkBox(this, maskCheckName(m, gp))->isChecked())
        wb->gp_fire_masks[m] |= (1 << gp);
  }

  wb->gp_quad_fire_mask = 0;
  for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++) {
    if (checkBox(this, kQuadCheck[gp])->isChecked())
      wb->gp_quad_fire_mask |= (1 << gp);
    if (QComboBox *combo = findChild<QComboBox*>(kBarrelCombo[gp]))
      wb->gp_weapon_index[gp] = combo->currentData().toInt();
  }

  for (int gp = 0; gp < MAX_WB_GUNPOINTS; gp++)
    if (findChild<QRadioButton*>(kBarrelRadio[gp])->isChecked())
      wb->aiming_gp_index = gp;

  wb->aiming_3d_dot = cos(ui->IDC_VIEW_CONE_ANGLE_EDIT->text().toFloat() * PI / 360.0);
  wb->aiming_3d_dist = ui->IDC_MAX_DISTANCE_EDIT->text().toFloat();
  wb->aiming_XZ_dot = cos(ui->IDC_XZ_PLANE_ANGLE_EDIT->text().toFloat() * PI / 360.0);

  wb->energy_usage = ui->IDC_ENERGY_USAGE_EDIT->text().toFloat();
  wb->ammo_usage = ui->IDC_AMMO_USAGE_EDIT->text().toFloat();

  wb->flags &= ~WBF_ANIM_MASKS;
  if (ui->IDC_WB_LOCAL_ANIM_RADIO->isChecked())
    wb->flags |= WBF_ANIM_LOCAL;
  else if (ui->IDC_WB_MODEL_ANIM_RADIO->isChecked())
    wb->flags |= WBF_ANIM_FULL;

  auto setFlag2 = [this, wb](uint16_t flag, const QString &name) {
    if (checkBox(this, name)->isChecked())
      wb->flags |= flag;
    else
      wb->flags &= ~flag;
  };
  setFlag2(WBF_SPRAY, "IDC_SPRAY_CHECK");
  setFlag2(WBF_GUIDED, "IDC_GUIDED_CHECK");
  setFlag2(WBF_ON_OFF, "IDC_ONOFF_CHECK");
  setFlag2(WBF_RANDOM_FIRE_ORDER, "IDC_WB_RANDOM_CHECK");
  setFlag2(WBF_USER_TIMEOUT, "IDC_USER_TIMEOUT_CHECK");
  setFlag2(WBF_FIRE_FVEC, "IDC_FORCE_TO_FVEC_CHECK");
  setFlag2(WBF_FIRE_TARGET, "IDC_FORCE_TO_TARGET_CHECK");
  setFlag2(WBF_AIM_FVEC, "IDC_WBAIMFORWARD_CHECK");
  setFlag2(WBF_USE_CUSTOM_FOV, "IDC_VIEW_CONE_ANGLE_CHECK");
  setFlag2(WBF_USE_CUSTOM_MAX_DIST, "IDC_MAX_DISTANCE_CHECK");
}

void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent) {
  RobotEditWeaponsDialog dlg(wb, pm, parent);
  dlg.exec();
}

