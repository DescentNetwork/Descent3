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

#include "multicheck_dialog.h"
#include "ui_multicheck.h"

#include <QLabel>
#include <QListWidget>


MultiCheckDialog::MultiCheckDialog(const QString &message, const QStringList &items, const QStringList &checked,
                                   QWidget *parent)
    : QDialog(parent), ui(new Ui::MulticheckDialog), m_list(nullptr)
{
  ui->setupUi(this);
  if (QLabel *label = ui->IDC_DIRECTIONS_TEXT)
    label->setText(message);
  m_list = ui->IDC_MULTICHECKIN_LISTBOX;
  if (m_list != nullptr) {
    m_list->addItems(items);
    for (int i = 0; i < m_list->count(); i++)
      if (checked.contains(m_list->item(i)->text()))
        m_list->item(i)->setSelected(true);
    m_list->setSelectionMode(QAbstractItemView::MultiSelection);
  }
}

MultiCheckDialog::~MultiCheckDialog() { delete ui; }

QStringList MultiCheckDialog::checkedItems() const {
  QStringList out;
  if (m_list != nullptr)
    for (QListWidgetItem *item : m_list->selectedItems())
      out << item->text();
  return out;
}

