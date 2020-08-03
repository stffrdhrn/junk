#include <stdio.h>

typedef unsigned long int u32;

#define __BIG_ENDIAN 1
#define BITS_PER_BYTE 8

/* This is an except from:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/openrisc/include/asm/cmpxchg.h?h=v5.8#n101
 *
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 *
 * Example: https://www.mail-archive.com/linux-kernel@vger.kernel.org/msg2256733.html
 */

static inline u32 cmpxchg_small(volatile void *ptr, u32 old, u32 new,
                                int size)
{
        int off = (unsigned long)ptr % sizeof(u32);
#ifdef __BIG_ENDIAN
        int bitoff = (sizeof(u32) - size - off) * BITS_PER_BYTE;
#else
        int bitoff = off * BITS_PER_BYTE;
#endif
        u32 bitmask = ((0x1 << size * BITS_PER_BYTE) - 1) << bitoff;

	printf(" bitmask=%08lx, off=%d, bitoff=%d, sizewidth=%d\n", bitmask,
		off, bitoff, size * BITS_PER_BYTE);

	return bitmask;
}

static inline u32 test(u32 ptr, int size) {
	printf ("Test with ptr=%08lx, size=%d: ", ptr, size);
	switch (size) {
	case 1:
	case 2:
		return cmpxchg_small((void *) ptr, 0, 0, size);
	case 4:
		printf ("not small\n");
		return 99;
	default:
		return 9999;
	}
}

int main(int argc, char ** argv) {
	test(0x0000, 1);
	test(0x0001, 1);
	test(0x0002, 1);
	test(0x0003, 1);
	test(0x0004, 1);

	test(0x0000, 2);
	test(0x0001, 2);
	test(0x0002, 2);
	// test(0x0003, 2); skip, 2 bytes not available at this offset
	test(0x0004, 2);

	test(0x0000, 4); // xchg_test.c:17:29: warning: shift too big (32) for type int

	return 0;
}
