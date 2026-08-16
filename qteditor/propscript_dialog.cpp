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

#include "propscript_dialog.h"

#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "manage.h"

namespace QtEditor {

PropScriptDialog::PropScriptDialog(QWidget *parent) : Dialog(":/ui/propscript.ui", parent), m_list(nullptr) {
  m_list = find<QListWidget>("IDC_SCRIPTSEL_LISTBOX");
  if (m_list != nullptr) {
    // Scripts are loaded through the OSIRIS module system; list compiled
    // modules present in the local scripts directory.
    QDir dir(LocalScriptDir);
    const QStringList names = dir.entryList(QStringList() << "*.dll", QDir::Files);
    for (const QString &n : names)
      m_list->addItem(QFileInfo(n).completeBaseName());
  }
  if (QPushButton *b = find<QPushButton>("IDC_PARAMETERS"))
    connect(b, &QPushButton::clicked, this, &PropScriptDialog::onParameters);
  if (QPushButton *b = find<QPushButton>("IDC_SCRIPTSEL_SCRIPT_WIZ"))
    connect(b, &QPushButton::clicked, this, &PropScriptDialog::onScriptWizard);
}

PropScriptDialog::~PropScriptDialog() = default;

void PropScriptDialog::onParameters() {
  QMessageBox::information(m_dialog, "Script parameters", "The script parameter editor has not been ported yet.");
}

void PropScriptDialog::onScriptWizard() {
  QMessageBox::information(m_dialog, "Script wizard", "The script wizard has not been ported yet.");
}

}
