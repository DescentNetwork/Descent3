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

#include "water_procedural_dialog.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>

#include "d3edit.h"
#include "gametexture.h"

namespace QtEditor {

WaterProceduralDialog::WaterProceduralDialog(QWidget *parent) : Dialog(":/ui/water_procedural.ui", parent) {
  const char *sliders[] = {"IDC_WP_THICKNESS_SLIDER", "IDC_WP_LIGHT_SLIDER", "IDC_WP_HEIGHT_SLIDER",
                           "IDC_WP_FREQUENCY_SLIDER", "IDC_WP_SIZE_SLIDER"};
  for (const char *name : sliders)
    if (QSlider *s = find<QSlider>(name)) {
      s->setRange(0, 255);
      connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
    }

  const char *fields[] = {"IDC_PROC_EVAL_TIME_EDIT", "IDC_PROC_OSC_TIME_EDIT", "IDC_PROC_OSC_VALUE_EDIT"};
  for (const char *name : fields)
    if (QLineEdit *e = find<QLineEdit>(name))
      connect(e, &QLineEdit::editingFinished, this, &WaterProceduralDialog::onFieldEdited);

  if (QComboBox *combo = find<QComboBox>("IDC_PROCEDURAL_PULLDOWN")) {
    combo->addItem("Water");
    combo->addItem("Procedural");
  }

  updateDialog();
}

WaterProceduralDialog::~WaterProceduralDialog() = default;

void WaterProceduralDialog::updateDialog() {
  if (QSlider *s = find<QSlider>("IDC_WP_THICKNESS_SLIDER"))
    s->setValue(128);
  if (QSlider *s = find<QSlider>("IDC_WP_LIGHT_SLIDER"))
    s->setValue(128);
  if (QSlider *s = find<QSlider>("IDC_WP_HEIGHT_SLIDER"))
    s->setValue(64);
  if (QSlider *s = find<QSlider>("IDC_WP_FREQUENCY_SLIDER"))
    s->setValue(64);
  if (QSlider *s = find<QSlider>("IDC_WP_SIZE_SLIDER"))
    s->setValue(255);
}

void WaterProceduralDialog::onParamChanged() {
  if (QLabel *l = find<QLabel>("IDC_THICKNESS_TEXT"))
    l->setText(QString("Thickness: %1").arg(find<QSlider>("IDC_WP_THICKNESS_SLIDER")->value()));
  if (QLabel *l = find<QLabel>("IDC_LIGHT_TEXT"))
    l->setText(QString("Light: %1").arg(find<QSlider>("IDC_WP_LIGHT_SLIDER")->value()));
  if (QLabel *l = find<QLabel>("IDC_HEIGHT_TEXT"))
    l->setText(QString("Height: %1").arg(find<QSlider>("IDC_WP_HEIGHT_SLIDER")->value()));
  if (QLabel *l = find<QLabel>("IDC_FREQ_TEXT"))
    l->setText(QString("Freq: %1").arg(find<QSlider>("IDC_WP_FREQUENCY_SLIDER")->value()));
  if (QLabel *l = find<QLabel>("IDC_SIZE_TEXT"))
    l->setText(QString("Size: %1").arg(find<QSlider>("IDC_WP_SIZE_SLIDER")->value()));
}

void WaterProceduralDialog::onFieldEdited() {
  // Eval/osc times feed the procedural animation; stored on the dialog only.
}

}
