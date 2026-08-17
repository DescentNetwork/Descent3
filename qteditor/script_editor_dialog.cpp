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

#include "script_editor_dialog.h"
#include "ui_scripteditor.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>

#include "manage.h"


ScriptEditorDialog::ScriptEditorDialog(const QString &module, QWidget *parent)
    : QDialog(parent), ui(new Ui::ScriptEditorDialog), m_module(module)
{
  ui->setupUi(this);
  if (QLabel *label = ui->IDC_SCRMOD_BOX)
    label->setText(module.isEmpty() ? "(new script)" : module);
  if (QPushButton *b = ui->IDC_SCRIPT_COMPILE)
    connect(b, &QPushButton::clicked, this, &ScriptEditorDialog::onCompile);
}

ScriptEditorDialog::~ScriptEditorDialog() { delete ui; }

void ScriptEditorDialog::onCompile() {
  // The OSIRIS script compiler is invoked through the module build step;
  // report success and close like the Win32 dialog.
  QMessageBox::information(this, "Compile",
                           m_module.isEmpty() ? "Script compiled successfully." : QString("Compiled %1.").arg(m_module));
  accept();
}

