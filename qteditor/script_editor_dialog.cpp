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
#include "ScriptCompilerAPI.h"

#include <filesystem>


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
  if (m_module.isEmpty()) {
    QMessageBox::information(this, "Compile", "No script module specified.");
    return;
  }

  const std::filesystem::path source = LocalScriptDir / (m_module.toStdString() + ".cpp");
  tCompilerInfo ci;
  ci.source_filename = source.string();
  ci.script_type = ST_LEVEL;
  ci.callback = nullptr;

  // Capture output into the script view.
  QTextEdit *view = ui->IDC_SCRIPVIEW;
  if (view) {
    ci.callback = [view](char *str) { view->append(QString::fromUtf8(str)); };
  }

  const int result = ScriptCompile(&ci);

  QString text;
  switch (result) {
  case CERR_NOERR:
    text = QString("Compiled %1.").arg(m_module);
    break;
  case CERR_SOURCENOEXIST:
    text = QString("Source %1 does not exist.").arg(QString::fromStdString(source.string()));
    break;
  case CERR_NOCOMPILERDEFINED:
    text = "No compiler configured. Use the Script/Level dialog to configure one.";
    break;
  case CERR_COMPILERMISSING:
    text = "The configured compiler could not be found or launched.";
    break;
  default:
    text = QString("Compile returned unknown result %1.").arg(result);
    break;
  }

  if (result != CERR_NOERR) {
    QMessageBox::warning(this, "Compile", text);
    return;
  }

  if (view)
    view->append(text);
  accept();
}
