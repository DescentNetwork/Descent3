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
 * @param enable_win_console enable console windows for WIN32 (no-op for POSIX systems)
 */
#ifdef LOGGER
void InitLog(LogSeverity log_level, bool enable_filelog, bool enable_win_console);
#endif


#define LOG_VERBOSE(...) (void)0
#define LOG_DEBUG(...) (void)0
#define LOG_INFO(...) (void)0
#define LOG_WARNING(...) (void)0
#define LOG_ERROR(...) (void)0
#define LOG_FATAL(...) (void)0
#define LOG_WARNING_IF(cond, ...) (void)0
#define LOG_DEBUG_IF(cond, ...) (void)0

