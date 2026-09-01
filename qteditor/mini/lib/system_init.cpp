/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024-2026 Descent Developers
 *
 * Qt-neutral system initialization and command-line argument handling
 * (ported from the original init.cpp / args.cpp).
 */

#include "init.h"
#include "args.h"
#include "mem.h"
#include "room.h"
#include "object.h"
#include "lightmap_info.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>

#include <QtGlobal>

void Error(const char *fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  qCritical("%s", buf);
  std::abort();
}

void GatherArgs(char **argv) {
  if (argv == nullptr)
    return;
  int n = 0;
  for (int i = 0; argv[i] && i < MAX_ARGS; i++) {
    std::strncpy(GameArgs[n++], argv[i], MAX_CHARS_PER_ARG - 1);
    GameArgs[n - 1][MAX_CHARS_PER_ARG - 1] = '\0';
  }
}

void GatherArgs(const char *str) {
  if (str == nullptr)
    return;
  int n = 0;
  const char *p = str;
  while (*p && n < MAX_ARGS) {
    while (*p == ' ')
      p++;
    if (!*p)
      break;
    int len = 0;
    while (p[len] && p[len] != ' ' && len < MAX_CHARS_PER_ARG - 1)
      len++;
    std::memcpy(GameArgs[n], p, len);
    GameArgs[n][len] = '\0';
    n++;
    p += len;
  }
}

int FindArg(const char *which, int start) {
  if (which == nullptr)
    return 0;
  for (int i = start; i < MAX_ARGS; i++) {
    if (GameArgs[i][0] && strcasecmp(GameArgs[i], which) == 0)
      return i;
  }
  return 0;
}

int FindArgChar(const char *which, char singleCharArg) { return FindArg(which); }

const char *GetArg(int index) {
  if (index < 0 || index >= MAX_ARGS)
    return "";
  return GameArgs[index];
}

// Minimal pre-init: establish the memory and error subsystems that the
// ported engine code relies on before anything else runs.
void PreInitD3Systems() {
  if (FindArg("-lowmem") || FindArg("-dedicated"))
    Mem_low_memory_mode = true;
  if (FindArg("-superlowmem")) {
    Mem_low_memory_mode = true;
    Mem_superlow_memory_mode = true;
  }
  if (FindArg("-himem")) {
    Mem_low_memory_mode = false;
    Mem_superlow_memory_mode = false;
  }
}

// First-phase system init. The decoupled editor replaces the full game's I/O,
// graphics, sound and network stacks with Qt + OpenGL and a HOG-capable
// cfile; here we initialise the data structures the editor core uses.
void InitD3Systems1(bool /*editor*/) {
  InitLightmapInfo();
  InitRooms();
  ResetObjectList();
}
