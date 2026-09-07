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

#include "terrain_sound_dialog.h"
#include "ui_terrain_sound_dialog.h"

#include <QComboBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "d3edit.h"
#include "game.h"
#include "sound_combo.h"


TerrainSoundDialog::TerrainSoundDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TerrainSoundDialog)
{
  ui->setupUi(this);
  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++)
    m_bands[b] = Terrain_sound_bands[b];

  connect(this, &QDialog::accept, this, &TerrainSoundDialog::onOk);
  connect(ui->IDC_TERRAIN_SOUND_NEXT, &QPushButton::clicked, this, &TerrainSoundDialog::onNext);
  connect(ui->IDC_TERRAIN_SOUND_PREV, &QPushButton::clicked, this, &TerrainSoundDialog::onPrev);
  connect(ui->IDC_TERRAIN_SOUND_COMBO, qOverload<int>(&QComboBox::currentIndexChanged), this, &TerrainSoundDialog::onSoundChanged);

  {
    QLineEdit *edit = ui->IDC_TERRAIN_SOUND_LOW_ALT;
    edit->setValidator(new QIntValidator(0, 255, edit));
  }
  {
    QLineEdit *edit = ui->IDC_TERRAIN_SOUND_HIGH_ALT;
    edit->setValidator(new QIntValidator(0, 255, edit));
  }
  {
    QLineEdit *edit = ui->IDC_TERRAIN_SOUND_LOW_VOLUME;
    edit->setValidator(new QDoubleValidator(0.0, 1.0, 2, edit));
  }
  {
    QLineEdit *edit = ui->IDC_TERRAIN_SOUND_HIGH_VOLUME;
    edit->setValidator(new QDoubleValidator(0.0, 1.0, 2, edit));
  }

  m_current = 0;
  copyToControls();
  updateDialog();
}

TerrainSoundDialog::~TerrainSoundDialog() { delete ui; }

void TerrainSoundDialog::updateDialog() {
  const bool enabled = m_bands[m_current].sound_index != -1;
  ui->IDC_TERRAIN_SOUND_LOW_ALT->setEnabled(enabled);
  ui->IDC_TERRAIN_SOUND_HIGH_ALT->setEnabled(enabled);
  ui->IDC_TERRAIN_SOUND_LOW_VOLUME->setEnabled(enabled);
  ui->IDC_TERRAIN_SOUND_HIGH_VOLUME->setEnabled(enabled);

  ui->IDC_TERRAIN_SOUND_BAND_TEXT->setText(QString("Band %1:").arg(m_current));

  ui->IDC_TERRAIN_SOUND_NEXT->setEnabled(m_current < NUM_TERRAIN_SOUND_BANDS - 1);
  ui->IDC_TERRAIN_SOUND_PREV->setEnabled(m_current > 0);
}

void TerrainSoundDialog::copyToControls() {
  ui->IDC_TERRAIN_SOUND_LOW_ALT->setText(QString::number(m_bands[m_current].low_alt));
  ui->IDC_TERRAIN_SOUND_HIGH_ALT->setText(QString::number(m_bands[m_current].high_alt));
  ui->IDC_TERRAIN_SOUND_LOW_VOLUME->setText(QString::number(m_bands[m_current].low_volume));
  ui->IDC_TERRAIN_SOUND_HIGH_VOLUME->setText(QString::number(m_bands[m_current].high_volume));
  setSoundComboSelected(ui->IDC_TERRAIN_SOUND_COMBO, m_bands[m_current].sound_index);
}

bool TerrainSoundDialog::copyFromControls() {
  m_bands[m_current].low_alt = ui->IDC_TERRAIN_SOUND_LOW_ALT->text().toInt();
  m_bands[m_current].high_alt = ui->IDC_TERRAIN_SOUND_HIGH_ALT->text().toInt();
  m_bands[m_current].low_volume = ui->IDC_TERRAIN_SOUND_LOW_VOLUME->text().toFloat();
  m_bands[m_current].high_volume = ui->IDC_TERRAIN_SOUND_HIGH_VOLUME->text().toFloat();
  m_bands[m_current].sound_index = soundComboSelected(ui->IDC_TERRAIN_SOUND_COMBO);

  if (m_bands[m_current].low_alt > m_bands[m_current].high_alt) {
    QMessageBox::warning(this, "Terrain Sound", "High elevation must be greater than or equal to low elevation.");
    return false;
  }
  return true;
}

void TerrainSoundDialog::onNext() {
  if (m_current >= NUM_TERRAIN_SOUND_BANDS - 1)
    return;
  if (!copyFromControls())
    return;
  m_current++;
  copyToControls();
  updateDialog();
}

void TerrainSoundDialog::onPrev() {
  if (m_current <= 0)
    return;
  if (!copyFromControls())
    return;
  m_current--;
  copyToControls();
  updateDialog();
}

void TerrainSoundDialog::onSoundChanged() {
  copyFromControls();
  updateDialog();
}

void TerrainSoundDialog::onOk() {
  if (!copyFromControls())
    return;

  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++)
    Terrain_sound_bands[b] = m_bands[b];

  World_changed = true;
  accept();
}

