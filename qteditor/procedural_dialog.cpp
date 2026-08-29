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

#include "procedural_dialog.h"
#include "ui_procedural_dialog.h"

#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>


#include "gametexture.h"


ProceduralDialog::ProceduralDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ProceduralDialog)
{
  ui->setupUi(this);
  if (QComboBox *combo = ui->IDC_PROCEDURAL_PULLDOWN)
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &ProceduralDialog::onProcTypeChanged);

  const char *sliders[] = {"IDC_FP_HEAT_SLIDER", "IDC_SPEED_SLIDER", "IDC_FREQ_SLIDER", "IDC_SIZE_SLIDER"};
  for (const char *name : sliders)
    if (QSlider *s = findChild<QSlider*>(name)) {
      s->setRange(0, 255);
      connect(s, &QSlider::valueChanged, this, &ProceduralDialog::onParamChanged);
    }

  // The Win32 editor lists the procedural types; the proc type data lives in
  // the engine's procedural system which the Qt port does not yet compile in,
  // so list the common types.
  if (QComboBox *combo = ui->IDC_PROCEDURAL_PULLDOWN) {
    combo->addItem("Fire");
    combo->addItem("Water");
    combo->addItem("Line Lightning");
  }

  if (QPushButton *b = ui->IDC_CLEAR_PROCEDURALS) {
    connect(b, &QPushButton::clicked, this, [this]() {
      if (QMessageBox::question(this, "Clear procedurals", "Clear all procedural textures?") ==
          QMessageBox::Yes) {
        for (int i = 0; i < MAX_TEXTURES; i++)
          if (GameTextures[i].used && GameTextures[i].flags.procedural)
            GameTextures[i].flags.procedural = false;
      }
    });
  }

  updateDialog();
}

ProceduralDialog::~ProceduralDialog() { delete ui; }

void ProceduralDialog::updateDialog() {
  if (QSlider *s = ui->IDC_FP_HEAT_SLIDER)
    s->setValue(128);
  if (QSlider *s = ui->IDC_SPEED_SLIDER)
    s->setValue(64);
  if (QSlider *s = ui->IDC_FREQ_SLIDER)
    s->setValue(64);
  if (QSlider *s = ui->IDC_SIZE_SLIDER)
    s->setValue(255);
}

void ProceduralDialog::onProcTypeChanged() {
  // Selecting a proc type updates the preview; the preview is renderer-driven
  // and left for the renderer integration.
}

void ProceduralDialog::onParamChanged() {
  if (QLabel *l = ui->IDC_HEAT_TEXT)
    l->setText(QString("Heat: %1").arg(ui->IDC_FP_HEAT_SLIDER->value()));
  if (QLabel *l = ui->IDC_SPEED_TEXT)
    l->setText(QString("Speed: %1").arg(ui->IDC_SPEED_SLIDER->value()));
  if (QLabel *l = ui->IDC_FREQUENCY_TEXT)
    l->setText(QString("Freq: %1").arg(ui->IDC_FREQ_SLIDER->value()));
  if (QLabel *l = ui->IDC_SIZE_TEXT2)
    l->setText(QString("Size: %1").arg(ui->IDC_SIZE_SLIDER->value()));
}

