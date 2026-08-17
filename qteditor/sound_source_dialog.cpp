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

#include "sound_source_dialog.h"
#include "ui_soundsource_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "object_external_struct.h"
#include "sound_combo.h"


SoundSourceDialog::SoundSourceDialog(soundsource_info_s *data, QWidget *parent)
    : QDialog(parent), ui(new Ui::SoundSourceDialog), m_data(data)
{
  ui->setupUi(this);

  connect(this, &QDialog::accept, this, &SoundSourceDialog::onOk);

  if (QPushButton *select = ui->IDC_SELECT)
    connect(select, &QPushButton::clicked, this, &SoundSourceDialog::onSelect);
  if (QLineEdit *edit = ui->IDC_VOLUME)
    edit->setValidator(new QDoubleValidator(0.0, 1.0, 2, edit));

  populateSoundCombo(ui->IDC_SOUND_COMBO, m_data->sound_index);
  if (QLineEdit *edit = ui->IDC_VOLUME)
    edit->setText(QString::number(m_data->volume));
}

SoundSourceDialog::~SoundSourceDialog() { delete ui; }

void SoundSourceDialog::onSelect() {
  // The MFC original opens CDallasSoundDlg; until that tree is ported, offer
  // the same game-sound list in a compact picker.
  QComboBox *combo = ui->IDC_SOUND_COMBO;
  if (combo == nullptr)
    return;

  QDialog picker(this);
  picker.setWindowTitle("Select Sound");
  auto *layout = new QVBoxLayout(&picker);
  auto *pickCombo = new QComboBox(&picker);
  populateSoundCombo(pickCombo, soundComboSelected(combo));
  layout->addWidget(pickCombo);
  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &picker);
  layout->addWidget(buttons);
  connect(buttons, &QDialogButtonBox::accepted, &picker, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, &picker, &QDialog::reject);

  if (picker.exec() == QDialog::Accepted)
    setSoundComboSelected(combo, soundComboSelected(pickCombo));
}

void SoundSourceDialog::onOk() {
  m_data->volume = ui->IDC_VOLUME->text().toFloat();
  m_data->sound_index = soundComboSelected(ui->IDC_SOUND_COMBO);
  accept();
}

