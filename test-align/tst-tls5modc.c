#include <tst-tls5.h>

#ifdef TLS_REGISTER
struct test_s
{
  __attribute__ ((aligned(0x20))) int a;
  __attribute__ ((aligned(0x200))) int b;
};

#define INIT_A (3 << 24) | 1
#define INIT_B 42
/* Deliberately not static.  */
static __thread struct test_s s __attribute__ ((tls_model ("initial-exec"))) =
{
  .a = INIT_A,
  .b = INIT_B
};
TLS_REGISTER (s)
#endif
