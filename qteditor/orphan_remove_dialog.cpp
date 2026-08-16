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

#include "orphan_remove_dialog.h"

#include <QLabel>
#include <QListWidget>
#include <QMessageBox>

#include "objinfo.h"
#include "object.h"

namespace QtEditor {

OrphanRemoveDialog::OrphanRemoveDialog(QWidget *parent) : Dialog(":/ui/orphan.ui", parent), m_list(nullptr) {
  m_list = find<QListWidget>("IDC_LIST");
  if (m_list != nullptr) {
    m_list->addItem("Scanning for orphan objects...");
    m_list->clear();
    // An orphan is an object whose id no longer maps to a valid object type.
    int orphans = 0;
    for (int i = 0; i <= Highest_object_index; i++) {
      object *obj = &Objects[i];
      if (obj->type == OBJ_NONE)
        continue;
      if (obj->id < 0 || obj->id >= MAX_OBJECT_IDS || Object_info[obj->id].type == OBJ_NONE) {
        m_list->addItem(QString("Object %1: orphaned id %2").arg(i).arg(obj->id));
        orphans++;
      }
    }
    if (orphans == 0)
      m_list->addItem("No orphans found.");
    if (QLabel *label = find<QLabel>("IDC_STATUS"))
      label->setText(QString("Scanned %1 objects; %2 orphans.").arg(Highest_object_index + 1).arg(orphans));
  }
}

OrphanRemoveDialog::~OrphanRemoveDialog() = default;

void OrphanRemoveDialog::onScan() {
  QMessageBox::information(this, "Orphan scan", "Orphan scan complete.");
}

}
