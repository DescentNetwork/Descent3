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

#include "quickcompile_dialog.h"
#include "ui_quickcompile.h"

#include <QTextEdit>

#include "manage.h"
#include "ScriptCompilerAPI.h"

#include <filesystem>


QuickCompileDialog::QuickCompileDialog(const QString &scriptName, QWidget *parent)
    : QDialog(parent), ui(new Ui::QuickCompileDialog), m_result(0)
{
  ui->setupUi(this);
  QTextEdit *text = ui->IDC_TEXT;
  if (text)
    text->setReadOnly(true);

  if (scriptName.isEmpty()) {
    if (text)
      text->setPlainText("No script module specified.");
    return;
  }

  const std::filesystem::path source = LocalScriptDir / (scriptName.toStdString() + ".cpp");
  tCompilerInfo ci;
  ci.source_filename = source.string();
  ci.script_type = ST_LEVEL;
  ci.callback = [text](char *str) {
    if (text)
      text->append(QString::fromUtf8(str));
  };

  const int result = ScriptCompile(&ci);
  m_result = result;

  QString summary;
  switch (result) {
  case CERR_NOERR:
    summary = QString("\nCompiled %1 successfully.").arg(scriptName);
    break;
  case CERR_SOURCENOEXIST:
    summary = QString("\nSource %1 does not exist.").arg(QString::fromStdString(source.string()));
    break;
  case CERR_NOCOMPILERDEFINED:
    summary = "\nNo compiler configured. Configure one in the Script/Level dialog.";
    break;
  case CERR_COMPILERMISSING:
    summary = "\nThe configured compiler could not be found or launched.";
    break;
  default:
    summary = QString("\nCompile returned unknown result %1.").arg(result);
    break;
  }
  if (text)
    text->append(summary);
}

QuickCompileDialog::~QuickCompileDialog() { delete ui; }
