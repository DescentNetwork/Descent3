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
  connect(ui->IDC_ALWAYS_ON_RADIO, &QRadioButton::clicked, this, &GenericLightDialog::onAlwaysOnRadio);
  connect(ui->IDC_FLICKER_RADIO, &QRadioButton::clicked, this, &GenericLightDialog::onFlickerRadio);
  connect(ui->IDC_USE_TIMEBITS_RADIO, &QRadioButton::clicked, this, &GenericLightDialog::onUseTimebitsRadio);
  connect(ui->IDC_RENDER_STATIC, &QRadioButton::clicked, this, &GenericLightDialog::onRenderStatic);
  connect(ui->IDC_RENDER_GOURAUD, &QRadioButton::clicked, this, &GenericLightDialog::onRenderGouraud);
  connect(ui->IDC_RENDER_LIGHTMAPS, &QRadioButton::clicked, this, &GenericLightDialog::onRenderLightmaps);

  connect(ui->IDC_PULSE, &QCheckBox::toggled, this, &GenericLightDialog::onPulseToggled);
  connect(ui->IDC_PULSE_TO_SECOND, &QCheckBox::toggled, this, &GenericLightDialog::onPulseToSecondToggled);
  connect(ui->IDC_FLICKER_SLIGHTLY_CHECK, &QCheckBox::toggled, this, &GenericLightDialog::onFlickerSlightlyToggled);
  connect(ui->IDC_DIRECTIONAL, &QCheckBox::toggled, this, &GenericLightDialog::onDirectionalToggled);
  connect(ui->IDC_NO_SPECULAR_CHECK, &QCheckBox::toggled, this, &GenericLightDialog::onNoSpecularToggled);

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

void GenericLightDialog::setFlag(int32_t flag, const char *checkName, bool checked) {
  if (checked)
    m_lightinfo->flags |= flag;
  else
    m_lightinfo->flags &= ~flag;
}

void GenericLightDialog::updateDialog() {
  light_info *li = m_lightinfo;
  ui->IDC_DIRECTIONAL->setChecked(li->flags & OLF_DIRECTIONAL);
  ui->IDC_NO_SPECULAR_CHECK->setChecked(li->flags & OLF_NO_SPECULARITY);
  ui->IDC_PULSE->setChecked(li->flags & OLF_PULSE);
  ui->IDC_PULSE_TO_SECOND->setChecked(li->flags & OLF_PULSE_TO_SECOND);
  ui->IDC_FLICKER_SLIGHTLY_CHECK->setChecked(li->flags & OLF_FLICKER_SLIGHTLY);

  ui->IDC_FLICKER_RADIO->setChecked(li->flags & OLF_FLICKERING);
  ui->IDC_USE_TIMEBITS_RADIO->setChecked(li->flags & OLF_TIMEBITS);
  ui->IDC_ALWAYS_ON_RADIO->setChecked(!(li->flags & (OLF_FLICKERING | OLF_TIMEBITS)));

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

void GenericLightDialog::onFlickerRadio() {
  m_lightinfo->flags |= OLF_FLICKERING;
  m_lightinfo->flags &= ~OLF_TIMEBITS;
}
void GenericLightDialog::onAlwaysOnRadio() {
  m_lightinfo->flags &= ~(OLF_FLICKERING | OLF_TIMEBITS);
}
void GenericLightDialog::onUseTimebitsRadio() {
  m_lightinfo->flags |= OLF_TIMEBITS;
  m_lightinfo->flags &= ~OLF_FLICKERING;
}
void GenericLightDialog::onPulseToggled(bool checked) { setFlag(OLF_PULSE, "", checked); }
void GenericLightDialog::onPulseToSecondToggled(bool checked) { setFlag(OLF_PULSE_TO_SECOND, "", checked); }
void GenericLightDialog::onFlickerSlightlyToggled(bool checked) { setFlag(OLF_FLICKER_SLIGHTLY, "", checked); }
void GenericLightDialog::onDirectionalToggled(bool checked) { setFlag(OLF_DIRECTIONAL, "", checked); }
void GenericLightDialog::onNoSpecularToggled(bool checked) { setFlag(OLF_NO_SPECULARITY, "", checked); }

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

void GenericLightDialog::onRenderStatic() { m_lightinfo->lighting_render_type = LRT_STATIC; }
void GenericLightDialog::onRenderGouraud() { m_lightinfo->lighting_render_type = LRT_GOURAUD; }
void GenericLightDialog::onRenderLightmaps() { m_lightinfo->lighting_render_type = LRT_LIGHTMAPS; }

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

