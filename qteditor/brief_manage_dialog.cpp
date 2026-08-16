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

#include "brief_manage_dialog.h"

#include <QInputDialog>
#include <QListWidget>
#include <QPushButton>


namespace QtEditor {

BriefManageDialog::BriefManageDialog(QWidget *parent) : Dialog(":/ui/brief_manage.ui", parent), m_list(nullptr) {
  m_list = find<QListWidget>("IDC_LIST");
  if (m_list != nullptr) {
    // Briefing screens are managed by the briefing editor; start with a
    // default screen entry.
    m_list->addItem("Screen 1");
  }
  if (QPushButton *b = find<QPushButton>("IDC_ADD"))
    connect(b, &QPushButton::clicked, this, &BriefManageDialog::onAdd);
}

BriefManageDialog::~BriefManageDialog() = default;

void BriefManageDialog::onAdd() {
  if (m_list != nullptr)
    m_list->addItem(QString("Screen %1").arg(m_list->count() + 1));
}

}
