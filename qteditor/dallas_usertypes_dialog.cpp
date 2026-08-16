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

#include "dallas_usertypes_dialog.h"

#include <QInputDialog>
#include <QListWidget>
#include <QPushButton>

namespace QtEditor {

DallasUserTypesDialog::DallasUserTypesDialog(QWidget *parent) : Dialog(":/ui/dallas_usertypes_dialog.ui", parent) {
  m_list = find<QListWidget>("IDC_VALUES_LIST");
  if (QPushButton *b = find<QPushButton>("IDC_ADD_BUTTON"))
    connect(b, &QPushButton::clicked, this, &DallasUserTypesDialog::onAdd);
  if (QPushButton *b = find<QPushButton>("IDC_CHANGE_BUTTON"))
    connect(b, &QPushButton::clicked, this, &DallasUserTypesDialog::onChange);
  if (QPushButton *b = find<QPushButton>("IDC_DELETE_BUTTON"))
    connect(b, &QPushButton::clicked, this, &DallasUserTypesDialog::onDelete);

  updateDialog();
}

DallasUserTypesDialog::~DallasUserTypesDialog() = default;

void DallasUserTypesDialog::updateDialog() {
  if (m_list == nullptr)
    return;
  // The values are edited in memory by the caller; here we just list the
  // current set of named values.
}

void DallasUserTypesDialog::onAdd() {
  bool ok = false;
  const QString name = QInputDialog::getText(m_dialog, "Add value", "Value name:", QLineEdit::Normal, "", &ok);
  if (ok && !name.isEmpty() && m_list != nullptr)
    m_list->addItem(name);
}

void DallasUserTypesDialog::onChange() {
  if (m_list == nullptr || m_list->currentItem() == nullptr)
    return;
  bool ok = false;
  const QString name = QInputDialog::getText(m_dialog, "Change value", "Value name:", QLineEdit::Normal,
                                             m_list->currentItem()->text(), &ok);
  if (ok && !name.isEmpty())
    m_list->currentItem()->setText(name);
}

void DallasUserTypesDialog::onDelete() {
  if (m_list != nullptr && m_list->currentItem() != nullptr)
    delete m_list->takeItem(m_list->currentRow());
}

}
