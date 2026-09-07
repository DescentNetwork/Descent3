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


PropertyPhysicsDialog::PropertyPhysicsDialog(physics_info *physInfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::PropertyPhysicsDialog), m_physInfo(physInfo)
{
  ui->setupUi(this);

  connect(ui->IDC_PTURNROLL, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.turnroll = checked; });
  connect(ui->IDC_PLEVELLING, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.leveling = checked; });
  connect(ui->IDC_PBOUNCE, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.bounce = checked; });
  connect(ui->IDC_PWIGGLE, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.wiggle = checked; });
  connect(ui->IDC_PSTICKS, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.stick = checked; });
  connect(ui->IDC_PPERSISTENT, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.persistent = checked; });
  connect(ui->IDC_PUSESTHRUST, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.uses_thrust = checked; });
  connect(ui->IDC_PGRAVITY, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.gravity = checked; });
  connect(ui->IDC_PWIND, &QCheckBox::toggled, this, [this](bool checked) { m_physInfo->flags.wind = checked; });

  connect(ui->IDC_PMASS, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->mass = ui->IDC_PMASS->text().toFloat();
  });
  connect(ui->IDC_PDRAG, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->drag = ui->IDC_PDRAG->text().toFloat();
  });
  connect(ui->IDC_PROTDRAG, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->rotdrag = ui->IDC_PROTDRAG->text().toFloat();
  });
  connect(ui->IDC_PFULL_THRUST, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->full_thrust = ui->IDC_PFULL_THRUST->text().toFloat();
  });
  connect(ui->IDC_PFULL_ROTTHRUST, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->full_rotthrust = ui->IDC_PFULL_ROTTHRUST->text().toFloat();
  });
  connect(ui->IDC_PMAX_TURNROLL_RATE, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->max_turnroll_rate = ui->IDC_PMAX_TURNROLL_RATE->text().toFloat();
  });
  connect(ui->IDC_PTURNROLL_RATIO, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->turnroll_ratio = ui->IDC_PTURNROLL_RATIO->text().toFloat();
  });
  connect(ui->IDC_PWIGGLE_AMPLITUDE, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->wiggle_amplitude = ui->IDC_PWIGGLE_AMPLITUDE->text().toFloat();
  });
  connect(ui->IDC_PWIGGLES_PER_SECOND, &QLineEdit::editingFinished, this, [this]() {
    m_physInfo->wiggles_per_sec = ui->IDC_PWIGGLES_PER_SECOND->text().toFloat();
  });

  connect(this, &QDialog::accept, this, &PropertyPhysicsDialog::onOk);

  updateDialog();
}

PropertyPhysicsDialog::~PropertyPhysicsDialog() { delete ui; }

void PropertyPhysicsDialog::updateDialog() {
  ui->IDC_PTURNROLL->setChecked(m_physInfo->flags.turnroll);
  ui->IDC_PLEVELLING->setChecked(m_physInfo->flags.leveling);
  ui->IDC_PBOUNCE->setChecked(m_physInfo->flags.bounce);
  ui->IDC_PWIGGLE->setChecked(m_physInfo->flags.wiggle);
  ui->IDC_PSTICKS->setChecked(m_physInfo->flags.stick);
  ui->IDC_PPERSISTENT->setChecked(m_physInfo->flags.persistent);
  ui->IDC_PUSESTHRUST->setChecked(m_physInfo->flags.uses_thrust);
  ui->IDC_PGRAVITY->setChecked(m_physInfo->flags.gravity);
  ui->IDC_PWIND->setChecked(m_physInfo->flags.wind);

  ui->IDC_PMASS->setText(QString::number(m_physInfo->mass));
  ui->IDC_PDRAG->setText(QString::number(m_physInfo->drag));
  ui->IDC_PROTDRAG->setText(QString::number(m_physInfo->rotdrag));
  ui->IDC_PFULL_THRUST->setText(QString::number(m_physInfo->full_thrust));
  ui->IDC_PFULL_ROTTHRUST->setText(QString::number(m_physInfo->full_rotthrust));
  ui->IDC_PMAX_TURNROLL_RATE->setText(QString::number(m_physInfo->max_turnroll_rate));
  ui->IDC_PTURNROLL_RATIO->setText(QString::number(m_physInfo->turnroll_ratio));
  ui->IDC_PWIGGLE_AMPLITUDE->setText(QString::number(m_physInfo->wiggle_amplitude));
  ui->IDC_PWIGGLES_PER_SECOND->setText(QString::number(m_physInfo->wiggles_per_sec));
}

void PropertyPhysicsDialog::onOk() {
  m_physInfo->mass = ui->IDC_PMASS->text().toFloat();
  m_physInfo->drag = ui->IDC_PDRAG->text().toFloat();
  m_physInfo->rotdrag = ui->IDC_PROTDRAG->text().toFloat();
  m_physInfo->full_thrust = ui->IDC_PFULL_THRUST->text().toFloat();
  m_physInfo->full_rotthrust = ui->IDC_PFULL_ROTTHRUST->text().toFloat();
  m_physInfo->max_turnroll_rate = ui->IDC_PMAX_TURNROLL_RATE->text().toFloat();
  m_physInfo->turnroll_ratio = ui->IDC_PTURNROLL_RATIO->text().toFloat();
  m_physInfo->wiggle_amplitude = ui->IDC_PWIGGLE_AMPLITUDE->text().toFloat();
  m_physInfo->wiggles_per_sec = ui->IDC_PWIGGLES_PER_SECOND->text().toFloat();
  accept();
}