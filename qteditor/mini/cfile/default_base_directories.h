/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
 *
 * Stub for the engine's compile-time list of default read-only base
 * directories. The decoupled editor locates its data at runtime instead of
 * baking in install paths, so this list is empty.
 */

#ifndef DEFAULT_BASE_DIRECTORIES_H
#define DEFAULT_BASE_DIRECTORIES_H

#include <filesystem>
#include <vector>

namespace D3 {
extern const std::vector<std::filesystem::path> Default_read_only_base_directories;
} // namespace D3
#endif // DEFAULT_BASE_DIRECTORIES_H
