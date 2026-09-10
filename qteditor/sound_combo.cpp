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

#include "sound_combo.h"

#include <QComboBox>

#include "ssl_lib.h"


void populateSoundCombo(QComboBox *combo, int selected) {
  if (combo == nullptr)
    return;

  combo->clear();
  combo->addItem("<none>", -1);

  for (int i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used) {
      int index = combo->count();
      combo->addItem(QString::fromStdString(Sounds[i].name), i);
      if (selected == i)
        combo->setCurrentIndex(index);
    }
  }
}

void setSoundComboSelected(QComboBox *combo, int selected) {
  if (combo == nullptr)
    return;
  for (int i = 0; i < combo->count(); i++) {
    if (combo->itemData(i).toInt() == selected)
      combo->setCurrentIndex(i);
  }
}

int soundComboSelected(QComboBox *combo) {
  if (combo == nullptr)
    return -1;
  return combo->currentData().toInt();
}

