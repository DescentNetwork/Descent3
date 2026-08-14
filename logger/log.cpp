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

#ifdef LOGGER
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>

#include "log.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace {

// Most verbose severity that is still logged. Records with a more verbose
// severity are discarded, mirroring plog's maxSeverity filtering.
LogSeverity g_min_severity = LogSeverity::verbose;

// File handle for Descent3.log, opened by InitLog when file logging is enabled.
std::FILE *g_log_file = nullptr;

std::mutex g_log_mutex;

const char *severityToString(LogSeverity severity) {
  switch (severity) {
  case LogSeverity::fatal:
    return "FATAL";
  case LogSeverity::error:
    return "ERROR";
  case LogSeverity::warning:
    return "WARN";
  case LogSeverity::info:
    return "INFO";
  case LogSeverity::debug:
    return "DEBUG";
  case LogSeverity::verbose:
    return "VERB";
  }
  return "NONE";
}

#ifdef _WIN32
HANDLE g_console_handle = nullptr;
WORD g_console_original_attr = 0;

void initConsoleColor() {
  if (g_console_handle != nullptr) {
    return;
  }
  g_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (g_console_handle != nullptr && g_console_handle != INVALID_HANDLE_VALUE) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(g_console_handle, &csbi)) {
      g_console_original_attr = csbi.wAttributes;
    }
  }
}

void setConsoleColor(LogSeverity severity) {
  if (g_console_handle == nullptr) {
    return;
  }
  switch (severity) {
  case LogSeverity::fatal:
    SetConsoleTextAttribute(g_console_handle,
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED); // white on red background
    break;
  case LogSeverity::error:
    SetConsoleTextAttribute(g_console_handle, FOREGROUND_RED | FOREGROUND_INTENSITY | (g_console_original_attr & 0xf0)); // red
    break;
  case LogSeverity::warning:
    SetConsoleTextAttribute(g_console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | (g_console_original_attr & 0xf0)); // yellow
    break;
  case LogSeverity::debug:
  case LogSeverity::verbose:
    SetConsoleTextAttribute(g_console_handle, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | (g_console_original_attr & 0xf0)); // cyan
    break;
  default:
    break;
  }
}

void resetConsoleColor() {
  if (g_console_handle != nullptr) {
    SetConsoleTextAttribute(g_console_handle, g_console_original_attr);
  }
}
#else
const char *severityColor(LogSeverity severity) {
  switch (severity) {
  case LogSeverity::fatal:
    return "\x1B[97m\x1B[41m"; // white on red background
  case LogSeverity::error:
    return "\x1B[91m"; // bright red
  case LogSeverity::warning:
    return "\x1B[93m"; // bright yellow
  case LogSeverity::debug:
  case LogSeverity::verbose:
    return "\x1B[96m"; // bright cyan
  default:
    return "";
  }
}

bool consoleHasColor() { return isatty(fileno(stdout)) != 0; }
#endif

// Formats one log record and writes it to the console (colored by severity
// when attached to a terminal) and to Descent3.log when file logging is on.
// The caller must hold g_log_mutex.
void writeLogLine(LogSeverity severity, const std::string &message) {
  const auto now = std::chrono::system_clock::now();
  const std::time_t time = std::chrono::system_clock::to_time_t(now);
  const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
  std::tm local{};
#ifdef _WIN32
  localtime_s(&local, &time);
#else
  localtime_r(&time, &local);
#endif

  char timestamp[32];
  std::snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03d", local.tm_year + 1900,
                local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec, static_cast<int>(millis));

  std::string line = timestamp;
  line += ' ';
  const char *severity_name = severityToString(severity);
  line += severity_name;
  const size_t name_len = std::strlen(severity_name);
  if (name_len < 5) {
    line.append(5 - name_len, ' '); // pad severity name to 5 columns
  }
  line += ' ';
  line += message;
  line += '\n';

#ifdef _WIN32
  initConsoleColor();
  setConsoleColor(severity);
  std::fputs(line.c_str(), stdout);
  resetConsoleColor();
  std::fflush(stdout);

  if (IsDebuggerPresent()) {
    OutputDebugStringA(line.c_str());
  }
#else
  const bool color = consoleHasColor();
  if (color) {
    std::fputs(severityColor(severity), stdout);
  }
  std::fputs(line.c_str(), stdout);
  if (color) {
    std::fputs("\x1B[0m\x1B[0K", stdout);
  }
  std::fflush(stdout);
#endif

  if (g_log_file != nullptr) {
    std::fputs(line.c_str(), g_log_file);
    std::fflush(g_log_file);
  }
}

} // namespace

void InitLog(LogSeverity log_level, bool enable_filelog, bool enable_win_console) {
  std::lock_guard<std::mutex> lock(g_log_mutex);

  g_min_severity = log_level;

  if (enable_filelog && g_log_file == nullptr) {
    // Mimic plog: start with a fresh log file, removing the previous one.
    std::remove("Descent3.log");
    g_log_file = std::fopen("Descent3.log", "a");
  }

#ifdef _WIN32
  if (enable_win_console) {
    // Open console window
    AllocConsole();
    std::freopen("CONIN$", "r", stdin);
    std::freopen("CONOUT$", "w", stdout);
    std::freopen("CONOUT$", "w", stderr);
  }
#endif
}

void WriteLog(LogSeverity log_level, const char *fmt, ...) {
  if (fmt == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_log_mutex);

  // Discard records more verbose than the configured limit (plog maxSeverity).
  if (log_level < g_min_severity) {
    return;
  }

  char buffer[4096];
  va_list args;
  va_start(args, fmt);
  const int length = std::vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  if (length < 0) {
    return;
  }

  writeLogLine(log_level, std::string(buffer, static_cast<size_t>(length)));
}
#endif
