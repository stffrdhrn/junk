#include <stdint.h>

volatile uint8_t g_doswap = 1;

uint64_t swap_1(uint64_t u64)
{
   uint32_t u64_lo, u64_hi, u64_tmp;

   u64_lo = u64 & 0xFFFFFFFF;
   u64_hi = u64 >> 32;

   if (g_doswap) {
      u64_tmp = u64_lo;
      u64_lo  = u64_hi;
      u64_hi  = u64_tmp;
   }

   u64 = u64_lo;
   u64 += ((uint64_t) u64_hi << 32);

   return u64;
}  // swap_1
