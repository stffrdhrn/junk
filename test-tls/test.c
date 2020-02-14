#include <stdio.h>

__thread int tbss_a;
__thread int tdata_b = 9;
static __thread int tdata_sb = 8;
#ifdef NOALIGN
static __thread int tdata_sc = 7;
#else
static __thread int tdata_sc __attribute__ ((aligned (0x10))) = 7;
#endif

int foo() {

/*  int b;

  if (a > 4) {
    b = 4;
  } else {
    b = 5;
  }
*/
  return tbss_a > 33;
}

int bar(int *data, int val) {
  int got;

  got = *data;
  if (got != val)
    printf ("got %d but expected %d\n", got, val);

  return *data > tbss_a;
}

int main(int argc, char ** argv) {
   tbss_a = argc;

   printf ("tdata = %d, tbss = %d\n", tdata_b, tbss_a);

   return foo()
	+ bar(&tdata_b, 9)
	+ bar(&tdata_sb, 8)
	+ bar(&tdata_sc, 7);
}
