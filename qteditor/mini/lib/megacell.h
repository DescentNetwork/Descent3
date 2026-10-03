/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
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

#ifndef MEGACELL_H
#define MEGACELL_H

#include <cstdint>
#include <optional>
#include <string>

#include "manage.h"
#include "gametexture.h"

#define MAX_MEGACELLS 100

#define DEFAULT_MEGACELL_WIDTH 8
#define DEFAULT_MEGACELL_HEIGHT 8

#define MAX_MEGACELL_WIDTH 8
#define MAX_MEGACELL_HEIGHT 8

// megacell::flags has no bit defined anywhere in the game or manage code, so the
// whole word is reserved.
struct [[gnu::packed]] megacell_flags_t {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t padding : 32; // Unused padding to complete 32 bits
#else
  uint32_t padding : 32; // Unused padding to complete 32 bits
#endif
};
static_assert(sizeof(megacell_flags_t) == sizeof(uint32_t));

struct megacell {
  std::string name;
  uint8_t width;
  uint8_t height;

  int16_t texture_handles[MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT];
  megacell_flags_t flags;
  uint8_t used;
};

extern uint32_t Num_megacells;
extern megacell Megacells[MAX_MEGACELLS];

// Sets all MEGACELLs to unused
void InitMegacells();

// Allocs a MEGACELL for use, returns std::nullopt if error, else index on success
std::optional<uint32_t> AllocMegacell();

// Frees MEGACELL index n
void FreeMegacell(uint32_t n);

// Gets next MEGACELL from n that has actually been alloced
// n is signed because it may be the -1 "nothing selected" sentinel
int32_t GetNextMegacell(int32_t n);

// Gets previous MEGACELL from n that has actually been alloced
// n is signed because it may be the -1 "nothing selected" sentinel
int32_t GetPrevMegacell(int32_t n);

// Searches thru all MEGACELLs for a specific name, returns std::nullopt if not
// found or the index of the MEGACELL with that name
std::optional<uint32_t> FindMegacellName(const std::string &name);

#endif
