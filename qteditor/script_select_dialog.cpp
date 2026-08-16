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

#include "script_select_dialog.h"

#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "manage.h"

namespace QtEditor {

ScriptSelectDialog::ScriptSelectDialog(QWidget *parent) : Dialog(":/ui/scriptselect.ui", parent), m_list(nullptr) {
  m_list = find<QListWidget>("IDC_SCRIPTSEL_LISTBOX");
  populate();
  if (QPushButton *b = find<QPushButton>("IDC_SCRIPTSEL_ADD"))
    connect(b, &QPushButton::clicked, this, &ScriptSelectDialog::onAdd);
  if (QPushButton *b = find<QPushButton>("IDC_EDITSCRIPT"))
    connect(b, &QPushButton::clicked, this, &ScriptSelectDialog::onEdit);
  if (QPushButton *b = find<QPushButton>("IDC_PARAMETERS"))
    connect(b, &QPushButton::clicked, this, &ScriptSelectDialog::onParameters);
}

ScriptSelectDialog::~ScriptSelectDialog() = default;

void ScriptSelectDialog::populate() {
  if (m_list == nullptr)
    return;
  m_list->clear();
  QDir dir(LocalScriptDir);
  const QStringList names = dir.entryList(QStringList() << "*.dll", QDir::Files);
  for (const QString &n : names)
    m_list->addItem(QFileInfo(n).completeBaseName());
}

QString ScriptSelectDialog::scriptName() const {
  return m_list != nullptr && m_list->currentItem() != nullptr ? m_list->currentItem()->text() : QString();
}

void ScriptSelectDialog::onAdd() {
  QMessageBox::information(this, "Add script", "Use a script wizard to create a new script module.");
}

void ScriptSelectDialog::onEdit() {
  if (scriptName().isEmpty())
    return;
  QMessageBox::information(this, "Edit script", "Open the script editor for " + scriptName());
}

void ScriptSelectDialog::onParameters() {
  if (scriptName().isEmpty())
    return;
  QMessageBox::information(this, "Parameters", "Edit parameters for " + scriptName());
}

}
