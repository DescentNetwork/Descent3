#pragma once
#include <cstdint>

namespace d3
{
  constexpr const int32_t rand_max = 0x7fff;
  void srand(uint32_t seed);
  int32_t rand(void);
}
