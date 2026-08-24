/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * Compatibility shims for ported engine code (cfile, manage page readers)
 * compiled inside the decoupled qteditor/mini build. The full Descent3Core
 * provides these symbols; the standalone editor provides lightweight
 * equivalents.
 */

#ifndef MINI_CFILE_COMPAT_H
#define MINI_CFILE_COMPAT_H

#include <QtGlobal> // Q_ASSERT

// ASSERT is used throughout the ported engine code; map it to Qt's Q_ASSERT.
#ifndef ASSERT
#define ASSERT(x) Q_ASSERT(x)
#endif

// The engine's error handler terminates the process on a fatal error.
void Error(const char *fmt, ...);

#endif // MINI_CFILE_COMPAT_H
