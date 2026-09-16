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

#include "generic_light_dialog.h"
#include "ui_genericlight.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "object.h"


GenericLightDialog::GenericLightDialog(light_info *lightinfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::GenericLightDialog), m_lightinfo(lightinfo)
{
  ui->setupUi(this);

  auto& rt = m_lightinfo->lighting_render_type;
  connect(ui->IDC_RENDER_STATIC,    &QRadioButton::clicked, [&rt](){ rt = LRT_STATIC; });
  connect(ui->IDC_RENDER_GOURAUD,   &QRadioButton::clicked, [&rt](){ rt = LRT_GOURAUD; });
  connect(ui->IDC_RENDER_LIGHTMAPS, &QRadioButton::clicked, [&rt](){ rt = LRT_LIGHTMAPS; });

  auto& flags = m_lightinfo->flags;
  connect(ui->IDC_ALWAYS_ON_RADIO,    &QRadioButton::clicked, [&flags](){ flags.flickering = 0; flags.timebits = 0; });
  connect(ui->IDC_USE_TIMEBITS_RADIO, &QRadioButton::clicked, [&flags](){ flags.flickering = 0; flags.timebits = 1; });
  connect(ui->IDC_FLICKER_RADIO,      &QRadioButton::clicked, [&flags](){ flags.flickering = 1; flags.timebits = 1; });

  connect(ui->IDC_PULSE,                  &QCheckBox::toggled, [&flags](){ flags.pulse            ^= flags.pulse; });
  connect(ui->IDC_PULSE_TO_SECOND,        &QCheckBox::toggled, [&flags](){ flags.pulse_to_second  ^= flags.pulse_to_second; });
  connect(ui->IDC_FLICKER_SLIGHTLY_CHECK, &QCheckBox::toggled, [&flags](){ flags.flicker_slightly ^= flags.flicker_slightly; });
  connect(ui->IDC_DIRECTIONAL,            &QCheckBox::toggled, [&flags](){ flags.directional      ^= flags.directional; });
  connect(ui->IDC_NO_SPECULAR_CHECK,      &QCheckBox::toggled, [&flags](){ flags.no_specularity   ^= flags.no_specularity; });

  connect(ui->IDC_TIME_CHECK1, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK2, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK3, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK4, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK5, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK6, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK7, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);
  connect(ui->IDC_TIME_CHECK8, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);

  connect(ui->IDC_GENERIC_LIGHT_DISTANCE, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_PULSE_TIME, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_RED_LIGHT, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_GREEN_LIGHT, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_BLUE_LIGHT, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_RED_LIGHT2, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_GREEN_LIGHT2, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_GENERIC_BLUE_LIGHT2, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_FLICKER_SLIGHTLY_EDIT, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);
  connect(ui->IDC_FOV_EDIT, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);

  updateDialog();
}

GenericLightDialog::~GenericLightDialog() { delete ui; }


void GenericLightDialog::updateDialog() {
  light_info *li = m_lightinfo;
  ui->IDC_DIRECTIONAL->setChecked(li->flags.directional);
  ui->IDC_NO_SPECULAR_CHECK->setChecked(li->flags.no_specularity);
  ui->IDC_PULSE->setChecked(li->flags.pulse);
  ui->IDC_PULSE_TO_SECOND->setChecked(li->flags.pulse_to_second);
  ui->IDC_FLICKER_SLIGHTLY_CHECK->setChecked(li->flags.flicker_slightly);

  ui->IDC_FLICKER_RADIO->setChecked(li->flags.flickering);
  ui->IDC_USE_TIMEBITS_RADIO->setChecked(li->flags.timebits);
  ui->IDC_ALWAYS_ON_RADIO->setChecked(!li->flags.flickering && !li->flags.timebits);

  ui->IDC_TIME_CHECK1->setChecked((li->timebits & (1 << 0)) != 0);
  ui->IDC_TIME_CHECK2->setChecked((li->timebits & (1 << 1)) != 0);
  ui->IDC_TIME_CHECK3->setChecked((li->timebits & (1 << 2)) != 0);
  ui->IDC_TIME_CHECK4->setChecked((li->timebits & (1 << 3)) != 0);
  ui->IDC_TIME_CHECK5->setChecked((li->timebits & (1 << 4)) != 0);
  ui->IDC_TIME_CHECK6->setChecked((li->timebits & (1 << 5)) != 0);
  ui->IDC_TIME_CHECK7->setChecked((li->timebits & (1 << 6)) != 0);
  ui->IDC_TIME_CHECK8->setChecked((li->timebits & (1 << 7)) != 0);

  const int rt = li->lighting_render_type;
  ui->IDC_RENDER_STATIC->setChecked(rt == LRT_STATIC);
  ui->IDC_RENDER_GOURAUD->setChecked(rt == LRT_GOURAUD);
  ui->IDC_RENDER_LIGHTMAPS->setChecked(rt == LRT_LIGHTMAPS);

  ui->IDC_GENERIC_LIGHT_DISTANCE->setText(QString::number(li->light_distance));
  ui->IDC_GENERIC_PULSE_TIME->setText(QString::number(li->time_interval));
  ui->IDC_GENERIC_RED_LIGHT->setText(QString::number(li->red_light1));
  ui->IDC_GENERIC_GREEN_LIGHT->setText(QString::number(li->green_light1));
  ui->IDC_GENERIC_BLUE_LIGHT->setText(QString::number(li->blue_light1));
  ui->IDC_GENERIC_RED_LIGHT2->setText(QString::number(li->red_light2));
  ui->IDC_GENERIC_GREEN_LIGHT2->setText(QString::number(li->green_light2));
  ui->IDC_GENERIC_BLUE_LIGHT2->setText(QString::number(li->blue_light2));
  ui->IDC_FLICKER_SLIGHTLY_EDIT->setText(QString::number(li->flicker_distance));
}

void GenericLightDialog::onTimeCheck() {
  int bits = 0;
  if (ui->IDC_TIME_CHECK1->isChecked())
    bits |= (1 << 0);
  if (ui->IDC_TIME_CHECK2->isChecked())
    bits |= (1 << 1);
  if (ui->IDC_TIME_CHECK3->isChecked())
    bits |= (1 << 2);
  if (ui->IDC_TIME_CHECK4->isChecked())
    bits |= (1 << 3);
  if (ui->IDC_TIME_CHECK5->isChecked())
    bits |= (1 << 4);
  if (ui->IDC_TIME_CHECK6->isChecked())
    bits |= (1 << 5);
  if (ui->IDC_TIME_CHECK7->isChecked())
    bits |= (1 << 6);
  if (ui->IDC_TIME_CHECK8->isChecked())
    bits |= (1 << 7);
  m_lightinfo->timebits = bits;
}


void GenericLightDialog::onFieldEdited() {
  light_info *li = m_lightinfo;
  li->light_distance = ui->IDC_GENERIC_LIGHT_DISTANCE->text().toFloat();
  li->time_interval = ui->IDC_GENERIC_PULSE_TIME->text().toFloat();
  li->red_light1 = ui->IDC_GENERIC_RED_LIGHT->text().toFloat();
  li->green_light1 = ui->IDC_GENERIC_GREEN_LIGHT->text().toFloat();
  li->blue_light1 = ui->IDC_GENERIC_BLUE_LIGHT->text().toFloat();
  li->red_light2 = ui->IDC_GENERIC_RED_LIGHT2->text().toFloat();
  li->green_light2 = ui->IDC_GENERIC_GREEN_LIGHT2->text().toFloat();
  li->blue_light2 = ui->IDC_GENERIC_BLUE_LIGHT2->text().toFloat();
  li->flicker_distance = ui->IDC_FLICKER_SLIGHTLY_EDIT->text().toFloat();
}

