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

// Logging is currently disabled: the LOG_* macros below are no-ops. The call
// sites are preserved as printf-style format strings so that a logging backend
// can be reintroduced later by redefining the macros and implementing InitLog.

enum class LogSeverity {
  verbose,
  debug,
  info,
  warning,
  error,
  fatal
};

/**
 * Initialize logger facility.
 * @param log_level desired log level
 * @param enable_filelog enable logging into Descent.log
 */
#ifdef LOGGER
void InitLog(LogSeverity log_level, bool enable_filelog, bool enable_win_console);
void WriteLog(LogSeverity log_level, const char *fmt, ...);
#define LOG_VERBOSE(...) WriteLog(LogSeverity::verbose, __VA_ARGS__)
#define LOG_DEBUG(...) WriteLog(LogSeverity::debug, __VA_ARGS__)
#define LOG_INFO(...) WriteLog(LogSeverity::info, __VA_ARGS__)
#define LOG_WARNING(...) WriteLog(LogSeverity::warning, __VA_ARGS__)
#define LOG_ERROR(...) WriteLog(LogSeverity::error, __VA_ARGS__)
#define LOG_FATAL(...) WriteLog(LogSeverity::fatal, __VA_ARGS__)
#define LOG_WARNING_IF(cond, ...) if(cond) { WriteLog(LogSeverity::warning, __VA_ARGS__); }
#define LOG_DEBUG_IF(cond, ...) if(cond) { WriteLog(LogSeverity::debug, __VA_ARGS__); }
#else
#define LOG_VERBOSE(...) do{}while(false)
#define LOG_DEBUG(...) do{}while(false)
#define LOG_INFO(...) do{}while(false)
#define LOG_WARNING(...) do{}while(false)
#define LOG_ERROR(...) do{}while(false)
#define LOG_FATAL(...) do{}while(false)
#define LOG_WARNING_IF(cond, ...) do{}while(false)
#define LOG_DEBUG_IF(cond, ...) do{}while(false)
#endif
