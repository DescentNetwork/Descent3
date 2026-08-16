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

#include "object_properties_dialog.h"

#include <QTabWidget>

#include "objinfo.h"
#include "object.h"
#include "physics_dialog.h"
#include "property_physics_dialog.h"

namespace QtEditor {

ObjectPropertiesDialog::ObjectPropertiesDialog(int objIndex, QWidget *parent)
    : Dialog(":/ui/objectproperties.ui", parent), m_objIndex(objIndex) {
  QTabWidget *tabs = find<QTabWidget>("IDC_PROPERTYTAB");
  if (tabs == nullptr || m_objIndex < 0 || m_objIndex > Highest_object_index)
    return;

  object *obj = &Objects[m_objIndex];
  if (obj->type == OBJ_NONE)
    return;

  // The Win32 dialog hosts the physics and AI editors as tabs. Object
  // instances reference their type's physics; use it when available.
  if (obj->id >= 0 && obj->id < MAX_OBJECT_IDS && Object_info[obj->id].type != OBJ_NONE) {
    PropertyPhysicsDialog *physics = new PropertyPhysicsDialog(&Object_info[obj->id].phys_info, tabs);
    tabs->addTab(physics->handle(), "Physics");
  }
}

ObjectPropertiesDialog::~ObjectPropertiesDialog() = default;

}
