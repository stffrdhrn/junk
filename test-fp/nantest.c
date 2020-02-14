#include <stdio.h>
#include <math.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif

static void tstnan (void)
{
  double d;

  /* Various tests to detect a NaN without using the math library (-lm).
 *    * MIPSpro 7.3.1.3m (IRIX64) does too many optimisations, so that
 *       * both NAN != NAN and !(NAN >= 0.0 || NAN <= 0.0) give 0 instead
 *          * of 1. As a consequence, in MPFR, one needs to use
 *             *    #define DOUBLE_ISNAN(x) (!(((x) >= 0.0) + ((x) <= 0.0)))
 *                * in this case. */

  d = NAN;
  printf ("\n");
  printf ("NAN != NAN --> %d (should be 1)\n", d != d);
  printf ("isnan(NAN) --> %d (should be 1)\n", isnan (d));
  printf ("NAN >= 0.0 --> %d (should be 0)\n", d >= 0.0);
  printf ("NAN <= 0.0 --> %d (should be 0)\n", d <= 0.0);
  printf ("  #3||#4   --> %d (should be 0)\n", d >= 0.0 || d <= 0.0);
  printf ("!(#3||#4)  --> %d (should be 1)\n", !(d >= 0.0 || d <= 0.0));
  printf ("  #3 + #4  --> %d (should be 0)\n", (d >= 0.0) + (d <= 0.0));
  printf ("!(#3 + #4) --> %d (should be 1)\n", !((d >= 0.0) + (d <= 0.0)));
}

int main(int argc, char ** args)
{
  tstnan ();
}
