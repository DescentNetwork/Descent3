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

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <cmath>

#include "vecmat.h"

namespace QtEditor {

namespace {
physics_info Paste_data{};
bool Paste_data_used = false;

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
    : Dialog(":/ui/physics.ui", parent), m_physInfo(physInfo), m_gravityFlag(0) {
  setPhysicsData(physInfo);

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &PhysicsDialog::onOk);
  }
  if (QPushButton *copy = find<QPushButton>("IDC_PHYSICS_COPY"))
    connect(copy, &QPushButton::clicked, this, &PhysicsDialog::onCopy);
  if (QPushButton *paste = find<QPushButton>("IDC_PHYSICS_PASTE")) {
    connect(paste, &QPushButton::clicked, this, &PhysicsDialog::onPaste);
    paste->setEnabled(Paste_data_used);
  }
  if (auto *cb = find<QCheckBox>("IDC_PHYSICS_WIGGLE_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onWiggleCheck);
  if (auto *rb = find<QRadioButton>("IDC_PHYSICS_GRAVITY_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onGravityRadio);
  if (auto *rb = find<QRadioButton>("IDC_PHYSICS_NO_GRAVITY_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onNoGravityRadio);
  if (auto *rb = find<QRadioButton>("IDC_PHYSICS_REVERSE_GRAVITY_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &PhysicsDialog::onReverseGravityRadio);
  if (auto *cb = find<QCheckBox>("IDC_TURN_ROLL_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onTurnRollCheck);
  if (auto *cb = find<QCheckBox>("IDC_PHYSICS_BOUNCY_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onBouncyCheck);
  if (auto *cb = find<QCheckBox>("IDC_PHYSICS_THRUSTS_CHECK"))
    connect(cb, &QCheckBox::toggled, this, &PhysicsDialog::onThrustsCheck);

  const char *terminalEdits[] = {"IDC_PHYSICS_DRAG_EDIT", "IDC_PHYSICS_ROTDRAG_EDIT",
                                 "IDC_PHYSICS_MAXTHRUST_EDIT", "IDC_PHYSICS_ROTTHRUST_EDIT"};
  for (const char *name : terminalEdits) {
    if (auto *edit = find<QLineEdit>(name))
      connect(edit, &QLineEdit::editingFinished, this, &PhysicsDialog::onTerminalEditChanged);
  }
}

PhysicsDialog::~PhysicsDialog() = default;

void PhysicsDialog::setWidgetEnabled(const char *name, bool enabled) {
  if (QWidget *w = find<QWidget>(name))
    w->setEnabled(enabled);
}

void PhysicsDialog::enableDisableFullPhysics() {
  const char *names[] = {"IDC_PHYSICS_DRAG_EDIT", "IDC_PHYSICS_ROTDRAG_EDIT",
                         "IDC_PHYSICS_MAXTHRUST_EDIT", "IDC_PHYSICS_ROTTHRUST_EDIT",
                         "IDC_PHYSICS_MAXTURNROLLRATE_EDIT", "IDC_PHYSICS_TURNROLLRATIO_EDIT",
                         "IDC_PHYSICS_DRAG_TEXT", "IDC_PHYSICS_ROTDRAG_TEXT",
                         "IDC_PHYSICS_MAXTHRUST_TEXT", "IDC_PHYSICS_ROTTHRUST_TEXT",
                         "IDC_PHYSICS_MAXTURNROLLRATE_TEXT", "IDC_PHYSICS_TURNROLLRATIO_TEXT"};
  for (const char *name : names)
    setWidgetEnabled(name, true);
}

void PhysicsDialog::enableDisableWiggle() {
  const bool enabled = find<QCheckBox>("IDC_PHYSICS_WIGGLE_CHECK")->isChecked();
  setWidgetEnabled("IDC_PHYSICS_WIGGLESIZE_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_WIGGLEFREQ_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_WIGGLESIZE_TEXT", enabled);
  setWidgetEnabled("IDC_PHYSICS_WIGGLES_PER_SEC_TEXT", enabled);
}

void PhysicsDialog::enableDisableBounce() {
  const bool enabled = find<QCheckBox>("IDC_PHYSICS_BOUNCY_CHECK")->isChecked();
  setWidgetEnabled("IDC_PHYSICS_MAX_BOUNCES_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_PERCENT_LOSS_EDIT", enabled);
  setWidgetEnabled("IDC_MAX_BOUNCES_TEXT", enabled);
  setWidgetEnabled("IDC_PERCENT_LOSS_TEXT", enabled);
}

void PhysicsDialog::enableDisableTurnRoll() {
  const bool enabled = find<QCheckBox>("IDC_TURN_ROLL_CHECK")->isChecked();
  setWidgetEnabled("IDC_PHYSICS_MAXTURNROLLRATE_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_TURNROLLRATIO_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_MAXTURNROLLRATE_TEXT", enabled);
  setWidgetEnabled("IDC_PHYSICS_TURNROLLRATIO_TEXT", enabled);
}

void PhysicsDialog::enableDisableThrust() {
  const bool enabled = find<QCheckBox>("IDC_PHYSICS_THRUSTS_CHECK")->isChecked();
  setWidgetEnabled("IDC_PHYSICS_MAXTHRUST_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_ROTTHRUST_EDIT", enabled);
  setWidgetEnabled("IDC_PHYSICS_MAXTHRUST_TEXT", enabled);
  setWidgetEnabled("IDC_PHYSICS_ROTTHRUST_TEXT", enabled);
}

void PhysicsDialog::updateTerminalText() {
  const float drag = editFloat(find("IDC_PHYSICS_DRAG_EDIT"));
  const float maxThrust = editFloat(find("IDC_PHYSICS_MAXTHRUST_EDIT"));
  const float rotDrag = editFloat(find("IDC_PHYSICS_ROTDRAG_EDIT"));
  const float rotThrust = editFloat(find("IDC_PHYSICS_ROTTHRUST_EDIT"));

  if (auto *label = find<QLabel>("IDC_PHYSICS_TERMINAL_VELOCITY_STATIC")) {
    label->setText(drag > 0.0f
                       ? QString("Terminal Velocity(thrust/drag): %1").arg(maxThrust / drag)
                       : QString("Terminal Velocity(thrust/drag): 0.0"));
  }
  if (auto *label = find<QLabel>("IDC_PHYSICS_TERMINAL_ROT_VELOCITY_STATIC")) {
    label->setText(rotDrag > 0.0f
                       ? QString("Terminal Rot. Vel.(rot. thrust/ rot. drag): %1")
                             .arg(rotThrust / rotDrag)
                       : QString("Terminal Rot. Vel.(rot. thrust/ rot. drag): 0.0"));
  }
}

void PhysicsDialog::setPhysicsData(const physics_info *p) {
  setEditFloat(find("IDC_PHYSICS_MASS_EDIT"), p->mass);
  setEditFloat(find("IDC_PHYSICS_DRAG_EDIT"), p->drag);
  setEditFloat(find("IDC_PHYSICS_ROTDRAG_EDIT"), p->rotdrag);
  setEditFloat(find("IDC_PHYSICS_MAXTHRUST_EDIT"), p->full_thrust);
  setEditFloat(find("IDC_PHYSICS_ROTTHRUST_EDIT"), p->full_rotthrust);
  setEditFloat(find("IDC_PHYSICS_MAXTURNROLLRATE_EDIT"), p->max_turnroll_rate);
  setEditFloat(find("IDC_PHYSICS_TURNROLLRATIO_EDIT"), p->turnroll_ratio);
  setEditFloat(find("IDC_PHYSICS_WIGGLESIZE_EDIT"), p->wiggle_amplitude);
  setEditFloat(find("IDC_PHYSICS_WIGGLEFREQ_EDIT"), p->wiggles_per_sec);
  setEditFloat(find("IDC_PHYSICS_INIT_VELOCITY_EDIT"), p->velocity.z());
  setEditFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_X_EDIT"), p->rotvel.x());
  setEditFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_Y_EDIT"), p->rotvel.y());
  setEditFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_Z_EDIT"), p->rotvel.z());
  setEditFloat(find("IDC_PHYSICS_MAX_BOUNCES_EDIT"), p->num_bounces);
  setEditFloat(find("IDC_PHYSICS_PERCENT_LOSS_EDIT"),
               (scalar)100.0 - (p->coeff_restitution * (scalar)100.0));

  find<QCheckBox>("IDC_PHYSICS_STICKY_CHECK")->setChecked((p->flags & PF_STICK) != 0);
  find<QCheckBox>("IDC_PHYSICS_BOUNCY_CHECK")->setChecked((p->flags & PF_BOUNCE) != 0);
  find<QCheckBox>("IDC_PHYSICS_THRUSTS_CHECK")->setChecked((p->flags & PF_USES_THRUST) != 0);
  find<QCheckBox>("IDC_PHYSICS_FIXED_VELOCITY_CHECK")->setChecked((p->flags & PF_FIXED_VELOCITY) != 0);
  find<QCheckBox>("IDC_PHYSICS_FIXED_ROTATE_CHECK")->setChecked((p->flags & PF_FIXED_ROT_VELOCITY) != 0);
  find<QCheckBox>("IDC_NO_COLLIDE_CHECK")->setChecked((p->flags & PF_NO_COLLIDE) != 0);
  find<QCheckBox>("IDC_TURN_ROLL_CHECK")->setChecked((p->flags & PF_TURNROLL) != 0);
  find<QCheckBox>("IDC_PHYSICS_AUTO_LEVELING_CHECK")->setChecked((p->flags & PF_LEVELING) != 0);
  find<QCheckBox>("IDC_PHYSICS_POINT_WALL_CHECK")->setChecked((p->flags & PF_POINT_COLLIDE_WALLS) != 0);
  find<QCheckBox>("IDC_PHYSICS_IGNORE_ROBOTS_CHECK")->setChecked((p->flags & PF_NO_ROBOT_COLLISIONS) != 0);
  find<QCheckBox>("IDC_PHYSICS_IGNORE_SAME_CHECK")->setChecked((p->flags & PF_NO_SAME_COLLISIONS) != 0);
  find<QCheckBox>("IDC_PHYSICS_IGNORE_DOOR_CHECK")->setChecked((p->flags & PF_NO_DOOR_COLLISIONS) != 0);
  find<QCheckBox>("IDC_PHYSICS_IGNORE_CONCUSSIVE_FORCES")
      ->setChecked((p->flags & PF_IGNORE_CONCUSSIVE_FORCES) != 0);
  find<QCheckBox>("IDC_L_X_CHECK")->setChecked((p->flags & PF_LOCK_X) != 0);
  find<QCheckBox>("IDC_L_Y_CHECK")->setChecked((p->flags & PF_LOCK_Y) != 0);
  find<QCheckBox>("IDC_L_Z_CHECK")->setChecked((p->flags & PF_LOCK_Z) != 0);
  find<QCheckBox>("IDC_L_P_CHECK")->setChecked((p->flags & PF_LOCK_P) != 0);
  find<QCheckBox>("IDC_L_B_CHECK")->setChecked((p->flags & PF_LOCK_B) != 0);
  find<QCheckBox>("IDC_L_H_CHECK")->setChecked((p->flags & PF_LOCK_H) != 0);
  find<QCheckBox>("IDC_NO_BIG_SPHERE_CHECK")->setChecked((p->flags & PF_NEVER_USE_BIG_SPHERE) != 0);
  find<QCheckBox>("IDC_PHYSICS_WIGGLE_CHECK")->setChecked((p->flags & PF_WIGGLE) != 0);
  find<QCheckBox>("IDC_PHYSICS_MAGNETISM_CHECK")->setChecked((p->flags & PF_IGNORE_OWN_CONC_FORCES) != 0);
  find<QCheckBox>("IDC_PHYSICS_WIND_CHECK")->setChecked((p->flags & PF_WIND) != 0);
  find<QCheckBox>("IDC_PHYSICS_PERSISTENT_CHECK")->setChecked((p->flags & PF_PERSISTENT) != 0);

  m_gravityFlag = p->flags & (PF_GRAVITY | PF_REVERSE_GRAVITY);
  if (m_gravityFlag == PF_GRAVITY)
    find<QRadioButton>("IDC_PHYSICS_GRAVITY_RADIO")->setChecked(true);
  else if (m_gravityFlag == PF_REVERSE_GRAVITY)
    find<QRadioButton>("IDC_PHYSICS_REVERSE_GRAVITY_RADIO")->setChecked(true);
  else
    find<QRadioButton>("IDC_PHYSICS_NO_GRAVITY_RADIO")->setChecked(true);

  setEditFloat(find("IDC_PHYSICS_HIT_DIE_ANGLE"),
               (p->hit_die_dot == -1) ? 0.0f
                                      : (float)(asin(p->hit_die_dot) * ((scalar)180.0 / (scalar)PI)));
  find("IDC_PHYSICS_HIT_DIE_ANGLE")->setEnabled(p->hit_die_dot != -1);
}

void PhysicsDialog::getPhysicsData(physics_info *p) const {
  p->mass = editFloat(find("IDC_PHYSICS_MASS_EDIT"));
  p->drag = editFloat(find("IDC_PHYSICS_DRAG_EDIT"));
  p->rotdrag = editFloat(find("IDC_PHYSICS_ROTDRAG_EDIT"));
  p->full_thrust = editFloat(find("IDC_PHYSICS_MAXTHRUST_EDIT"));
  p->full_rotthrust = editFloat(find("IDC_PHYSICS_ROTTHRUST_EDIT"));
  p->max_turnroll_rate = editFloat(find("IDC_PHYSICS_MAXTURNROLLRATE_EDIT"));
  p->turnroll_ratio = editFloat(find("IDC_PHYSICS_TURNROLLRATIO_EDIT"));
  p->wiggle_amplitude = editFloat(find("IDC_PHYSICS_WIGGLESIZE_EDIT"));
  p->wiggles_per_sec = editFloat(find("IDC_PHYSICS_WIGGLEFREQ_EDIT"));
  p->velocity.z() = editFloat(find("IDC_PHYSICS_INIT_VELOCITY_EDIT"));
  p->rotvel.x() = editFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_X_EDIT"));
  p->rotvel.y() = editFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_Y_EDIT"));
  p->rotvel.z() = editFloat(find("IDC_PHYSICS_INIT_ROT_VELOCITY_Z_EDIT"));
  p->num_bounces = (int)editFloat(find("IDC_PHYSICS_MAX_BOUNCES_EDIT"));
  p->coeff_restitution = ((scalar)100.0 - editFloat(find("IDC_PHYSICS_PERCENT_LOSS_EDIT"))) / (scalar)100.0;

  auto setFlag = [p](uint32_t flag, QCheckBox *cb) {
    if (cb->isChecked())
      p->flags |= flag;
    else
      p->flags &= ~flag;
  };
  setFlag(PF_STICK, find<QCheckBox>("IDC_PHYSICS_STICKY_CHECK"));
  setFlag(PF_BOUNCE, find<QCheckBox>("IDC_PHYSICS_BOUNCY_CHECK"));
  setFlag(PF_USES_THRUST, find<QCheckBox>("IDC_PHYSICS_THRUSTS_CHECK"));
  setFlag(PF_FIXED_VELOCITY, find<QCheckBox>("IDC_PHYSICS_FIXED_VELOCITY_CHECK"));
  setFlag(PF_FIXED_ROT_VELOCITY, find<QCheckBox>("IDC_PHYSICS_FIXED_ROTATE_CHECK"));
  setFlag(PF_NO_COLLIDE, find<QCheckBox>("IDC_NO_COLLIDE_CHECK"));
  setFlag(PF_TURNROLL, find<QCheckBox>("IDC_TURN_ROLL_CHECK"));
  setFlag(PF_LEVELING, find<QCheckBox>("IDC_PHYSICS_AUTO_LEVELING_CHECK"));
  setFlag(PF_POINT_COLLIDE_WALLS, find<QCheckBox>("IDC_PHYSICS_POINT_WALL_CHECK"));
  setFlag(PF_NO_ROBOT_COLLISIONS, find<QCheckBox>("IDC_PHYSICS_IGNORE_ROBOTS_CHECK"));
  setFlag(PF_NO_SAME_COLLISIONS, find<QCheckBox>("IDC_PHYSICS_IGNORE_SAME_CHECK"));
  setFlag(PF_NO_DOOR_COLLISIONS, find<QCheckBox>("IDC_PHYSICS_IGNORE_DOOR_CHECK"));
  setFlag(PF_IGNORE_CONCUSSIVE_FORCES, find<QCheckBox>("IDC_PHYSICS_IGNORE_CONCUSSIVE_FORCES"));
  setFlag(PF_LOCK_X, find<QCheckBox>("IDC_L_X_CHECK"));
  setFlag(PF_LOCK_Y, find<QCheckBox>("IDC_L_Y_CHECK"));
  setFlag(PF_LOCK_Z, find<QCheckBox>("IDC_L_Z_CHECK"));
  setFlag(PF_LOCK_P, find<QCheckBox>("IDC_L_P_CHECK"));
  setFlag(PF_LOCK_B, find<QCheckBox>("IDC_L_B_CHECK"));
  setFlag(PF_LOCK_H, find<QCheckBox>("IDC_L_H_CHECK"));
  setFlag(PF_NEVER_USE_BIG_SPHERE, find<QCheckBox>("IDC_NO_BIG_SPHERE_CHECK"));
  setFlag(PF_WIGGLE, find<QCheckBox>("IDC_PHYSICS_WIGGLE_CHECK"));
  setFlag(PF_IGNORE_OWN_CONC_FORCES, find<QCheckBox>("IDC_PHYSICS_MAGNETISM_CHECK"));
  setFlag(PF_WIND, find<QCheckBox>("IDC_PHYSICS_WIND_CHECK"));
  setFlag(PF_PERSISTENT, find<QCheckBox>("IDC_PHYSICS_PERSISTENT_CHECK"));

  if (p->hit_die_dot != -1)
    p->hit_die_dot = sin(editFloat(find("IDC_PHYSICS_HIT_DIE_ANGLE")) * (scalar)PI / (scalar)180.0);
  p->flags &= ~PF_GRAVITY_MASK;
  p->flags |= m_gravityFlag;
}

void PhysicsDialog::onWiggleCheck() { enableDisableWiggle(); }
void PhysicsDialog::onGravityRadio() { m_gravityFlag = PF_GRAVITY; }
void PhysicsDialog::onNoGravityRadio() { m_gravityFlag = 0; }
void PhysicsDialog::onReverseGravityRadio() { m_gravityFlag = PF_REVERSE_GRAVITY; }
void PhysicsDialog::onTurnRollCheck() { enableDisableTurnRoll(); }
void PhysicsDialog::onBouncyCheck() { enableDisableBounce(); }
void PhysicsDialog::onThrustsCheck() { enableDisableThrust(); }
void PhysicsDialog::onTerminalEditChanged() { updateTerminalText(); }

void PhysicsDialog::onCopy() {
  getPhysicsData(&Paste_data);
  Paste_data_used = true;
  if (QPushButton *paste = find<QPushButton>("IDC_PHYSICS_PASTE"))
    paste->setEnabled(true);
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

}
