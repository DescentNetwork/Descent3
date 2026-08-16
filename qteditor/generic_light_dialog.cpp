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

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include "object.h"

namespace QtEditor {

namespace {
const char *kTimeChecks[8] = {"IDC_TIME_CHECK1", "IDC_TIME_CHECK2", "IDC_TIME_CHECK3", "IDC_TIME_CHECK4",
                              "IDC_TIME_CHECK5", "IDC_TIME_CHECK6", "IDC_TIME_CHECK7", "IDC_TIME_CHECK8"};
} // namespace

GenericLightDialog::GenericLightDialog(light_info *lightinfo, QWidget *parent)
    : Dialog(":/ui/genericlight.ui", parent), m_lightinfo(lightinfo) {
  if (QRadioButton *rb = find<QRadioButton>("IDC_ALWAYS_ON_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onAlwaysOnRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_FLICKER_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onFlickerRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_USE_TIMEBITS_RADIO"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onUseTimebitsRadio);
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_STATIC"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onRenderStatic);
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_GOURAUD"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onRenderGouraud);
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_LIGHTMAPS"))
    connect(rb, &QRadioButton::clicked, this, &GenericLightDialog::onRenderLightmaps);

  const struct {
    const char *name;
    void (GenericLightDialog::*slot)(bool);
    int32_t flag;
  } checks[] = {
      {"IDC_PULSE", &GenericLightDialog::onPulseToggled, OLF_PULSE},
      {"IDC_PULSE_TO_SECOND", &GenericLightDialog::onPulseToSecondToggled, OLF_PULSE_TO_SECOND},
      {"IDC_FLICKER_SLIGHTLY_CHECK", &GenericLightDialog::onFlickerSlightlyToggled, OLF_FLICKER_SLIGHTLY},
      {"IDC_DIRECTIONAL", &GenericLightDialog::onDirectionalToggled, OLF_DIRECTIONAL},
      {"IDC_NO_SPECULAR_CHECK", &GenericLightDialog::onNoSpecularToggled, OLF_NO_SPECULARITY},
  };
  for (const auto &c : checks)
    if (QCheckBox *cb = find<QCheckBox>(c.name))
      connect(cb, &QCheckBox::toggled, this, c.slot);

  for (const char *name : kTimeChecks)
    if (QCheckBox *cb = find<QCheckBox>(name))
      connect(cb, &QCheckBox::toggled, this, &GenericLightDialog::onTimeCheck);

  const char *fields[] = {"IDC_GENERIC_LIGHT_DISTANCE", "IDC_GENERIC_PULSE_TIME", "IDC_GENERIC_RED_LIGHT",
                          "IDC_GENERIC_GREEN_LIGHT", "IDC_GENERIC_BLUE_LIGHT", "IDC_GENERIC_RED_LIGHT2",
                          "IDC_GENERIC_GREEN_LIGHT2", "IDC_GENERIC_BLUE_LIGHT2", "IDC_FLICKER_SLIGHTLY_EDIT",
                          "IDC_FOV_EDIT"};
  for (const char *name : fields)
    if (QLineEdit *edit = find<QLineEdit>(name))
      connect(edit, &QLineEdit::editingFinished, this, &GenericLightDialog::onFieldEdited);

  updateDialog();
}

GenericLightDialog::~GenericLightDialog() = default;

void GenericLightDialog::setFlag(int32_t flag, const char *checkName, bool checked) {
  if (checked)
    m_lightinfo->flags |= flag;
  else
    m_lightinfo->flags &= ~flag;
}

void GenericLightDialog::updateDialog() {
  light_info *li = m_lightinfo;
  if (QCheckBox *cb = find<QCheckBox>("IDC_DIRECTIONAL"))
    cb->setChecked(li->flags & OLF_DIRECTIONAL);
  if (QCheckBox *cb = find<QCheckBox>("IDC_NO_SPECULAR_CHECK"))
    cb->setChecked(li->flags & OLF_NO_SPECULARITY);
  if (QCheckBox *cb = find<QCheckBox>("IDC_PULSE"))
    cb->setChecked(li->flags & OLF_PULSE);
  if (QCheckBox *cb = find<QCheckBox>("IDC_PULSE_TO_SECOND"))
    cb->setChecked(li->flags & OLF_PULSE_TO_SECOND);
  if (QCheckBox *cb = find<QCheckBox>("IDC_FLICKER_SLIGHTLY_CHECK"))
    cb->setChecked(li->flags & OLF_FLICKER_SLIGHTLY);

  if (QRadioButton *rb = find<QRadioButton>("IDC_FLICKER_RADIO"))
    rb->setChecked(li->flags & OLF_FLICKERING);
  if (QRadioButton *rb = find<QRadioButton>("IDC_USE_TIMEBITS_RADIO"))
    rb->setChecked(li->flags & OLF_TIMEBITS);
  if (QRadioButton *rb = find<QRadioButton>("IDC_ALWAYS_ON_RADIO"))
    rb->setChecked(!(li->flags & (OLF_FLICKERING | OLF_TIMEBITS)));

  for (int i = 0; i < 8; i++)
    if (QCheckBox *cb = find<QCheckBox>(kTimeChecks[i]))
      cb->setChecked((li->timebits & (1 << i)) != 0);

  const int rt = li->lighting_render_type;
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_STATIC"))
    rb->setChecked(rt == LRT_STATIC);
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_GOURAUD"))
    rb->setChecked(rt == LRT_GOURAUD);
  if (QRadioButton *rb = find<QRadioButton>("IDC_RENDER_LIGHTMAPS"))
    rb->setChecked(rt == LRT_LIGHTMAPS);

  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_LIGHT_DISTANCE"))
    e->setText(QString::number(li->light_distance));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_PULSE_TIME"))
    e->setText(QString::number(li->time_interval));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_RED_LIGHT"))
    e->setText(QString::number(li->red_light1));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_GREEN_LIGHT"))
    e->setText(QString::number(li->green_light1));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_BLUE_LIGHT"))
    e->setText(QString::number(li->blue_light1));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_RED_LIGHT2"))
    e->setText(QString::number(li->red_light2));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_GREEN_LIGHT2"))
    e->setText(QString::number(li->green_light2));
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_BLUE_LIGHT2"))
    e->setText(QString::number(li->blue_light2));
  if (QLineEdit *e = find<QLineEdit>("IDC_FLICKER_SLIGHTLY_EDIT"))
    e->setText(QString::number(li->flicker_distance));
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
  for (int i = 0; i < 8; i++)
    if (QCheckBox *cb = find<QCheckBox>(kTimeChecks[i]))
      if (cb->isChecked())
        bits |= (1 << i);
  m_lightinfo->timebits = bits;
}

void GenericLightDialog::onRenderStatic() { m_lightinfo->lighting_render_type = LRT_STATIC; }
void GenericLightDialog::onRenderGouraud() { m_lightinfo->lighting_render_type = LRT_GOURAUD; }
void GenericLightDialog::onRenderLightmaps() { m_lightinfo->lighting_render_type = LRT_LIGHTMAPS; }

void GenericLightDialog::onFieldEdited() {
  light_info *li = m_lightinfo;
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_LIGHT_DISTANCE"))
    li->light_distance = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_PULSE_TIME"))
    li->time_interval = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_RED_LIGHT"))
    li->red_light1 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_GREEN_LIGHT"))
    li->green_light1 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_BLUE_LIGHT"))
    li->blue_light1 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_RED_LIGHT2"))
    li->red_light2 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_GREEN_LIGHT2"))
    li->green_light2 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_GENERIC_BLUE_LIGHT2"))
    li->blue_light2 = e->text().toFloat();
  if (QLineEdit *e = find<QLineEdit>("IDC_FLICKER_SLIGHTLY_EDIT"))
    li->flicker_distance = e->text().toFloat();
}

}
