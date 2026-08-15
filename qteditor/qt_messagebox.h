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

#pragma once

// The D3 core's OutrageMessageBox() routes through Debug_ErrorBox() which on
// Linux calls SDL_TriggerBreakpoint() - unusable inside the Qt editor. Provide
// a Qt equivalent and redirect OutrageMessageBox() calls to it.
//
// This header must be included before pserror.h so that pserror's
// OutrageMessageBox() declarations expand to the QtEditor overloads.

#include <cstdarg>
#include <cstdio>

#include <QMessageBox>
#include <QString>

namespace QtEditor {

inline int outrageMessageBox(const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  QMessageBox::information(nullptr, "Descent 3 Editor", QString::fromUtf8(buf));
  return 0;
}

inline int outrageMessageBox(int type, const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  if (type != 0) {
    QMessageBox box(QMessageBox::Information, "Descent 3 Editor", QString::fromUtf8(buf),
                    QMessageBox::Yes | QMessageBox::No);
    return box.exec() == QMessageBox::Yes ? 1 : 0;
  }
  QMessageBox::information(nullptr, "Descent 3 Editor", QString::fromUtf8(buf));
  return 0;
}

} // namespace QtEditor

#define OutrageMessageBox(...) QtEditor::outrageMessageBox(__VA_ARGS__)
