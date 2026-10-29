#include "rand.h"

namespace d3
{
  static int32_t ps_holdrand = 1L;
  void srand(uint32_t seed) { ps_holdrand = static_cast<int32_t>(seed); }
  int32_t rand(void) { return (((ps_holdrand = ps_holdrand * 214013L + 2531011L) >> 16) & 0x7fff); }
}
