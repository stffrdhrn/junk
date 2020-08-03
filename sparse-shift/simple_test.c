#include <stdio.h>

typedef unsigned long int u32;

static inline u32 shift_small(int size)
{
	return 1 << (size * 8);
}

static inline u32 test(int size) {
	printf ("Test size=%d: ", size);
	switch (size) {
	case 1:
	case 2:
		return shift_small(size);
	case 4:
		printf ("not small\n");
		return 99;
	default:
		return 9999;
	}
}

int main(int argc, char ** argv) {
	test(1);

	test(2);

	test(4); // simple_test.c:7:18: warning: shift too big (32) for type int

	return 0;
}
