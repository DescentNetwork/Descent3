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

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "object_external_struct.h"
#include "physics.h"

namespace QtEditor {

namespace {
const struct {
  const char *name;
  uint32_t flag;
} kFlagChecks[] = {
    {"IDC_PTURNROLL", PF_TURNROLL},      {"IDC_PLEVELLING", PF_LEVELING},
    {"IDC_PBOUNCE", PF_BOUNCE},          {"IDC_PWIGGLE", PF_WIGGLE},
    {"IDC_PSTICKS", PF_STICK},           {"IDC_PPERSISTENT", PF_PERSISTENT},
    {"IDC_PUSESTHRUST", PF_USES_THRUST}, {"IDC_PGRAVITY", PF_GRAVITY},
    {"IDC_PWIND", PF_WIND},
};

} // namespace

PropertyPhysicsDialog::PropertyPhysicsDialog(physics_info *physInfo, QWidget *parent)
    : Dialog(":/ui/propphysics.ui", parent), m_physInfo(physInfo) {
  for (const auto &c : kFlagChecks)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, &PropertyPhysicsDialog::onFlagToggled);

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &PropertyPhysicsDialog::onOk);
  }

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
    if (QLineEdit *e = find<QLineEdit>(f.name))
      connect(e, &QLineEdit::editingFinished, this, [this, f]() {
        m_physInfo->*f.field = find<QLineEdit>(f.name)->text().toFloat();
      });

  updateDialog();
}

PropertyPhysicsDialog::~PropertyPhysicsDialog() = default;

void PropertyPhysicsDialog::setFlag(uint32_t flag, const char *checkName, bool checked) {
  if (checked)
    m_physInfo->flags |= flag;
  else
    m_physInfo->flags &= ~flag;
}

void PropertyPhysicsDialog::updateDialog() {
  for (const auto &c : kFlagChecks)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      cb->setChecked(m_physInfo->flags & c.flag);
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
    if (QLineEdit *e = find<QLineEdit>(f.name))
      e->setText(QString::number(m_physInfo->*f.field));
}

void PropertyPhysicsDialog::onFlagToggled() {
  QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
  if (cb == nullptr)
    return;
  for (const auto &c : kFlagChecks)
    if (strcmp(c.name, cb->objectName().toLatin1().constData()) == 0)
      setFlag(c.flag, c.name, cb->isChecked());
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
    if (QLineEdit *e = find<QLineEdit>(f.name))
      m_physInfo->*f.field = e->text().toFloat();
  m_dialog->accept();
}

}
