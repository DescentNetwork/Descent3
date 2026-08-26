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

#include "dallas_sound_dialog.h"
#include "ui_dallas_sound_dialog.h"

#include <QListWidget>
#include <QPushButton>

#include "sndlib/hlsoundlib.h"
#include "ssl_lib.h"


DallasSoundDialog::DallasSoundDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DallasSoundDialog), m_list(nullptr)
{
  ui->setupUi(this);
  m_list = ui->IDC_SOUND_LIST;
  if (m_list != nullptr) {
    for (int i = 0; i < MAX_SOUNDS; i++)
      if (Sounds[i].used)
        m_list->addItem(Sounds[i].name);
  }
  if (QPushButton *b = ui->IDC_PLAY_SOUND_BUTTON)
    connect(b, &QPushButton::clicked, this, &DallasSoundDialog::onPlay);
  if (QPushButton *b = ui->IDC_STOP_SOUNDS_BUTTON)
    connect(b, &QPushButton::clicked, this, &DallasSoundDialog::onStop);
}

DallasSoundDialog::~DallasSoundDialog() { delete ui; }

int DallasSoundDialog::selectedSound() const {
  if (m_list == nullptr || m_list->currentRow() < 0)
    return -1;
  int usedCount = 0;
  for (int i = 0; i < MAX_SOUNDS; i++) {
    if (!Sounds[i].used)
      continue;
    if (usedCount == m_list->currentRow())
      return i;
    usedCount++;
  }
  return -1;
}

void DallasSoundDialog::onPlay() {
  const int s = selectedSound();
  if (s < 0)
    return;
  Sound_system.BeginSoundFrame();
  Sound_system.Play2dSound(s);
  Sound_system.EndSoundFrame();
}

void DallasSoundDialog::onStop() {
  Sound_system.BeginSoundFrame();
  Sound_system.StopAllSounds();
  Sound_system.EndSoundFrame();
}

