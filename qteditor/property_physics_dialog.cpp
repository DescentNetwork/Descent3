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

#include "physics.h"
#include "objinfo.h"


optref<physics_info> PropertyPhysicsDialog::data(void)
{
  if(m_object_id < 0)
    return std::nullopt;
  return Object_info[m_object_id].phys_info;
}

PropertyPhysicsDialog::PropertyPhysicsDialog(int object_id, QWidget *parent)
    : QDialog(parent), ui(new Ui::PropertyPhysicsDialog), m_object_id(object_id)
{
  ui->setupUi(this);

  connect(ui->IDC_PTURNROLL,    &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.turnroll    = checked; });
  connect(ui->IDC_PLEVELLING,   &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.leveling    = checked; });
  connect(ui->IDC_PBOUNCE,      &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.bounce      = checked; });
  connect(ui->IDC_PWIGGLE,      &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.wiggle      = checked; });
  connect(ui->IDC_PSTICKS,      &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.stick       = checked; });
  connect(ui->IDC_PPERSISTENT,  &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.persistent  = checked; });
  connect(ui->IDC_PUSESTHRUST,  &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.uses_thrust = checked; });
  connect(ui->IDC_PGRAVITY,     &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.gravity     = checked; });
  connect(ui->IDC_PWIND,        &QCheckBox::toggled,  [this](bool checked) { if(data()) data()->flags.wind        = checked; });

  connect(ui->IDC_PMASS,        &QLineEdit::editingFinished, [this]() {
    if(data()) data()->mass = ui->IDC_PMASS->text().toFloat();
  });
  connect(ui->IDC_PDRAG,        &QLineEdit::editingFinished, [this]() {
    if(data()) data()->drag = ui->IDC_PDRAG->text().toFloat();
  });
  connect(ui->IDC_PROTDRAG,     &QLineEdit::editingFinished, [this]() {
    if(data()) data()->rotdrag = ui->IDC_PROTDRAG->text().toFloat();
  });
  connect(ui->IDC_PFULL_THRUST, &QLineEdit::editingFinished, [this]() {
    if(data()) data()->full_thrust = ui->IDC_PFULL_THRUST->text().toFloat();
  });
  connect(ui->IDC_PFULL_ROTTHRUST,      &QLineEdit::editingFinished, [this]() {
    if(data()) data()->full_rotthrust = ui->IDC_PFULL_ROTTHRUST->text().toFloat();
  });
  connect(ui->IDC_PMAX_TURNROLL_RATE,   &QLineEdit::editingFinished, [this]() {
    if(data()) data()->max_turnroll_rate = ui->IDC_PMAX_TURNROLL_RATE->text().toFloat();
  });
  connect(ui->IDC_PTURNROLL_RATIO,      &QLineEdit::editingFinished, [this]() {
    if(data()) data()->turnroll_ratio = ui->IDC_PTURNROLL_RATIO->text().toFloat();
  });
  connect(ui->IDC_PWIGGLE_AMPLITUDE,    &QLineEdit::editingFinished, [this]() {
    if(data()) data()->wiggle_amplitude = ui->IDC_PWIGGLE_AMPLITUDE->text().toFloat();
  });
  connect(ui->IDC_PWIGGLES_PER_SECOND,  &QLineEdit::editingFinished, [this]() {
    if(data()) data()->wiggles_per_sec = ui->IDC_PWIGGLES_PER_SECOND->text().toFloat();
  });

  //connect(ui->IDOK, &QPushButton::clicked, this, &PropertyPhysicsDialog::onOk);

  updateDialog();
}

PropertyPhysicsDialog::~PropertyPhysicsDialog() { delete ui; }

void PropertyPhysicsDialog::updateDialog() {
  if(auto physInfo = data(); physInfo)
  {
    ui->IDC_PTURNROLL->setChecked(physInfo->flags.turnroll);
    ui->IDC_PLEVELLING->setChecked(physInfo->flags.leveling);
    ui->IDC_PBOUNCE->setChecked(physInfo->flags.bounce);
    ui->IDC_PWIGGLE->setChecked(physInfo->flags.wiggle);
    ui->IDC_PSTICKS->setChecked(physInfo->flags.stick);
    ui->IDC_PPERSISTENT->setChecked(physInfo->flags.persistent);
    ui->IDC_PUSESTHRUST->setChecked(physInfo->flags.uses_thrust);
    ui->IDC_PGRAVITY->setChecked(physInfo->flags.gravity);
    ui->IDC_PWIND->setChecked(physInfo->flags.wind);

    ui->IDC_PMASS->setText(QString::number(physInfo->mass));
    ui->IDC_PDRAG->setText(QString::number(physInfo->drag));
    ui->IDC_PROTDRAG->setText(QString::number(physInfo->rotdrag));
    ui->IDC_PFULL_THRUST->setText(QString::number(physInfo->full_thrust));
    ui->IDC_PFULL_ROTTHRUST->setText(QString::number(physInfo->full_rotthrust));
    ui->IDC_PMAX_TURNROLL_RATE->setText(QString::number(physInfo->max_turnroll_rate));
    ui->IDC_PTURNROLL_RATIO->setText(QString::number(physInfo->turnroll_ratio));
    ui->IDC_PWIGGLE_AMPLITUDE->setText(QString::number(physInfo->wiggle_amplitude));
    ui->IDC_PWIGGLES_PER_SECOND->setText(QString::number(physInfo->wiggles_per_sec));
  }
}

void PropertyPhysicsDialog::onOk() {
  if(auto physInfo = data(); physInfo)
  {
    physInfo->mass        = ui->IDC_PMASS->text().toFloat();
    physInfo->drag        = ui->IDC_PDRAG->text().toFloat();
    physInfo->rotdrag     = ui->IDC_PROTDRAG->text().toFloat();
    physInfo->full_thrust = ui->IDC_PFULL_THRUST->text().toFloat();
    physInfo->full_rotthrust    = ui->IDC_PFULL_ROTTHRUST->text().toFloat();
    physInfo->max_turnroll_rate = ui->IDC_PMAX_TURNROLL_RATE->text().toFloat();
    physInfo->turnroll_ratio    = ui->IDC_PTURNROLL_RATIO->text().toFloat();
    physInfo->wiggle_amplitude  = ui->IDC_PWIGGLE_AMPLITUDE->text().toFloat();
    physInfo->wiggles_per_sec   = ui->IDC_PWIGGLES_PER_SECOND->text().toFloat();
  }
  accept();
}
