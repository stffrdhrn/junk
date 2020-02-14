//********************************************************************************
// The main() program returns '0' if all OK; '1' if there is a failure.
// A failure happens if the result from calling the swap_1() function with a known
//   input does not match the known/expected result.
//
// NOTE: The main program is organized as a loop structure so gcc does not 
//   optimize-away the calls to swap_1().  Compiling with -O2 is still smart
//   enough to optimize-away the calls, but using -Os does not.
// The bad code is only generated when compiled with -Os, like this (generates file "swap1.s"):
//   
// % or1k-elf-gcc --sysroot= -nostdlib -Os -mhard-mul -msoft-div -msoft-float  -S swap1.c
// 
// FYI: Defining the symbol "EN_PRINT" causes a print of the input and output of the
//   swap_1() function each time it's called.
//
// Result:
// Below is a snippet of the bad code generated in swap1.s.
// All code is OK, except for the very last instruction (in the l.jr delay slot).
//   Up to that point in execution, r11 and r12 contain the correct (expected)
//   values, but the execution of the final "l.addc" corrupts r11.
//
// swap_1:
// .LFB1:
// 	.cfi_startproc
// 	l.movhi	r17,hi(.LANCHOR0)
// 	l.movhi	r12,hi(0)
// 	l.ori	r17,r17,lo(.LANCHOR0)
// 	l.ori	r11,r4,0
// 	l.lbz	r17,0(r17)
// 	l.sfeqi	r17,0
// 	l.cmov	r4,r4,r3
// 	l.add	r12,r12,r4
// 	l.cmov	r11,r3,r11
// 	l.jr	r9
// 	 l.addc	r11,r11,r12   # <<<--- Bad instruction
// 	.cfi_endproc
// 
//********************************************************************************

#include <stdint.h>
#include <stdio.h>

// #define EN_PRINT

volatile static uint8_t g_doswap = 1;

#ifdef EN_PRINT
//********************************************************************************
// print an unsigned 64-bit value as 16 hex digits with leading '0x'
// Only requires 'puts()' to be available in linked library; does not require printf().
void p64(uint64_t val)
{
   char arr[32];
   char *ptr;
   char ch;
   int  nib;
   int  ii;

   ptr = arr;
   *ptr++ = '0';
   *ptr++ = 'x';
   for (ii=15; ii >= 0; ii--) {
      nib = (val >> (ii*4)) & 0xF;
      switch (nib) {
      case 0:    ch = '0';   break;
      case 1:    ch = '1';   break;
      case 2:    ch = '2';   break;
      case 3:    ch = '3';   break;
      case 4:    ch = '4';   break;
      case 5:    ch = '5';   break;
      case 6:    ch = '6';   break;
      case 7:    ch = '7';   break;
      case 8:    ch = '8';   break;
      case 9:    ch = '9';   break;
      case 10:   ch = 'A';   break;
      case 11:   ch = 'B';   break;
      case 12:   ch = 'C';   break;
      case 13:   ch = 'D';   break;
      case 14:   ch = 'E';   break;
      case 15:   ch = 'F';   break;
      }
      *ptr++ = ch;
   }
   *ptr++ = '\0';
   puts(arr);
}  // p64
#endif


//********************************************************************************
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


//********************************************************************************
int main()
{
   int ret;
   int iter;
   uint64_t  aa[2];   // inputs to swap function
   uint64_t  ee[2];   // expected outputs of swap function
   uint64_t  rr[2];   // actual results of swap function

   g_doswap = 1;

   // populate inputs, and expected outputs:
   aa[0] = 0x123456789abcdef0;
   aa[1] = 0x0123456789abcdef;

   ee[0] = 0x9ABCDEF012345678;
   ee[1] = 0x89ABCDEF01234567;

   ret = 0;
   for (iter = 0; iter < 2; iter++) {
      rr[iter] = swap_1(aa[iter]);
#ifdef EN_PRINT
      p64(aa[iter]);
      p64(rr[iter]);
#endif
      // early-out if there's a mis-match:
      if (ee[iter] != rr[iter])
         ret = 1;
   }

   return ret;
}  // main

#undef EN_PRINT
