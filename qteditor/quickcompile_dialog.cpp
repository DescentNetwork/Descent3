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

#include <QTextEdit>

namespace QtEditor {

QuickCompileDialog::QuickCompileDialog(const QString &scriptName, QWidget *parent)
    : Dialog(":/ui/quickcompile.ui", parent), m_result(0) {
  if (QTextEdit *text = find<QTextEdit>("IDC_TEXT")) {
    text->setReadOnly(true);
    if (scriptName.isEmpty())
      text->setPlainText("No script module specified.");
    else
      text->setPlainText(QString("Compiling %1...\n\n(Compiler integration pending.)").arg(scriptName));
  }
}

QuickCompileDialog::~QuickCompileDialog() = default;

}
