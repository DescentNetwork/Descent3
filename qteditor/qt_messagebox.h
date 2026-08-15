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
// pserror.h also transitively pulls into some Qt editor sources (gamepath.h
// includes it). The silent pserror.h declarations of `void OutrageMessageBox
// (const char*, ...)` / `int OutrageMessageBox(int, const char*, ...)` are
// captured by the macro below and routed into QtEditor::outrageMessageBox,
// whose inline definitions match the pserror.h signatures so the two are
// compatible. The MBOX_* flags below are guarded so they survive either
// order.

#include <cstdarg>
#include <cstdio>

#include <QMessageBox>
#include <QString>

namespace QtEditor {

#ifndef MBOX_OK
#define MBOX_OK 1
#endif
#ifndef MBOX_YESNO
#define MBOX_YESNO 2
#endif
#ifndef MBOX_YESNOCANCEL
#define MBOX_YESNOCANCEL 3
#endif
#ifndef MBOX_ABORTRETRYIGNORE
#define MBOX_ABORTRETRYIGNORE 4
#endif

inline void outrageMessageBox(const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  QMessageBox::information(nullptr, "Descent 3 Editor", QString::fromUtf8(buf));
}

inline int outrageMessageBox(int type, const char *fmt, ...) {
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  if (type == MBOX_YESNO) {
    QMessageBox box(QMessageBox::Information, "Descent 3 Editor", QString::fromUtf8(buf),
                    QMessageBox::Yes | QMessageBox::No);
    return box.exec() == QMessageBox::Yes ? 1 : 0;
  }
  QMessageBox::information(nullptr, "Descent 3 Editor", QString::fromUtf8(buf));
  return 0;
}

} // namespace QtEditor

// Redirect OutrageMessageBox() calls into QtEditor::outrageMessageBox(). The
// strong override in qt_messagebox.cpp also defines the global namespace
// OutrageMessageBox symbols (which the D3 core resolves at link time thanks
// to the weak attribute in ddebug/error.cpp). This macro only affects tokens
// expanded after the include - so don't include this header from the .cpp
// file that defines the global overrides.
#ifndef QTEDITOR_NO_OUTRAGE_REDIRECT
#define OutrageMessageBox(...) QtEditor::outrageMessageBox(__VA_ARGS__)
#endif
