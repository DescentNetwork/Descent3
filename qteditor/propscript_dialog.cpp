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
#include "ui_propscript.h"

#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include "manage.h"


PropScriptDialog::PropScriptDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PropScriptDialog), m_list(nullptr)
{
  ui->setupUi(this);
  m_list = ui->IDC_SCRIPTSEL_LISTBOX;
  if (m_list != nullptr) {
    // Scripts are loaded through the OSIRIS module system; list compiled
    // modules present in the local scripts directory.
    QDir dir(QString::fromStdString(LocalScriptDir.string()));
    const QStringList names = dir.entryList(QStringList() << "*.dll", QDir::Files);
    for (const QString &n : names)
      m_list->addItem(QFileInfo(n).completeBaseName());
  }
  if (QPushButton *b = ui->IDC_PARAMETERS)
    connect(b, &QPushButton::clicked, this, &PropScriptDialog::onParameters);
  if (QPushButton *b = ui->IDC_SCRIPTSEL_SCRIPT_WIZ)
    connect(b, &QPushButton::clicked, this, &PropScriptDialog::onScriptWizard);
}

PropScriptDialog::~PropScriptDialog() { delete ui; }

void PropScriptDialog::onParameters() {
  QMessageBox::information(this, "Script parameters", "The script parameter editor has not been ported yet.");
}

void PropScriptDialog::onScriptWizard() {
  QMessageBox::information(this, "Script wizard", "The script wizard has not been ported yet.");
}

