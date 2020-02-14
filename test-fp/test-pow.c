#include <math.h>
#include <stdio.h>
#include <fenv.h>
#include <errno.h>
#include <stdlib.h>

#ifdef LONG_DOUBLE
/* The test is for long double */
#define F_TYPE long double
#define LIT(x) x ## L
#define F_FMT "La"
#elif defined(DOUBLE)
/* Just double */
#define F_TYPE double
#define LIT(x) x
#define F_FMT "a"
#else
/* Float */
#define F_TYPE float
#define LIT(x) x
#define F_FMT "a"
#define powl pow
#endif

struct test_data {
  F_TYPE base;
  F_TYPE exp;
  F_TYPE expected;
  char * expected_exceptions;
};

static struct test_data tests [] = {
  { LIT ( 2.0 ),          LIT ( 3.0 ),           LIT ( 8.0 ),                   "------" },
  { LIT (-0x2.00004p+0 ), LIT ( -0x3.ffep+12 ),  LIT ( 0x0p+0 ),                "1----U" },
  { LIT (-0x2.00004p+0 ), LIT ( -0x3.fffp+12 ),  LIT (-0x4p-1076 ),             "-----U" },
  { LIT (-0x2.00004p+0 ), LIT ( -0x7.ep+4 ),     LIT (0x3.ffc101f405952p-128 ), "-----U" },
};
static int test_data_len = 4;

static char *
fex_to_str() {
  char * str = calloc(16, 1);

  str[0] = errno ? '1' : '-';
  str[1] = fetestexcept(FE_DIVBYZERO) ? 'D' : '-';
  str[2] = fetestexcept(FE_INEXACT) ?   'X' : '-';
  str[3] = fetestexcept(FE_INVALID) ?   'V' : '-';
  str[4] = fetestexcept(FE_OVERFLOW) ?  'O' : '-';
  str[5] = fetestexcept(FE_UNDERFLOW) ? 'U' : '-';

  return str;
}

int main() {
  for (int i = 0; i < test_data_len; i++) {
    struct test_data td = tests[i];
    F_TYPE d;
    char * fex;

    /* Clear exceptions and errors */
    errno = 0;
    fesetenv (FE_DFL_ENV);

    /* Run test */
    d = powl (td.base, td.exp);

    /* Print Results */
   fex = fex_to_str ();    
   printf ("pow(%20"F_FMT", %20"F_FMT") : (%24"F_FMT" %s) -> %24"F_FMT" %s\n", td.base, td.exp,
            td.expected, td.expected_exceptions, d, fex);
   free (fex);
  }
}
