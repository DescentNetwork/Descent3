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

#include "object_list_combo.h"

#include <QComboBox>

#include "object.h"
#include "object_external.h"

void populateObjectCombo(QComboBox *combo, int type, int selected_handle) {
  if (combo == nullptr)
    return;

  combo->clear();
  combo->addItem("<none>", OBJECT_HANDLE_NONE);

  for (int i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type == OBJ_NONE)
      continue;
    if (Objects[i].name && (type == OBJ_NONE || Objects[i].type == type)) {
      char str[100];
      snprintf(str, sizeof(str), "%s (%s, %x)", Objects[i].name ? Objects[i].name : "<no name>",
               Object_type_names[Objects[i].type], Objects[i].handle);
      int index = combo->count();
      combo->addItem(str, Objects[i].handle);
      if (Objects[i].handle == selected_handle)
        combo->setCurrentIndex(index);
    }
  }
}

void setObjectComboSelected(QComboBox *combo, int selected_handle) {
  if (combo == nullptr)
    return;
  for (int i = 0; i < combo->count(); i++) {
    if (combo->itemData(i).toInt() == selected_handle) {
      combo->setCurrentIndex(i);
      return;
    }
  }
  combo->setCurrentIndex(0);
}

int objectComboSelected(QComboBox *combo) {
  if (combo == nullptr)
    return OBJECT_HANDLE_NONE;
  return combo->currentData().toInt();
}
