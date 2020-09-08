#include <stdio.h>

#define EFAULT 99
#include <uaccess.h>

typedef unsigned long u32;
typedef unsigned long long u64;




        u32 gets2s(u32 *ptr)
        {
                u32 val;

                asm("l.lwz %0,0(%1)" : "=r"(val) : "r"(ptr));
                return val;
        }
        u64 getd2d(u64 *ptr)
        {
                u64 val;
                asm volatile ("l.lwz %0,0(%1)\n"
                              "l.lwz %H0,4(%1)" : "=&r"(val) : "r"(ptr));
                //val = *ptr;
		return val;
        }

	u32 ___space0___() {
		return 0;
	}

        u32 getd2s(u64 *ptr)
        {
                u32 val;

                asm volatile ("l.lwz %0,0(%1)\n"
                              "l.lwz %H0,4(%1)" : "=&r"(val) : "r"(ptr));
                return val;
        }

        u32 getd2st(u64 *ptr)
        {
                u64 val;

                asm volatile ("l.lwz %0,0(%1)\n"
                              "l.lwz %H0,4(%1)" : "=&r"(val) : "r"(ptr));
                return val;
        }

	u32 ___space1___() {
		return 1;
	}

	u64 gets2d(u32 *ptr)
        {
                u64 val = 0;

                asm("l.lwz %0,0(%1)" : "=r"(val) : "r"(ptr));
                return val;
        }

	u64 gets2dt(u32 *ptr)
        {
                u32 val;

                asm("l.lwz %0,0(%1)" : "=r"(val) : "r"(ptr));
                return val;
        }

// 64-bit pair for storing 1.
//    r17  0
//    r19  1
//
//    Memory
//    0x0 0
//    0x4 1
//
static	u32 v32 = 32;
static	u64 v64 = 64;


int main() {
        u64 t64;
        u32 t32;

	__get_user(t32, &v32);
	__get_user(t64, &v64);
	printf ("gu s2s: %ld\n", t32);
	printf ("gu d2d: %ld\n", (u32) t64);

	__get_user(t32, &v64);
	printf ("gu d2s: %ld\n", t32);

	__get_user(t64, &v32);
	printf ("gu s2d: %ld\n", (u32) t64);

	printf ("d2s: %ld\n", getd2s(&v64));
	printf ("d2st: %ld\n", getd2st(&v64));

	printf ("s2d: %ld\n", (u32) gets2d(&v32));
	printf ("s2dt: %ld\n", (u32) gets2dt(&v32));
}
