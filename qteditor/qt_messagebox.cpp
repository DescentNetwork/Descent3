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

// Strong override of the D3 core's OutrageMessageBox(). The D3 core's
// implementation in ddebug/error.cpp is marked weak so the strong override
// here wins at link time. The override routes message boxes through QtEditor
// so the dialogs use Qt instead of SDL_TriggerBreakpoint() (which the Linux
// Debug_MessageBox path uses).

#include <cstdarg>

#define QTEDITOR_NO_OUTRAGE_REDIRECT
#include "qt_messagebox.h"
#undef QTEDITOR_NO_OUTRAGE_REDIRECT

namespace {

void formatInto(std::va_list ap, char *buf, std::size_t cap, const char *fmt) {
  std::vsnprintf(buf, cap, fmt, ap);
}

} // namespace

// Strong override of the weak symbols in ddebug/error.cpp. Not extern "C" so
// the two-argument overload is allowed in C++.
void OutrageMessageBox(const char *str, ...) {
  char buf[2048];
  std::va_list ap;
  va_start(ap, str);
  formatInto(ap, buf, sizeof(buf), str);
  va_end(ap);
  outrageMessageBox("%s", buf);
}

int OutrageMessageBox(int type, const char *str, ...) {
  char buf[2048];
  std::va_list ap;
  va_start(ap, str);
  formatInto(ap, buf, sizeof(buf), str);
  va_end(ap);
  return outrageMessageBox(type, "%s", buf);
}
