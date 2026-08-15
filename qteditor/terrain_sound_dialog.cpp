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

namespace QtEditor {

TerrainSoundDialog::TerrainSoundDialog(QWidget *parent) : Dialog(":/ui/terrain_sound_dialog.ui", parent) {
  for (int b = 0; b < NUM_TERRAIN_SOUND_BANDS; b++)
    m_bands[b] = Terrain_sound_bands[b];

  if (QPushButton *ok = find<QPushButton>("IDOK")) {
    disconnect(ok, &QPushButton::clicked, m_dialog, &QDialog::accept);
    connect(ok, &QPushButton::clicked, this, &TerrainSoundDialog::onOk);
  }
  if (QPushButton *next = find<QPushButton>("IDC_TERRAIN_SOUND_NEXT"))
    connect(next, &QPushButton::clicked, this, &TerrainSoundDialog::onNext);
  if (QPushButton *prev = find<QPushButton>("IDC_TERRAIN_SOUND_PREV"))
    connect(prev, &QPushButton::clicked, this, &TerrainSoundDialog::onPrev);
  if (QComboBox *combo = find<QComboBox>("IDC_TERRAIN_SOUND_COMBO"))
    connect(combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &TerrainSoundDialog::onSoundChanged);

  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_ALT"))
    edit->setValidator(new QIntValidator(0, 255, edit));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_ALT"))
    edit->setValidator(new QIntValidator(0, 255, edit));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_VOLUME"))
    edit->setValidator(new QDoubleValidator(0.0, 1.0, 2, edit));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_VOLUME"))
    edit->setValidator(new QDoubleValidator(0.0, 1.0, 2, edit));

  m_current = 0;
  copyToControls();
  updateDialog();
}

TerrainSoundDialog::~TerrainSoundDialog() = default;

void TerrainSoundDialog::updateDialog() {
  const bool enabled = m_bands[m_current].sound_index != -1;
  if (QWidget *w = find<QWidget>("IDC_TERRAIN_SOUND_LOW_ALT"))
    w->setEnabled(enabled);
  if (QWidget *w = find<QWidget>("IDC_TERRAIN_SOUND_HIGH_ALT"))
    w->setEnabled(enabled);
  if (QWidget *w = find<QWidget>("IDC_TERRAIN_SOUND_LOW_VOLUME"))
    w->setEnabled(enabled);
  if (QWidget *w = find<QWidget>("IDC_TERRAIN_SOUND_HIGH_VOLUME"))
    w->setEnabled(enabled);

  if (QLabel *label = find<QLabel>("IDC_TERRAIN_SOUND_BAND_TEXT"))
    label->setText(QString("Band %1:").arg(m_current));

  if (QPushButton *next = find<QPushButton>("IDC_TERRAIN_SOUND_NEXT"))
    next->setEnabled(m_current < NUM_TERRAIN_SOUND_BANDS - 1);
  if (QPushButton *prev = find<QPushButton>("IDC_TERRAIN_SOUND_PREV"))
    prev->setEnabled(m_current > 0);
}

void TerrainSoundDialog::copyToControls() {
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_ALT"))
    edit->setText(QString::number(m_bands[m_current].low_alt));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_ALT"))
    edit->setText(QString::number(m_bands[m_current].high_alt));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_VOLUME"))
    edit->setText(QString::number(m_bands[m_current].low_volume));
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_VOLUME"))
    edit->setText(QString::number(m_bands[m_current].high_volume));
  if (QComboBox *combo = find<QComboBox>("IDC_TERRAIN_SOUND_COMBO"))
    setSoundComboSelected(combo, m_bands[m_current].sound_index);
}

bool TerrainSoundDialog::copyFromControls() {
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_ALT"))
    m_bands[m_current].low_alt = edit->text().toInt();
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_ALT"))
    m_bands[m_current].high_alt = edit->text().toInt();
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_LOW_VOLUME"))
    m_bands[m_current].low_volume = edit->text().toFloat();
  if (QLineEdit *edit = find<QLineEdit>("IDC_TERRAIN_SOUND_HIGH_VOLUME"))
    m_bands[m_current].high_volume = edit->text().toFloat();
  if (QComboBox *combo = find<QComboBox>("IDC_TERRAIN_SOUND_COMBO"))
    m_bands[m_current].sound_index = soundComboSelected(combo);

  if (m_bands[m_current].low_alt > m_bands[m_current].high_alt) {
    QMessageBox::warning(m_dialog, "Terrain Sound", "High elevation must be greater than or equal to low elevation.");
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

  World_changed = 1;
  m_dialog->accept();
}

}
