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

#include "property_physics_dialog.h"
#include "ui_propphysics.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "object_external_struct.h"
#include "physics.h"


namespace {
struct FlagCheck {
  const char *name;
  bool (*get)(const physics_flags_t &);
  void (*set)(physics_flags_t &, bool);
};
const FlagCheck kFlagChecks[] = {
    {"IDC_PTURNROLL", [](const physics_flags_t &f) { return f.turnroll != 0; },
     [](physics_flags_t &f, bool v) { f.turnroll = v; }},
    {"IDC_PLEVELLING", [](const physics_flags_t &f) { return f.leveling != 0; },
     [](physics_flags_t &f, bool v) { f.leveling = v; }},
    {"IDC_PBOUNCE", [](const physics_flags_t &f) { return f.bounce != 0; },
     [](physics_flags_t &f, bool v) { f.bounce = v; }},
    {"IDC_PWIGGLE", [](const physics_flags_t &f) { return f.wiggle != 0; },
     [](physics_flags_t &f, bool v) { f.wiggle = v; }},
    {"IDC_PSTICKS", [](const physics_flags_t &f) { return f.stick != 0; },
     [](physics_flags_t &f, bool v) { f.stick = v; }},
    {"IDC_PPERSISTENT", [](const physics_flags_t &f) { return f.persistent != 0; },
     [](physics_flags_t &f, bool v) { f.persistent = v; }},
    {"IDC_PUSESTHRUST", [](const physics_flags_t &f) { return f.uses_thrust != 0; },
     [](physics_flags_t &f, bool v) { f.uses_thrust = v; }},
    {"IDC_PGRAVITY", [](const physics_flags_t &f) { return f.gravity != 0; },
     [](physics_flags_t &f, bool v) { f.gravity = v; }},
    {"IDC_PWIND", [](const physics_flags_t &f) { return f.wind != 0; },
     [](physics_flags_t &f, bool v) { f.wind = v; }},
};

} // namespace

PropertyPhysicsDialog::PropertyPhysicsDialog(physics_info *physInfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::PropertyPhysicsDialog), m_physInfo(physInfo)
{
  ui->setupUi(this);
  for (const auto &c : kFlagChecks)
    if (QCheckBox *cb = findChild<QCheckBox*>(c.name))
      connect(cb, &QCheckBox::toggled, this, &PropertyPhysicsDialog::onFlagToggled);

  connect(this, &QDialog::accept, this, &PropertyPhysicsDialog::onOk);

  const struct {
    const char *name;
    float physics_info::*field;
  } fields[] = {
      {"IDC_PMASS", &physics_info::mass},
      {"IDC_PDRAG", &physics_info::drag},
      {"IDC_PROTDRAG", &physics_info::rotdrag},
      {"IDC_PFULL_THRUST", &physics_info::full_thrust},
      {"IDC_PFULL_ROTTHRUST", &physics_info::full_rotthrust},
      {"IDC_PMAX_TURNROLL_RATE", &physics_info::max_turnroll_rate},
      {"IDC_PTURNROLL_RATIO", &physics_info::turnroll_ratio},
      {"IDC_PWIGGLE_AMPLITUDE", &physics_info::wiggle_amplitude},
      {"IDC_PWIGGLES_PER_SECOND", &physics_info::wiggles_per_sec},
  };
  for (const auto &f : fields)
    if (QLineEdit *e = findChild<QLineEdit*>(f.name))
      connect(e, &QLineEdit::editingFinished, this, [this, f]() {
        m_physInfo->*f.field = findChild<QLineEdit*>(f.name)->text().toFloat();
      });

  updateDialog();
}

PropertyPhysicsDialog::~PropertyPhysicsDialog() { delete ui; }

void PropertyPhysicsDialog::updateDialog() {
  for (const auto &c : kFlagChecks)
    if (QCheckBox *cb = findChild<QCheckBox*>(c.name))
      cb->setChecked(c.get(m_physInfo->flags));
  const struct {
    const char *name;
    float physics_info::*field;
  } fields[] = {
      {"IDC_PMASS", &physics_info::mass},
      {"IDC_PDRAG", &physics_info::drag},
      {"IDC_PROTDRAG", &physics_info::rotdrag},
      {"IDC_PFULL_THRUST", &physics_info::full_thrust},
      {"IDC_PFULL_ROTTHRUST", &physics_info::full_rotthrust},
      {"IDC_PMAX_TURNROLL_RATE", &physics_info::max_turnroll_rate},
      {"IDC_PTURNROLL_RATIO", &physics_info::turnroll_ratio},
      {"IDC_PWIGGLE_AMPLITUDE", &physics_info::wiggle_amplitude},
      {"IDC_PWIGGLES_PER_SECOND", &physics_info::wiggles_per_sec},
  };
  for (const auto &f : fields)
    if (QLineEdit *e = findChild<QLineEdit*>(f.name))
      e->setText(QString::number(m_physInfo->*f.field));
}

void PropertyPhysicsDialog::onFlagToggled() {
  QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
  if (cb == nullptr)
    return;
  for (const auto &c : kFlagChecks)
    if (strcmp(c.name, cb->objectName().toLatin1().constData()) == 0)
      c.set(m_physInfo->flags, cb->isChecked());
}

void PropertyPhysicsDialog::onOk() {
  const struct {
    const char *name;
    float physics_info::*field;
  } fields[] = {
      {"IDC_PMASS", &physics_info::mass}, {"IDC_PDRAG", &physics_info::drag},
      {"IDC_PROTDRAG", &physics_info::rotdrag}, {"IDC_PFULL_THRUST", &physics_info::full_thrust},
      {"IDC_PFULL_ROTTHRUST", &physics_info::full_rotthrust},
      {"IDC_PMAX_TURNROLL_RATE", &physics_info::max_turnroll_rate},
      {"IDC_PTURNROLL_RATIO", &physics_info::turnroll_ratio},
      {"IDC_PWIGGLE_AMPLITUDE", &physics_info::wiggle_amplitude},
      {"IDC_PWIGGLES_PER_SECOND", &physics_info::wiggles_per_sec},
  };
  for (const auto &f : fields)
    if (QLineEdit *e = findChild<QLineEdit*>(f.name))
      m_physInfo->*f.field = e->text().toFloat();
  accept();
}

