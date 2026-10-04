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

// Logging backend for the LOG_* macros (activates when ENABLE_LOGGER is ON,
// which defines LOGGER and compiles this file).  Each LOG_* call site passes a
// printf-style format string; the message is routed to the matching Qt logging
// facility (qDebug/qInfo/qWarning/qCritical/qFatal).

#include "log.h"

#include <QtGlobal>
#include <QDebug>
#include <QString>

#include <cstdarg>
#include <cstdio>
#include <vector>

void InitLog(LogSeverity /*log_level*/, bool /*enable_filelog*/, bool /*enable_win_console*/) {}

static void WriteLogVa(LogSeverity log_level, const char *fmt, va_list args) {
  va_list copy;
  va_copy(copy, args);
  int needed = std::vsnprintf(nullptr, 0, fmt, copy);
  va_end(copy);
  if (needed < 0)
    return;

  std::vector<char> buf(static_cast<size_t>(needed) + 1);
  std::vsnprintf(buf.data(), buf.size(), fmt, args);
  const QString message = QString::fromUtf8(buf.data(), needed);

  switch (log_level) {
  case LogSeverity::verbose:
    qDebug().noquote() << message;
    break;
  case LogSeverity::debug:
    qDebug().noquote() << message;
    break;
  case LogSeverity::info:
    qInfo().noquote() << message;
    break;
  case LogSeverity::warning:
    qWarning().noquote() << message;
    break;
  case LogSeverity::error:
    qCritical().noquote() << message;
    break;
  case LogSeverity::fatal:
    qFatal("%s", qPrintable(message));
    break;
  }
}

void WriteLog(LogSeverity log_level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  WriteLogVa(log_level, fmt, args);
  va_end(args);
}