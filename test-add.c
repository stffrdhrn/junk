#include <stdio.h>

void do_reloc(long address) {
   short high;
   short low;

   // the 0x8000 adjustment does...?
   high = ((address + 0x8000) >> 16);
   low = (address & 0xffff);

   printf("%08lx : %04x + %04x = %08x\n", address, high, low, (high << 16) + low);
}

int main() {
   do_reloc(0x000aeeee);
   do_reloc(0x000a0001);
   
}
