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
#include "ui_water_procedural.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>


#include "gametexture.h"


WaterProceduralDialog::WaterProceduralDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::WaterProceduralDialog)
{
  ui->setupUi(this);
  {
    QSlider *s = ui->IDC_WP_THICKNESS_SLIDER;
    s->setRange(0, 255);
    connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
  }
  {
    QSlider *s = ui->IDC_WP_LIGHT_SLIDER;
    s->setRange(0, 255);
    connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
  }
  {
    QSlider *s = ui->IDC_WP_HEIGHT_SLIDER;
    s->setRange(0, 255);
    connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
  }
  {
    QSlider *s = ui->IDC_WP_FREQUENCY_SLIDER;
    s->setRange(0, 255);
    connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
  }
  {
    QSlider *s = ui->IDC_WP_SIZE_SLIDER;
    s->setRange(0, 255);
    connect(s, &QSlider::valueChanged, this, &WaterProceduralDialog::onParamChanged);
  }

  connect(ui->IDC_PROC_EVAL_TIME_EDIT, &QLineEdit::editingFinished, this,
          &WaterProceduralDialog::onFieldEdited);
  connect(ui->IDC_PROC_OSC_TIME_EDIT, &QLineEdit::editingFinished, this,
          &WaterProceduralDialog::onFieldEdited);
  connect(ui->IDC_PROC_OSC_VALUE_EDIT, &QLineEdit::editingFinished, this,
          &WaterProceduralDialog::onFieldEdited);

  {
    QComboBox *combo = ui->IDC_PROCEDURAL_PULLDOWN;
    combo->addItem("Water");
    combo->addItem("Procedural");
  }

  updateDialog();
}

WaterProceduralDialog::~WaterProceduralDialog() { delete ui; }

void WaterProceduralDialog::updateDialog() {
  ui->IDC_WP_THICKNESS_SLIDER->setValue(128);
  ui->IDC_WP_LIGHT_SLIDER->setValue(128);
  ui->IDC_WP_HEIGHT_SLIDER->setValue(64);
  ui->IDC_WP_FREQUENCY_SLIDER->setValue(64);
  ui->IDC_WP_SIZE_SLIDER->setValue(255);
}

void WaterProceduralDialog::onParamChanged() {
  ui->IDC_THICKNESS_TEXT->setText(QString("Thickness: %1").arg(ui->IDC_WP_THICKNESS_SLIDER->value()));
  ui->IDC_LIGHT_TEXT->setText(QString("Light: %1").arg(ui->IDC_WP_LIGHT_SLIDER->value()));
  ui->IDC_HEIGHT_TEXT->setText(QString("Height: %1").arg(ui->IDC_WP_HEIGHT_SLIDER->value()));
  ui->IDC_FREQ_TEXT->setText(QString("Freq: %1").arg(ui->IDC_WP_FREQUENCY_SLIDER->value()));
  ui->IDC_SIZE_TEXT->setText(QString("Size: %1").arg(ui->IDC_WP_SIZE_SLIDER->value()));
}

void WaterProceduralDialog::onFieldEdited() {
  // Eval/osc times feed the procedural animation; stored on the dialog only.
}

