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

// Replacements for the `Int3()` and `ASSERT(x)` macros the legacy MFC editor
// pulls in via pserror.h. The Qt editor does not include pserror.h (its Linux
// implementation routes through SDL_TriggerBreakpoint(), which is unusable
// here) so these macros provide Qt-friendly equivalents that log and abort.

#include <cstdlib>

#include <QDebug>

#ifndef NDEBUG

// Trigger a debug breakpoint via a fatal Qt message and abort().
#define Int3()                                                                                                          \
  do {                                                                                                                  \
    qFatal("Int3 at %s:%d", __FILE__, __LINE__);                                                                        \
    std::abort();                                                                                                       \
  } while (0)

#define ASSERT(x)                                                                                                       \
  do {                                                                                                                  \
    if (!(x)) {                                                                                                         \
      qFatal("ASSERT(%s) failed at %s:%d", #x, __FILE__, __LINE__);                                                     \
      std::abort();                                                                                                     \
    }                                                                                                                   \
  } while (0)

#else

#define Int3()              ((void)0)
#define ASSERT(x)           ((void)0)

#endif
