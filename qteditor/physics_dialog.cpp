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

#include "physics_dialog.h"
#include "ui_physics.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <cmath>

#include "vecmat.h"


namespace {
physics_info Paste_data{};
bool Paste_data_used = false;

constexpr int kNoGravity =0;
constexpr int kForwardGravity =1;
constexpr int kReverseGravity =2;

float editFloat(QWidget *w) {
  auto *edit = qobject_cast<QLineEdit *>(w);
  return edit ? edit->text().toFloat() : 0.0f;
}

void setEditFloat(QWidget *w, float value) {
  if (auto *edit = qobject_cast<QLineEdit *>(w))
    edit->setText(QString::number(value));
}
} // namespace

PhysicsDialog::PhysicsDialog(physics_info *physInfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::PhysicsDialog), m_physInfo(physInfo), m_gravityFlag(0)
{
  ui->setupUi(this);
  setPhysicsData(physInfo);

  connect(ui->IDOK, &QPushButton::clicked, this, &PhysicsDialog::onOk);

  connect(ui->IDC_PHYSICS_COPY, &QPushButton::clicked, this, &PhysicsDialog::onCopy);
  {
    QPushButton *paste = ui->IDC_PHYSICS_PASTE;
    connect(paste, &QPushButton::clicked, this, &PhysicsDialog::onPaste);
    paste->setEnabled(Paste_data_used);
  }
  if (auto *cb = ui->IDC_PHYSICS_WIGGLE_CHECK)
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onWiggleCheck);
  if (auto *rb = ui->IDC_PHYSICS_GRAVITY_RADIO)
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onGravityRadio);
  if (auto *rb = ui->IDC_PHYSICS_NO_GRAVITY_RADIO)
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onNoGravityRadio);
  if (auto *rb = ui->IDC_PHYSICS_REVERSE_GRAVITY_RADIO)
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onReverseGravityRadio);
  if (auto *cb = ui->IDC_TURN_ROLL_CHECK)
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onTurnRollCheck);
  if (auto *cb = ui->IDC_PHYSICS_BOUNCY_CHECK)
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onBouncyCheck);
  if (auto *cb = ui->IDC_PHYSICS_THRUSTS_CHECK)
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onThrustsCheck);

  connect(ui->IDC_PHYSICS_DRAG_EDIT, &QLineEdit::editingFinished, this, &PhysicsDialog::onTerminalEditChanged);
  connect(ui->IDC_PHYSICS_ROTDRAG_EDIT, &QLineEdit::editingFinished, this, &PhysicsDialog::onTerminalEditChanged);
  connect(ui->IDC_PHYSICS_MAXTHRUST_EDIT, &QLineEdit::editingFinished, this, &PhysicsDialog::onTerminalEditChanged);
  connect(ui->IDC_PHYSICS_ROTTHRUST_EDIT, &QLineEdit::editingFinished, this, &PhysicsDialog::onTerminalEditChanged);
}

PhysicsDialog::~PhysicsDialog() { delete ui; }

void PhysicsDialog::enableDisableFullPhysics() {
  ui->IDC_PHYSICS_DRAG_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_ROTDRAG_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_MAXTHRUST_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_ROTTHRUST_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_MAXTURNROLLRATE_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_TURNROLLRATIO_EDIT->setEnabled(true);
  ui->IDC_PHYSICS_DRAG_TEXT->setEnabled(true);
  ui->IDC_PHYSICS_ROTDRAG_TEXT->setEnabled(true);
  ui->IDC_PHYSICS_MAXTHRUST_TEXT->setEnabled(true);
  ui->IDC_PHYSICS_ROTTHRUST_TEXT->setEnabled(true);
  ui->IDC_PHYSICS_MAXTURNROLLRATE_TEXT->setEnabled(true);
  ui->IDC_PHYSICS_TURNROLLRATIO_TEXT->setEnabled(true);
}

void PhysicsDialog::enableDisableWiggle() {
  const bool enabled = ui->IDC_PHYSICS_WIGGLE_CHECK->isChecked();
  ui->IDC_PHYSICS_WIGGLESIZE_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_WIGGLEFREQ_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_WIGGLESIZE_TEXT->setEnabled(enabled);
  ui->IDC_PHYSICS_WIGGLES_PER_SEC_TEXT->setEnabled(enabled);
}

void PhysicsDialog::enableDisableBounce() {
  const bool enabled = ui->IDC_PHYSICS_BOUNCY_CHECK->isChecked();
  ui->IDC_PHYSICS_MAX_BOUNCES_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_PERCENT_LOSS_EDIT->setEnabled(enabled);
  ui->IDC_MAX_BOUNCES_TEXT->setEnabled(enabled);
  ui->IDC_PERCENT_LOSS_TEXT->setEnabled(enabled);
}

void PhysicsDialog::enableDisableTurnRoll() {
  const bool enabled = ui->IDC_TURN_ROLL_CHECK->isChecked();
  ui->IDC_PHYSICS_MAXTURNROLLRATE_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_TURNROLLRATIO_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_MAXTURNROLLRATE_TEXT->setEnabled(enabled);
  ui->IDC_PHYSICS_TURNROLLRATIO_TEXT->setEnabled(enabled);
}

void PhysicsDialog::enableDisableThrust() {
  const bool enabled = ui->IDC_PHYSICS_THRUSTS_CHECK->isChecked();
  ui->IDC_PHYSICS_MAXTHRUST_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_ROTTHRUST_EDIT->setEnabled(enabled);
  ui->IDC_PHYSICS_MAXTHRUST_TEXT->setEnabled(enabled);
  ui->IDC_PHYSICS_ROTTHRUST_TEXT->setEnabled(enabled);
}

void PhysicsDialog::updateTerminalText() {
  const float drag = editFloat(ui->IDC_PHYSICS_DRAG_EDIT);
  const float maxThrust = editFloat(ui->IDC_PHYSICS_MAXTHRUST_EDIT);
  const float rotDrag = editFloat(ui->IDC_PHYSICS_ROTDRAG_EDIT);
  const float rotThrust = editFloat(ui->IDC_PHYSICS_ROTTHRUST_EDIT);

  if (auto *label = ui->IDC_PHYSICS_TERMINAL_VELOCITY_STATIC) {
    label->setText(drag > 0.0f
                       ? QString("Terminal Velocity(thrust/drag): %1").arg(maxThrust / drag)
                       : QString("Terminal Velocity(thrust/drag): 0.0"));
  }
  if (auto *label = ui->IDC_PHYSICS_TERMINAL_ROT_VELOCITY_STATIC) {
    label->setText(rotDrag > 0.0f
                       ? QString("Terminal Rot. Vel.(rot. thrust/ rot. drag): %1")
                             .arg(rotThrust / rotDrag)
                       : QString("Terminal Rot. Vel.(rot. thrust/ rot. drag): 0.0"));
  }
}

void PhysicsDialog::setPhysicsData(const physics_info *p) {
  setEditFloat(ui->IDC_PHYSICS_MASS_EDIT, p->mass);
  setEditFloat(ui->IDC_PHYSICS_DRAG_EDIT, p->drag);
  setEditFloat(ui->IDC_PHYSICS_ROTDRAG_EDIT, p->rotdrag);
  setEditFloat(ui->IDC_PHYSICS_MAXTHRUST_EDIT, p->full_thrust);
  setEditFloat(ui->IDC_PHYSICS_ROTTHRUST_EDIT, p->full_rotthrust);
  setEditFloat(ui->IDC_PHYSICS_MAXTURNROLLRATE_EDIT, p->max_turnroll_rate);
  setEditFloat(ui->IDC_PHYSICS_TURNROLLRATIO_EDIT, p->turnroll_ratio);
  setEditFloat(ui->IDC_PHYSICS_WIGGLESIZE_EDIT, p->wiggle_amplitude);
  setEditFloat(ui->IDC_PHYSICS_WIGGLEFREQ_EDIT, p->wiggles_per_sec);
  setEditFloat(ui->IDC_PHYSICS_INIT_VELOCITY_EDIT, p->velocity.z());
  setEditFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_X_EDIT, p->rotvel.x());
  setEditFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_Y_EDIT, p->rotvel.y());
  setEditFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_Z_EDIT, p->rotvel.z());
  setEditFloat(ui->IDC_PHYSICS_MAX_BOUNCES_EDIT, p->num_bounces);
  setEditFloat(ui->IDC_PHYSICS_PERCENT_LOSS_EDIT,
               (scalar)100.0 - (p->coeff_restitution * (scalar)100.0));

  ui->IDC_PHYSICS_STICKY_CHECK->setChecked(p->flags.stick);
  ui->IDC_PHYSICS_BOUNCY_CHECK->setChecked(p->flags.bounce);
  ui->IDC_PHYSICS_THRUSTS_CHECK->setChecked(p->flags.uses_thrust);
  ui->IDC_PHYSICS_FIXED_VELOCITY_CHECK->setChecked(p->flags.fixed_velocity);
  ui->IDC_PHYSICS_FIXED_ROTATE_CHECK->setChecked(p->flags.fixed_rot_velocity);
  ui->IDC_NO_COLLIDE_CHECK->setChecked(p->flags.no_collide);
  ui->IDC_TURN_ROLL_CHECK->setChecked(p->flags.turnroll);
  ui->IDC_PHYSICS_AUTO_LEVELING_CHECK->setChecked(p->flags.leveling);
  ui->IDC_PHYSICS_POINT_WALL_CHECK->setChecked(p->flags.point_collide_walls);
  ui->IDC_PHYSICS_IGNORE_ROBOTS_CHECK->setChecked(p->flags.no_robot_collisions);
  ui->IDC_PHYSICS_IGNORE_SAME_CHECK->setChecked(p->flags.no_same_collisions);
  ui->IDC_PHYSICS_IGNORE_DOOR_CHECK->setChecked(p->flags.no_door_collisions);
  ui->IDC_PHYSICS_IGNORE_CONCUSSIVE_FORCES->setChecked(p->flags.ignore_concussive_forces);
  ui->IDC_L_X_CHECK->setChecked(p->flags.lock_x);
  ui->IDC_L_Y_CHECK->setChecked(p->flags.lock_y);
  ui->IDC_L_Z_CHECK->setChecked(p->flags.lock_z);
  ui->IDC_L_P_CHECK->setChecked(p->flags.lock_p);
  ui->IDC_L_B_CHECK->setChecked(p->flags.lock_b);
  ui->IDC_L_H_CHECK->setChecked(p->flags.lock_h);
  ui->IDC_NO_BIG_SPHERE_CHECK->setChecked(p->flags.never_use_big_sphere);
  ui->IDC_PHYSICS_WIGGLE_CHECK->setChecked(p->flags.wiggle);
  ui->IDC_PHYSICS_MAGNETISM_CHECK->setChecked(p->flags.ignore_own_conc_forces);
  ui->IDC_PHYSICS_WIND_CHECK->setChecked(p->flags.wind);
  ui->IDC_PHYSICS_PERSISTENT_CHECK->setChecked(p->flags.persistent);

  m_gravityFlag = p->flags.gravity ? kForwardGravity :(p->flags.reverse_gravity ? kReverseGravity : kNoGravity);
  if (m_gravityFlag == kForwardGravity)
    ui->IDC_PHYSICS_GRAVITY_RADIO->setChecked(true);
  else if (m_gravityFlag == kReverseGravity)
    ui->IDC_PHYSICS_REVERSE_GRAVITY_RADIO->setChecked(true);
  else
    ui->IDC_PHYSICS_NO_GRAVITY_RADIO->setChecked(true);

  setEditFloat(ui->IDC_PHYSICS_HIT_DIE_ANGLE,
               (p->hit_die_dot == -1) ? 0.0f
                                      : (float)(asin(p->hit_die_dot) * ((scalar)180.0 / (scalar)PI)));
  ui->IDC_PHYSICS_HIT_DIE_ANGLE->setEnabled(p->hit_die_dot != -1);
}

void PhysicsDialog::getPhysicsData(physics_info *p) const {
  p->mass = editFloat(ui->IDC_PHYSICS_MASS_EDIT);
  p->drag = editFloat(ui->IDC_PHYSICS_DRAG_EDIT);
  p->rotdrag = editFloat(ui->IDC_PHYSICS_ROTDRAG_EDIT);
  p->full_thrust = editFloat(ui->IDC_PHYSICS_MAXTHRUST_EDIT);
  p->full_rotthrust = editFloat(ui->IDC_PHYSICS_ROTTHRUST_EDIT);
  p->max_turnroll_rate = editFloat(ui->IDC_PHYSICS_MAXTURNROLLRATE_EDIT);
  p->turnroll_ratio = editFloat(ui->IDC_PHYSICS_TURNROLLRATIO_EDIT);
  p->wiggle_amplitude = editFloat(ui->IDC_PHYSICS_WIGGLESIZE_EDIT);
  p->wiggles_per_sec = editFloat(ui->IDC_PHYSICS_WIGGLEFREQ_EDIT);
  p->velocity.z() = editFloat(ui->IDC_PHYSICS_INIT_VELOCITY_EDIT);
  p->rotvel.x() = editFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_X_EDIT);
  p->rotvel.y() = editFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_Y_EDIT);
  p->rotvel.z() = editFloat(ui->IDC_PHYSICS_INIT_ROT_VELOCITY_Z_EDIT);
  p->num_bounces = (int)editFloat(ui->IDC_PHYSICS_MAX_BOUNCES_EDIT);
  p->coeff_restitution = ((scalar)100.0 - editFloat(ui->IDC_PHYSICS_PERCENT_LOSS_EDIT)) / (scalar)100.0;

  
  p->flags.stick = ui->IDC_PHYSICS_STICKY_CHECK->isChecked();
  p->flags.bounce = ui->IDC_PHYSICS_BOUNCY_CHECK->isChecked();
  p->flags.uses_thrust = ui->IDC_PHYSICS_THRUSTS_CHECK->isChecked();
  p->flags.fixed_velocity = ui->IDC_PHYSICS_FIXED_VELOCITY_CHECK->isChecked();
  p->flags.fixed_rot_velocity = ui->IDC_PHYSICS_FIXED_ROTATE_CHECK->isChecked();
  p->flags.no_collide = ui->IDC_NO_COLLIDE_CHECK->isChecked();
  p->flags.turnroll = ui->IDC_TURN_ROLL_CHECK->isChecked();
  p->flags.leveling = ui->IDC_PHYSICS_AUTO_LEVELING_CHECK->isChecked();
  p->flags.point_collide_walls = ui->IDC_PHYSICS_POINT_WALL_CHECK->isChecked();
  p->flags.no_robot_collisions = ui->IDC_PHYSICS_IGNORE_ROBOTS_CHECK->isChecked();
  p->flags.no_same_collisions = ui->IDC_PHYSICS_IGNORE_SAME_CHECK->isChecked();
  p->flags.no_door_collisions = ui->IDC_PHYSICS_IGNORE_DOOR_CHECK->isChecked();
  p->flags.ignore_concussive_forces = ui->IDC_PHYSICS_IGNORE_CONCUSSIVE_FORCES->isChecked();
  p->flags.lock_x = ui->IDC_L_X_CHECK->isChecked();
  p->flags.lock_y = ui->IDC_L_Y_CHECK->isChecked();
  p->flags.lock_z = ui->IDC_L_Z_CHECK->isChecked();
  p->flags.lock_p = ui->IDC_L_P_CHECK->isChecked();
  p->flags.lock_b = ui->IDC_L_B_CHECK->isChecked();
  p->flags.lock_h = ui->IDC_L_H_CHECK->isChecked();
  p->flags.never_use_big_sphere = ui->IDC_NO_BIG_SPHERE_CHECK->isChecked();
  p->flags.wiggle = ui->IDC_PHYSICS_WIGGLE_CHECK->isChecked();
  p->flags.ignore_own_conc_forces = ui->IDC_PHYSICS_MAGNETISM_CHECK->isChecked();
  p->flags.wind = ui->IDC_PHYSICS_WIND_CHECK->isChecked();
  p->flags.persistent = ui->IDC_PHYSICS_PERSISTENT_CHECK->isChecked();

  if (p->hit_die_dot != -1)
    p->hit_die_dot = sin(editFloat(ui->IDC_PHYSICS_HIT_DIE_ANGLE) * (scalar)PI / (scalar)180.0);
p->flags.gravity = (m_gravityFlag == kForwardGravity);
  p->flags.reverse_gravity =(m_gravityFlag == kReverseGravity);
}

void PhysicsDialog::onWiggleCheck() { enableDisableWiggle(); }
void PhysicsDialog::onGravityRadio() { m_gravityFlag = kForwardGravity; }
void PhysicsDialog::onNoGravityRadio() { m_gravityFlag = kNoGravity; }
void PhysicsDialog::onReverseGravityRadio() { m_gravityFlag = kReverseGravity; }
void PhysicsDialog::onTurnRollCheck() { enableDisableTurnRoll(); }
void PhysicsDialog::onBouncyCheck() { enableDisableBounce(); }
void PhysicsDialog::onThrustsCheck() { enableDisableThrust(); }
void PhysicsDialog::onTerminalEditChanged() { updateTerminalText(); }

void PhysicsDialog::onCopy() {
  getPhysicsData(&Paste_data);
  Paste_data_used = true;
  ui->IDC_PHYSICS_PASTE->setEnabled(true);
}

void PhysicsDialog::onPaste() {
  if (Paste_data_used) {
    setPhysicsData(&Paste_data);
    updateTerminalText();
    enableDisableFullPhysics();
    enableDisableWiggle();
    enableDisableBounce();
    enableDisableTurnRoll();
    enableDisableThrust();
  }
}

void PhysicsDialog::onOk() {
  getPhysicsData(m_physInfo);
  accept();
}

