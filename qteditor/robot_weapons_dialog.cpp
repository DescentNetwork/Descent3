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


RobotEditWeaponsDialog::RobotEditWeaponsDialog(otype_wb_info *static_wb, poly_model *pm, QWidget *parent)
    : QDialog(parent), ui(new Ui::RobotWeaponsDialog), m_wb(static_wb), m_pm(pm)
{
  ui->setupUi(this);
  // Barrel weapon combos: list used weapons.
  {
    QComboBox *combo = ui->IDC_BARREL_0_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_1_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_2_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_3_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_4_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_5_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_6_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }
  {
    QComboBox *combo = ui->IDC_BARREL_7_COMBO;
    for (int i = 0; i < MAX_WEAPONS; i++)
      if (Weapons[i].used)
        combo->addItem(QString::fromStdString(Weapons[i].name), i);
  }

  // Fire sound combos.
  populateSoundCombo(ui->IDC_WB_FIRE0_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE1_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE2_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE3_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE4_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE5_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE6_SOUND_PULLDOWN, -1);
  populateSoundCombo(ui->IDC_WB_FIRE7_SOUND_PULLDOWN, -1);

  loadData();

  connect(this, &QDialog::accepted, this, [this]() { getData(); accept(); });
}

RobotEditWeaponsDialog::~RobotEditWeaponsDialog() { delete ui; }

void RobotEditWeaponsDialog::loadData() {
  otype_wb_info *wb = m_wb;

  ui->IDC_GB_NUM_MASKS_EDIT->setText(QString::number(wb->num_masks));

  ui->IDC_GB_MASK_LATENCY_0_EDIT->setText(QString::number(wb->gp_fire_wait[0]));
  ui->IDC_WB_FIRE0_START_TICK->setText(QString::number(wb->anim_start_frame[0]));
  ui->IDC_WB_FIRE0_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[0]));
  ui->IDC_WB_FIRE0_END_TICK->setText(QString::number(wb->anim_end_frame[0]));
  ui->IDC_WB_FIRE0_CYCLE_TIME->setText(QString::number(wb->anim_time[0]));
  setSoundComboSelected(ui->IDC_WB_FIRE0_SOUND_PULLDOWN, wb->fm_fire_sound_index[0]);
  ui->IDC_GP_MASK_0_0->setChecked((wb->gp_fire_masks[0] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_0_1->setChecked((wb->gp_fire_masks[0] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_0_2->setChecked((wb->gp_fire_masks[0] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_0_3->setChecked((wb->gp_fire_masks[0] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_0_4->setChecked((wb->gp_fire_masks[0] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_0_5->setChecked((wb->gp_fire_masks[0] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_0_6->setChecked((wb->gp_fire_masks[0] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_0_7->setChecked((wb->gp_fire_masks[0] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_1_EDIT->setText(QString::number(wb->gp_fire_wait[1]));
  ui->IDC_WB_FIRE1_START_TICK->setText(QString::number(wb->anim_start_frame[1]));
  ui->IDC_WB_FIRE1_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[1]));
  ui->IDC_WB_FIRE1_END_TICK->setText(QString::number(wb->anim_end_frame[1]));
  ui->IDC_WB_FIRE1_CYCLE_TIME->setText(QString::number(wb->anim_time[1]));
  setSoundComboSelected(ui->IDC_WB_FIRE1_SOUND_PULLDOWN, wb->fm_fire_sound_index[1]);
  ui->IDC_GP_MASK_1_0->setChecked((wb->gp_fire_masks[1] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_1_1->setChecked((wb->gp_fire_masks[1] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_1_2->setChecked((wb->gp_fire_masks[1] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_1_3->setChecked((wb->gp_fire_masks[1] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_1_4->setChecked((wb->gp_fire_masks[1] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_1_5->setChecked((wb->gp_fire_masks[1] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_1_6->setChecked((wb->gp_fire_masks[1] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_1_7->setChecked((wb->gp_fire_masks[1] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_2_EDIT->setText(QString::number(wb->gp_fire_wait[2]));
  ui->IDC_WB_FIRE2_START_TICK->setText(QString::number(wb->anim_start_frame[2]));
  ui->IDC_WB_FIRE2_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[2]));
  ui->IDC_WB_FIRE2_END_TICK->setText(QString::number(wb->anim_end_frame[2]));
  ui->IDC_WB_FIRE2_CYCLE_TIME->setText(QString::number(wb->anim_time[2]));
  setSoundComboSelected(ui->IDC_WB_FIRE2_SOUND_PULLDOWN, wb->fm_fire_sound_index[2]);
  ui->IDC_GP_MASK_2_0->setChecked((wb->gp_fire_masks[2] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_2_1->setChecked((wb->gp_fire_masks[2] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_2_2->setChecked((wb->gp_fire_masks[2] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_2_3->setChecked((wb->gp_fire_masks[2] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_2_4->setChecked((wb->gp_fire_masks[2] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_2_5->setChecked((wb->gp_fire_masks[2] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_2_6->setChecked((wb->gp_fire_masks[2] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_2_7->setChecked((wb->gp_fire_masks[2] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_3_EDIT->setText(QString::number(wb->gp_fire_wait[3]));
  ui->IDC_WB_FIRE3_START_TICK->setText(QString::number(wb->anim_start_frame[3]));
  ui->IDC_WB_FIRE3_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[3]));
  ui->IDC_WB_FIRE3_END_TICK->setText(QString::number(wb->anim_end_frame[3]));
  ui->IDC_WB_FIRE3_CYCLE_TIME->setText(QString::number(wb->anim_time[3]));
  setSoundComboSelected(ui->IDC_WB_FIRE3_SOUND_PULLDOWN, wb->fm_fire_sound_index[3]);
  ui->IDC_GP_MASK_3_0->setChecked((wb->gp_fire_masks[3] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_3_1->setChecked((wb->gp_fire_masks[3] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_3_2->setChecked((wb->gp_fire_masks[3] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_3_3->setChecked((wb->gp_fire_masks[3] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_3_4->setChecked((wb->gp_fire_masks[3] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_3_5->setChecked((wb->gp_fire_masks[3] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_3_6->setChecked((wb->gp_fire_masks[3] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_3_7->setChecked((wb->gp_fire_masks[3] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_4_EDIT->setText(QString::number(wb->gp_fire_wait[4]));
  ui->IDC_WB_FIRE4_START_TICK->setText(QString::number(wb->anim_start_frame[4]));
  ui->IDC_WB_FIRE4_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[4]));
  ui->IDC_WB_FIRE4_END_TICK->setText(QString::number(wb->anim_end_frame[4]));
  ui->IDC_WB_FIRE4_CYCLE_TIME->setText(QString::number(wb->anim_time[4]));
  setSoundComboSelected(ui->IDC_WB_FIRE4_SOUND_PULLDOWN, wb->fm_fire_sound_index[4]);
  ui->IDC_GP_MASK_4_0->setChecked((wb->gp_fire_masks[4] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_4_1->setChecked((wb->gp_fire_masks[4] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_4_2->setChecked((wb->gp_fire_masks[4] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_4_3->setChecked((wb->gp_fire_masks[4] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_4_4->setChecked((wb->gp_fire_masks[4] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_4_5->setChecked((wb->gp_fire_masks[4] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_4_6->setChecked((wb->gp_fire_masks[4] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_4_7->setChecked((wb->gp_fire_masks[4] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_5_EDIT->setText(QString::number(wb->gp_fire_wait[5]));
  ui->IDC_WB_FIRE5_START_TICK->setText(QString::number(wb->anim_start_frame[5]));
  ui->IDC_WB_FIRE5_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[5]));
  ui->IDC_WB_FIRE5_END_TICK->setText(QString::number(wb->anim_end_frame[5]));
  ui->IDC_WB_FIRE5_CYCLE_TIME->setText(QString::number(wb->anim_time[5]));
  setSoundComboSelected(ui->IDC_WB_FIRE5_SOUND_PULLDOWN, wb->fm_fire_sound_index[5]);
  ui->IDC_GP_MASK_5_0->setChecked((wb->gp_fire_masks[5] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_5_1->setChecked((wb->gp_fire_masks[5] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_5_2->setChecked((wb->gp_fire_masks[5] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_5_3->setChecked((wb->gp_fire_masks[5] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_5_4->setChecked((wb->gp_fire_masks[5] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_5_5->setChecked((wb->gp_fire_masks[5] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_5_6->setChecked((wb->gp_fire_masks[5] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_5_7->setChecked((wb->gp_fire_masks[5] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_6_EDIT->setText(QString::number(wb->gp_fire_wait[6]));
  ui->IDC_WB_FIRE6_START_TICK->setText(QString::number(wb->anim_start_frame[6]));
  ui->IDC_WB_FIRE6_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[6]));
  ui->IDC_WB_FIRE6_END_TICK->setText(QString::number(wb->anim_end_frame[6]));
  ui->IDC_WB_FIRE6_CYCLE_TIME->setText(QString::number(wb->anim_time[6]));
  setSoundComboSelected(ui->IDC_WB_FIRE6_SOUND_PULLDOWN, wb->fm_fire_sound_index[6]);
  ui->IDC_GP_MASK_6_0->setChecked((wb->gp_fire_masks[6] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_6_1->setChecked((wb->gp_fire_masks[6] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_6_2->setChecked((wb->gp_fire_masks[6] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_6_3->setChecked((wb->gp_fire_masks[6] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_6_4->setChecked((wb->gp_fire_masks[6] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_6_5->setChecked((wb->gp_fire_masks[6] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_6_6->setChecked((wb->gp_fire_masks[6] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_6_7->setChecked((wb->gp_fire_masks[6] & (1 << 7)) != 0);
  ui->IDC_GB_MASK_LATENCY_7_EDIT->setText(QString::number(wb->gp_fire_wait[7]));
  ui->IDC_WB_FIRE7_START_TICK->setText(QString::number(wb->anim_start_frame[7]));
  ui->IDC_WB_FIRE7_FIRE_TICK->setText(QString::number(wb->anim_fire_frame[7]));
  ui->IDC_WB_FIRE7_END_TICK->setText(QString::number(wb->anim_end_frame[7]));
  ui->IDC_WB_FIRE7_CYCLE_TIME->setText(QString::number(wb->anim_time[7]));
  setSoundComboSelected(ui->IDC_WB_FIRE7_SOUND_PULLDOWN, wb->fm_fire_sound_index[7]);
  ui->IDC_GP_MASK_7_0->setChecked((wb->gp_fire_masks[7] & (1 << 0)) != 0);
  ui->IDC_GP_MASK_7_1->setChecked((wb->gp_fire_masks[7] & (1 << 1)) != 0);
  ui->IDC_GP_MASK_7_2->setChecked((wb->gp_fire_masks[7] & (1 << 2)) != 0);
  ui->IDC_GP_MASK_7_3->setChecked((wb->gp_fire_masks[7] & (1 << 3)) != 0);
  ui->IDC_GP_MASK_7_4->setChecked((wb->gp_fire_masks[7] & (1 << 4)) != 0);
  ui->IDC_GP_MASK_7_5->setChecked((wb->gp_fire_masks[7] & (1 << 5)) != 0);
  ui->IDC_GP_MASK_7_6->setChecked((wb->gp_fire_masks[7] & (1 << 6)) != 0);
  ui->IDC_GP_MASK_7_7->setChecked((wb->gp_fire_masks[7] & (1 << 7)) != 0);

  ui->IDC_BARREL_0_COMBO->setCurrentIndex(ui->IDC_BARREL_0_COMBO->findData(wb->gp_weapon_index[0]));
  ui->IDC_GP_MASK_Q_0->setChecked((wb->gp_quad_fire_mask & (1 << 0)) != 0);
  ui->IDC_BARREL_0_RADIO->setChecked(wb->aiming_gp_index == 0);
  ui->IDC_BARREL_1_COMBO->setCurrentIndex(ui->IDC_BARREL_1_COMBO->findData(wb->gp_weapon_index[1]));
  ui->IDC_GP_MASK_Q_1->setChecked((wb->gp_quad_fire_mask & (1 << 1)) != 0);
  ui->IDC_BARREL_1_RADIO->setChecked(wb->aiming_gp_index == 1);
  ui->IDC_BARREL_2_COMBO->setCurrentIndex(ui->IDC_BARREL_2_COMBO->findData(wb->gp_weapon_index[2]));
  ui->IDC_GP_MASK_Q_2->setChecked((wb->gp_quad_fire_mask & (1 << 2)) != 0);
  ui->IDC_BARREL_2_RADIO->setChecked(wb->aiming_gp_index == 2);
  ui->IDC_BARREL_3_COMBO->setCurrentIndex(ui->IDC_BARREL_3_COMBO->findData(wb->gp_weapon_index[3]));
  ui->IDC_GP_MASK_Q_3->setChecked((wb->gp_quad_fire_mask & (1 << 3)) != 0);
  ui->IDC_BARREL_3_RADIO->setChecked(wb->aiming_gp_index == 3);
  ui->IDC_BARREL_4_COMBO->setCurrentIndex(ui->IDC_BARREL_4_COMBO->findData(wb->gp_weapon_index[4]));
  ui->IDC_GP_MASK_Q_4->setChecked((wb->gp_quad_fire_mask & (1 << 4)) != 0);
  ui->IDC_BARREL_4_RADIO->setChecked(wb->aiming_gp_index == 4);
  ui->IDC_BARREL_5_COMBO->setCurrentIndex(ui->IDC_BARREL_5_COMBO->findData(wb->gp_weapon_index[5]));
  ui->IDC_GP_MASK_Q_5->setChecked((wb->gp_quad_fire_mask & (1 << 5)) != 0);
  ui->IDC_BARREL_5_RADIO->setChecked(wb->aiming_gp_index == 5);
  ui->IDC_BARREL_6_COMBO->setCurrentIndex(ui->IDC_BARREL_6_COMBO->findData(wb->gp_weapon_index[6]));
  ui->IDC_GP_MASK_Q_6->setChecked((wb->gp_quad_fire_mask & (1 << 6)) != 0);
  ui->IDC_BARREL_6_RADIO->setChecked(wb->aiming_gp_index == 6);
  ui->IDC_BARREL_7_COMBO->setCurrentIndex(ui->IDC_BARREL_7_COMBO->findData(wb->gp_weapon_index[7]));
  ui->IDC_GP_MASK_Q_7->setChecked((wb->gp_quad_fire_mask & (1 << 7)) != 0);
  ui->IDC_BARREL_7_RADIO->setChecked(wb->aiming_gp_index == 7);

  const float view_cone = wb->aiming_3d_dot > 1.0f ? 0.0f : acos(wb->aiming_3d_dot) * (360.0 / PI);
  const float xz_angle = wb->aiming_XZ_dot > 1.0f ? 0.0f : acos(wb->aiming_XZ_dot) * (360.0 / PI);
  ui->IDC_VIEW_CONE_ANGLE_EDIT->setText(QString::number(view_cone));
  ui->IDC_MAX_DISTANCE_EDIT->setText(QString::number(wb->aiming_3d_dist));
  ui->IDC_XZ_PLANE_ANGLE_EDIT->setText(QString::number(xz_angle));

  ui->IDC_ENERGY_USAGE_EDIT->setText(QString::number(wb->energy_usage));
  ui->IDC_AMMO_USAGE_EDIT->setText(QString::number(wb->ammo_usage));

  ui->IDC_SPRAY_CHECK->setChecked(wb->flags.spray);
  ui->IDC_GUIDED_CHECK->setChecked(wb->flags.guided);
  ui->IDC_ONOFF_CHECK->setChecked(wb->flags.on_off);
  ui->IDC_WB_RANDOM_CHECK->setChecked(wb->flags.random_fire_order);
  ui->IDC_USER_TIMEOUT_CHECK->setChecked(wb->flags.user_timeout);
  ui->IDC_FORCE_TO_FVEC_CHECK->setChecked(wb->flags.fire_fvec);
  ui->IDC_FORCE_TO_TARGET_CHECK->setChecked(wb->flags.fire_target);
  ui->IDC_WBAIMFORWARD_CHECK->setChecked(wb->flags.aim_fvec);
  ui->IDC_VIEW_CONE_ANGLE_CHECK->setChecked(wb->flags.use_custom_fov);
  ui->IDC_MAX_DISTANCE_CHECK->setChecked(wb->flags.use_custom_max_dist);

  const int anim_type = (wb->flags.anim_local ? WBF_ANIM_LOCAL : 0) | (wb->flags.anim_full ? WBF_ANIM_FULL : 0);
  ui->IDC_WB_NO_ANIM_RADIO->setChecked(anim_type == 0);
  ui->IDC_WB_LOCAL_ANIM_RADIO->setChecked(anim_type == WBF_ANIM_LOCAL);
  ui->IDC_WB_MODEL_ANIM_RADIO->setChecked(anim_type == WBF_ANIM_FULL);

  updateDialog();
}

void RobotEditWeaponsDialog::updateDialog() {
  otype_wb_info *wb = m_wb;
  int num_gps = 0;
  if (m_pm != nullptr && m_pm->num_wbs > 0)
    num_gps = m_pm->poly_wb[0].num_gps;

  const int num_masks = ui->IDC_GB_NUM_MASKS_EDIT->text().toInt();

  ui->IDC_GP_MASK_0_0->setEnabled(0 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_0_1->setEnabled(0 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_0_2->setEnabled(0 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_0_3->setEnabled(0 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_0_4->setEnabled(0 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_0_5->setEnabled(0 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_0_6->setEnabled(0 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_0_7->setEnabled(0 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_0_EDIT->setEnabled(0 < num_masks);
  ui->IDC_GP_MASK_1_0->setEnabled(1 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_1_1->setEnabled(1 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_1_2->setEnabled(1 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_1_3->setEnabled(1 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_1_4->setEnabled(1 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_1_5->setEnabled(1 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_1_6->setEnabled(1 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_1_7->setEnabled(1 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_1_EDIT->setEnabled(1 < num_masks);
  ui->IDC_GP_MASK_2_0->setEnabled(2 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_2_1->setEnabled(2 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_2_2->setEnabled(2 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_2_3->setEnabled(2 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_2_4->setEnabled(2 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_2_5->setEnabled(2 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_2_6->setEnabled(2 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_2_7->setEnabled(2 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_2_EDIT->setEnabled(2 < num_masks);
  ui->IDC_GP_MASK_3_0->setEnabled(3 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_3_1->setEnabled(3 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_3_2->setEnabled(3 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_3_3->setEnabled(3 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_3_4->setEnabled(3 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_3_5->setEnabled(3 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_3_6->setEnabled(3 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_3_7->setEnabled(3 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_3_EDIT->setEnabled(3 < num_masks);
  ui->IDC_GP_MASK_4_0->setEnabled(4 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_4_1->setEnabled(4 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_4_2->setEnabled(4 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_4_3->setEnabled(4 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_4_4->setEnabled(4 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_4_5->setEnabled(4 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_4_6->setEnabled(4 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_4_7->setEnabled(4 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_4_EDIT->setEnabled(4 < num_masks);
  ui->IDC_GP_MASK_5_0->setEnabled(5 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_5_1->setEnabled(5 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_5_2->setEnabled(5 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_5_3->setEnabled(5 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_5_4->setEnabled(5 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_5_5->setEnabled(5 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_5_6->setEnabled(5 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_5_7->setEnabled(5 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_5_EDIT->setEnabled(5 < num_masks);
  ui->IDC_GP_MASK_6_0->setEnabled(6 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_6_1->setEnabled(6 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_6_2->setEnabled(6 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_6_3->setEnabled(6 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_6_4->setEnabled(6 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_6_5->setEnabled(6 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_6_6->setEnabled(6 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_6_7->setEnabled(6 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_6_EDIT->setEnabled(6 < num_masks);
  ui->IDC_GP_MASK_7_0->setEnabled(7 < num_masks && 0 < num_gps);
  ui->IDC_GP_MASK_7_1->setEnabled(7 < num_masks && 1 < num_gps);
  ui->IDC_GP_MASK_7_2->setEnabled(7 < num_masks && 2 < num_gps);
  ui->IDC_GP_MASK_7_3->setEnabled(7 < num_masks && 3 < num_gps);
  ui->IDC_GP_MASK_7_4->setEnabled(7 < num_masks && 4 < num_gps);
  ui->IDC_GP_MASK_7_5->setEnabled(7 < num_masks && 5 < num_gps);
  ui->IDC_GP_MASK_7_6->setEnabled(7 < num_masks && 6 < num_gps);
  ui->IDC_GP_MASK_7_7->setEnabled(7 < num_masks && 7 < num_gps);
  ui->IDC_GB_MASK_LATENCY_7_EDIT->setEnabled(7 < num_masks);

  {
    const bool has_gp = 0 < num_gps;
    ui->IDC_BARREL_0_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_0_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_0->setEnabled(has_gp);
  }
  {
    const bool has_gp = 1 < num_gps;
    ui->IDC_BARREL_1_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_1_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_1->setEnabled(has_gp);
  }
  {
    const bool has_gp = 2 < num_gps;
    ui->IDC_BARREL_2_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_2_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_2->setEnabled(has_gp);
  }
  {
    const bool has_gp = 3 < num_gps;
    ui->IDC_BARREL_3_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_3_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_3->setEnabled(has_gp);
  }
  {
    const bool has_gp = 4 < num_gps;
    ui->IDC_BARREL_4_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_4_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_4->setEnabled(has_gp);
  }
  {
    const bool has_gp = 5 < num_gps;
    ui->IDC_BARREL_5_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_5_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_5->setEnabled(has_gp);
  }
  {
    const bool has_gp = 6 < num_gps;
    ui->IDC_BARREL_6_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_6_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_6->setEnabled(has_gp);
  }
  {
    const bool has_gp = 7 < num_gps;
    ui->IDC_BARREL_7_COMBO->setEnabled(has_gp);
    ui->IDC_BARREL_7_RADIO->setEnabled(has_gp);
    ui->IDC_GP_MASK_Q_7->setEnabled(has_gp);
  }

  const bool custom_fov = ui->IDC_VIEW_CONE_ANGLE_CHECK->isChecked();
  const bool custom_dist = ui->IDC_MAX_DISTANCE_CHECK->isChecked();
  ui->IDC_VIEW_CONE_ANGLE_EDIT->setEnabled(custom_fov);
  ui->IDC_XZ_PLANE_ANGLE_EDIT->setEnabled(custom_fov);
  ui->IDC_MAX_DISTANCE_EDIT->setEnabled(custom_dist);

  // Turret info (static read-only display).
  if (m_pm != nullptr && m_pm->num_wbs > 0 && m_pm->poly_wb[0].num_turrets > 0) {
    const int turret = m_pm->poly_wb[0].turret_index[0];
    ui->IDC_TURRET_SOBJ_STATIC->setText(QString::number(turret));
    ui->IDC_TURRET_FOV_STATIC->setText(QString::number(m_pm->submodel[turret].fov * 720.0));
    ui->IDC_TURRET_REACTION_TIME_STATIC->setText(QString::number(m_pm->submodel[turret].think_interval, 'f', 2));
    ui->IDC_TURRET_SPR_STATIC->setText(QString::number(1.0f / m_pm->submodel[turret].rps, 'f', 2));
  }

  (void)wb;
}

void RobotEditWeaponsDialog::getData() {
  otype_wb_info *wb = m_wb;

  wb->num_masks = ui->IDC_GB_NUM_MASKS_EDIT->text().toInt();
  if (wb->num_masks < 1)
    wb->num_masks = 1;
  if (wb->num_masks > 8)
    wb->num_masks = 8;

  wb->gp_fire_wait[0] = ui->IDC_GB_MASK_LATENCY_0_EDIT->text().toFloat();
  wb->anim_start_frame[0] = ui->IDC_WB_FIRE0_START_TICK->text().toFloat();
  wb->anim_fire_frame[0] = ui->IDC_WB_FIRE0_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[0] = ui->IDC_WB_FIRE0_END_TICK->text().toFloat();
  wb->anim_time[0] = ui->IDC_WB_FIRE0_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[0] = soundComboSelected(ui->IDC_WB_FIRE0_SOUND_PULLDOWN);
  wb->gp_fire_masks[0] = 0;
  if (ui->IDC_GP_MASK_0_0->isChecked())
    wb->gp_fire_masks[0] |= (1 << 0);
  if (ui->IDC_GP_MASK_0_1->isChecked())
    wb->gp_fire_masks[0] |= (1 << 1);
  if (ui->IDC_GP_MASK_0_2->isChecked())
    wb->gp_fire_masks[0] |= (1 << 2);
  if (ui->IDC_GP_MASK_0_3->isChecked())
    wb->gp_fire_masks[0] |= (1 << 3);
  if (ui->IDC_GP_MASK_0_4->isChecked())
    wb->gp_fire_masks[0] |= (1 << 4);
  if (ui->IDC_GP_MASK_0_5->isChecked())
    wb->gp_fire_masks[0] |= (1 << 5);
  if (ui->IDC_GP_MASK_0_6->isChecked())
    wb->gp_fire_masks[0] |= (1 << 6);
  if (ui->IDC_GP_MASK_0_7->isChecked())
    wb->gp_fire_masks[0] |= (1 << 7);
  wb->gp_fire_wait[1] = ui->IDC_GB_MASK_LATENCY_1_EDIT->text().toFloat();
  wb->anim_start_frame[1] = ui->IDC_WB_FIRE1_START_TICK->text().toFloat();
  wb->anim_fire_frame[1] = ui->IDC_WB_FIRE1_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[1] = ui->IDC_WB_FIRE1_END_TICK->text().toFloat();
  wb->anim_time[1] = ui->IDC_WB_FIRE1_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[1] = soundComboSelected(ui->IDC_WB_FIRE1_SOUND_PULLDOWN);
  wb->gp_fire_masks[1] = 0;
  if (ui->IDC_GP_MASK_1_0->isChecked())
    wb->gp_fire_masks[1] |= (1 << 0);
  if (ui->IDC_GP_MASK_1_1->isChecked())
    wb->gp_fire_masks[1] |= (1 << 1);
  if (ui->IDC_GP_MASK_1_2->isChecked())
    wb->gp_fire_masks[1] |= (1 << 2);
  if (ui->IDC_GP_MASK_1_3->isChecked())
    wb->gp_fire_masks[1] |= (1 << 3);
  if (ui->IDC_GP_MASK_1_4->isChecked())
    wb->gp_fire_masks[1] |= (1 << 4);
  if (ui->IDC_GP_MASK_1_5->isChecked())
    wb->gp_fire_masks[1] |= (1 << 5);
  if (ui->IDC_GP_MASK_1_6->isChecked())
    wb->gp_fire_masks[1] |= (1 << 6);
  if (ui->IDC_GP_MASK_1_7->isChecked())
    wb->gp_fire_masks[1] |= (1 << 7);
  wb->gp_fire_wait[2] = ui->IDC_GB_MASK_LATENCY_2_EDIT->text().toFloat();
  wb->anim_start_frame[2] = ui->IDC_WB_FIRE2_START_TICK->text().toFloat();
  wb->anim_fire_frame[2] = ui->IDC_WB_FIRE2_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[2] = ui->IDC_WB_FIRE2_END_TICK->text().toFloat();
  wb->anim_time[2] = ui->IDC_WB_FIRE2_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[2] = soundComboSelected(ui->IDC_WB_FIRE2_SOUND_PULLDOWN);
  wb->gp_fire_masks[2] = 0;
  if (ui->IDC_GP_MASK_2_0->isChecked())
    wb->gp_fire_masks[2] |= (1 << 0);
  if (ui->IDC_GP_MASK_2_1->isChecked())
    wb->gp_fire_masks[2] |= (1 << 1);
  if (ui->IDC_GP_MASK_2_2->isChecked())
    wb->gp_fire_masks[2] |= (1 << 2);
  if (ui->IDC_GP_MASK_2_3->isChecked())
    wb->gp_fire_masks[2] |= (1 << 3);
  if (ui->IDC_GP_MASK_2_4->isChecked())
    wb->gp_fire_masks[2] |= (1 << 4);
  if (ui->IDC_GP_MASK_2_5->isChecked())
    wb->gp_fire_masks[2] |= (1 << 5);
  if (ui->IDC_GP_MASK_2_6->isChecked())
    wb->gp_fire_masks[2] |= (1 << 6);
  if (ui->IDC_GP_MASK_2_7->isChecked())
    wb->gp_fire_masks[2] |= (1 << 7);
  wb->gp_fire_wait[3] = ui->IDC_GB_MASK_LATENCY_3_EDIT->text().toFloat();
  wb->anim_start_frame[3] = ui->IDC_WB_FIRE3_START_TICK->text().toFloat();
  wb->anim_fire_frame[3] = ui->IDC_WB_FIRE3_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[3] = ui->IDC_WB_FIRE3_END_TICK->text().toFloat();
  wb->anim_time[3] = ui->IDC_WB_FIRE3_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[3] = soundComboSelected(ui->IDC_WB_FIRE3_SOUND_PULLDOWN);
  wb->gp_fire_masks[3] = 0;
  if (ui->IDC_GP_MASK_3_0->isChecked())
    wb->gp_fire_masks[3] |= (1 << 0);
  if (ui->IDC_GP_MASK_3_1->isChecked())
    wb->gp_fire_masks[3] |= (1 << 1);
  if (ui->IDC_GP_MASK_3_2->isChecked())
    wb->gp_fire_masks[3] |= (1 << 2);
  if (ui->IDC_GP_MASK_3_3->isChecked())
    wb->gp_fire_masks[3] |= (1 << 3);
  if (ui->IDC_GP_MASK_3_4->isChecked())
    wb->gp_fire_masks[3] |= (1 << 4);
  if (ui->IDC_GP_MASK_3_5->isChecked())
    wb->gp_fire_masks[3] |= (1 << 5);
  if (ui->IDC_GP_MASK_3_6->isChecked())
    wb->gp_fire_masks[3] |= (1 << 6);
  if (ui->IDC_GP_MASK_3_7->isChecked())
    wb->gp_fire_masks[3] |= (1 << 7);
  wb->gp_fire_wait[4] = ui->IDC_GB_MASK_LATENCY_4_EDIT->text().toFloat();
  wb->anim_start_frame[4] = ui->IDC_WB_FIRE4_START_TICK->text().toFloat();
  wb->anim_fire_frame[4] = ui->IDC_WB_FIRE4_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[4] = ui->IDC_WB_FIRE4_END_TICK->text().toFloat();
  wb->anim_time[4] = ui->IDC_WB_FIRE4_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[4] = soundComboSelected(ui->IDC_WB_FIRE4_SOUND_PULLDOWN);
  wb->gp_fire_masks[4] = 0;
  if (ui->IDC_GP_MASK_4_0->isChecked())
    wb->gp_fire_masks[4] |= (1 << 0);
  if (ui->IDC_GP_MASK_4_1->isChecked())
    wb->gp_fire_masks[4] |= (1 << 1);
  if (ui->IDC_GP_MASK_4_2->isChecked())
    wb->gp_fire_masks[4] |= (1 << 2);
  if (ui->IDC_GP_MASK_4_3->isChecked())
    wb->gp_fire_masks[4] |= (1 << 3);
  if (ui->IDC_GP_MASK_4_4->isChecked())
    wb->gp_fire_masks[4] |= (1 << 4);
  if (ui->IDC_GP_MASK_4_5->isChecked())
    wb->gp_fire_masks[4] |= (1 << 5);
  if (ui->IDC_GP_MASK_4_6->isChecked())
    wb->gp_fire_masks[4] |= (1 << 6);
  if (ui->IDC_GP_MASK_4_7->isChecked())
    wb->gp_fire_masks[4] |= (1 << 7);
  wb->gp_fire_wait[5] = ui->IDC_GB_MASK_LATENCY_5_EDIT->text().toFloat();
  wb->anim_start_frame[5] = ui->IDC_WB_FIRE5_START_TICK->text().toFloat();
  wb->anim_fire_frame[5] = ui->IDC_WB_FIRE5_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[5] = ui->IDC_WB_FIRE5_END_TICK->text().toFloat();
  wb->anim_time[5] = ui->IDC_WB_FIRE5_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[5] = soundComboSelected(ui->IDC_WB_FIRE5_SOUND_PULLDOWN);
  wb->gp_fire_masks[5] = 0;
  if (ui->IDC_GP_MASK_5_0->isChecked())
    wb->gp_fire_masks[5] |= (1 << 0);
  if (ui->IDC_GP_MASK_5_1->isChecked())
    wb->gp_fire_masks[5] |= (1 << 1);
  if (ui->IDC_GP_MASK_5_2->isChecked())
    wb->gp_fire_masks[5] |= (1 << 2);
  if (ui->IDC_GP_MASK_5_3->isChecked())
    wb->gp_fire_masks[5] |= (1 << 3);
  if (ui->IDC_GP_MASK_5_4->isChecked())
    wb->gp_fire_masks[5] |= (1 << 4);
  if (ui->IDC_GP_MASK_5_5->isChecked())
    wb->gp_fire_masks[5] |= (1 << 5);
  if (ui->IDC_GP_MASK_5_6->isChecked())
    wb->gp_fire_masks[5] |= (1 << 6);
  if (ui->IDC_GP_MASK_5_7->isChecked())
    wb->gp_fire_masks[5] |= (1 << 7);
  wb->gp_fire_wait[6] = ui->IDC_GB_MASK_LATENCY_6_EDIT->text().toFloat();
  wb->anim_start_frame[6] = ui->IDC_WB_FIRE6_START_TICK->text().toFloat();
  wb->anim_fire_frame[6] = ui->IDC_WB_FIRE6_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[6] = ui->IDC_WB_FIRE6_END_TICK->text().toFloat();
  wb->anim_time[6] = ui->IDC_WB_FIRE6_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[6] = soundComboSelected(ui->IDC_WB_FIRE6_SOUND_PULLDOWN);
  wb->gp_fire_masks[6] = 0;
  if (ui->IDC_GP_MASK_6_0->isChecked())
    wb->gp_fire_masks[6] |= (1 << 0);
  if (ui->IDC_GP_MASK_6_1->isChecked())
    wb->gp_fire_masks[6] |= (1 << 1);
  if (ui->IDC_GP_MASK_6_2->isChecked())
    wb->gp_fire_masks[6] |= (1 << 2);
  if (ui->IDC_GP_MASK_6_3->isChecked())
    wb->gp_fire_masks[6] |= (1 << 3);
  if (ui->IDC_GP_MASK_6_4->isChecked())
    wb->gp_fire_masks[6] |= (1 << 4);
  if (ui->IDC_GP_MASK_6_5->isChecked())
    wb->gp_fire_masks[6] |= (1 << 5);
  if (ui->IDC_GP_MASK_6_6->isChecked())
    wb->gp_fire_masks[6] |= (1 << 6);
  if (ui->IDC_GP_MASK_6_7->isChecked())
    wb->gp_fire_masks[6] |= (1 << 7);
  wb->gp_fire_wait[7] = ui->IDC_GB_MASK_LATENCY_7_EDIT->text().toFloat();
  wb->anim_start_frame[7] = ui->IDC_WB_FIRE7_START_TICK->text().toFloat();
  wb->anim_fire_frame[7] = ui->IDC_WB_FIRE7_FIRE_TICK->text().toFloat();
  wb->anim_end_frame[7] = ui->IDC_WB_FIRE7_END_TICK->text().toFloat();
  wb->anim_time[7] = ui->IDC_WB_FIRE7_CYCLE_TIME->text().toFloat();
  wb->fm_fire_sound_index[7] = soundComboSelected(ui->IDC_WB_FIRE7_SOUND_PULLDOWN);
  wb->gp_fire_masks[7] = 0;
  if (ui->IDC_GP_MASK_7_0->isChecked())
    wb->gp_fire_masks[7] |= (1 << 0);
  if (ui->IDC_GP_MASK_7_1->isChecked())
    wb->gp_fire_masks[7] |= (1 << 1);
  if (ui->IDC_GP_MASK_7_2->isChecked())
    wb->gp_fire_masks[7] |= (1 << 2);
  if (ui->IDC_GP_MASK_7_3->isChecked())
    wb->gp_fire_masks[7] |= (1 << 3);
  if (ui->IDC_GP_MASK_7_4->isChecked())
    wb->gp_fire_masks[7] |= (1 << 4);
  if (ui->IDC_GP_MASK_7_5->isChecked())
    wb->gp_fire_masks[7] |= (1 << 5);
  if (ui->IDC_GP_MASK_7_6->isChecked())
    wb->gp_fire_masks[7] |= (1 << 6);
  if (ui->IDC_GP_MASK_7_7->isChecked())
    wb->gp_fire_masks[7] |= (1 << 7);

  wb->gp_quad_fire_mask = 0;
  if (ui->IDC_GP_MASK_Q_0->isChecked())
    wb->gp_quad_fire_mask |= (1 << 0);
  wb->gp_weapon_index[0] = ui->IDC_BARREL_0_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_1->isChecked())
    wb->gp_quad_fire_mask |= (1 << 1);
  wb->gp_weapon_index[1] = ui->IDC_BARREL_1_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_2->isChecked())
    wb->gp_quad_fire_mask |= (1 << 2);
  wb->gp_weapon_index[2] = ui->IDC_BARREL_2_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_3->isChecked())
    wb->gp_quad_fire_mask |= (1 << 3);
  wb->gp_weapon_index[3] = ui->IDC_BARREL_3_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_4->isChecked())
    wb->gp_quad_fire_mask |= (1 << 4);
  wb->gp_weapon_index[4] = ui->IDC_BARREL_4_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_5->isChecked())
    wb->gp_quad_fire_mask |= (1 << 5);
  wb->gp_weapon_index[5] = ui->IDC_BARREL_5_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_6->isChecked())
    wb->gp_quad_fire_mask |= (1 << 6);
  wb->gp_weapon_index[6] = ui->IDC_BARREL_6_COMBO->currentData().toInt();
  if (ui->IDC_GP_MASK_Q_7->isChecked())
    wb->gp_quad_fire_mask |= (1 << 7);
  wb->gp_weapon_index[7] = ui->IDC_BARREL_7_COMBO->currentData().toInt();

  if (ui->IDC_BARREL_0_RADIO->isChecked())
    wb->aiming_gp_index = 0;
  if (ui->IDC_BARREL_1_RADIO->isChecked())
    wb->aiming_gp_index = 1;
  if (ui->IDC_BARREL_2_RADIO->isChecked())
    wb->aiming_gp_index = 2;
  if (ui->IDC_BARREL_3_RADIO->isChecked())
    wb->aiming_gp_index = 3;
  if (ui->IDC_BARREL_4_RADIO->isChecked())
    wb->aiming_gp_index = 4;
  if (ui->IDC_BARREL_5_RADIO->isChecked())
    wb->aiming_gp_index = 5;
  if (ui->IDC_BARREL_6_RADIO->isChecked())
    wb->aiming_gp_index = 6;
  if (ui->IDC_BARREL_7_RADIO->isChecked())
    wb->aiming_gp_index = 7;

  wb->aiming_3d_dot = cos(ui->IDC_VIEW_CONE_ANGLE_EDIT->text().toFloat() * PI / 360.0);
  wb->aiming_3d_dist = ui->IDC_MAX_DISTANCE_EDIT->text().toFloat();
  wb->aiming_XZ_dot = cos(ui->IDC_XZ_PLANE_ANGLE_EDIT->text().toFloat() * PI / 360.0);

  wb->energy_usage = ui->IDC_ENERGY_USAGE_EDIT->text().toFloat();
  wb->ammo_usage = ui->IDC_AMMO_USAGE_EDIT->text().toFloat();

  wb->flags.anim_local = false;
  wb->flags.anim_full = false;
  if (ui->IDC_WB_LOCAL_ANIM_RADIO->isChecked())
    wb->flags.anim_local = true;
  else if (ui->IDC_WB_MODEL_ANIM_RADIO->isChecked())
    wb->flags.anim_full = true;

  wb->flags.spray = ui->IDC_SPRAY_CHECK->isChecked();
  wb->flags.guided = ui->IDC_GUIDED_CHECK->isChecked();
  wb->flags.on_off = ui->IDC_ONOFF_CHECK->isChecked();
  wb->flags.random_fire_order = ui->IDC_WB_RANDOM_CHECK->isChecked();
  wb->flags.user_timeout = ui->IDC_USER_TIMEOUT_CHECK->isChecked();
  wb->flags.fire_fvec = ui->IDC_FORCE_TO_FVEC_CHECK->isChecked();
  wb->flags.fire_target = ui->IDC_FORCE_TO_TARGET_CHECK->isChecked();
  wb->flags.aim_fvec = ui->IDC_WBAIMFORWARD_CHECK->isChecked();
  wb->flags.use_custom_fov = ui->IDC_VIEW_CONE_ANGLE_CHECK->isChecked();
  wb->flags.use_custom_max_dist = ui->IDC_MAX_DISTANCE_CHECK->isChecked();
}

void editRobotWeapons(otype_wb_info *wb, poly_model *pm, QWidget *parent) {
  RobotEditWeaponsDialog dlg(wb, pm, parent);
  dlg.exec();
}