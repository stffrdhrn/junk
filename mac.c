#include <stdio.h>

int main() {

  short i = 0xfffd;
  int a = 0xfffffffe;

  long acc = 0xffffffffffffffff;

  printf ("%d %x = %d x %d\n", a*i, a*i, a, i);

  printf ("%d %x = %d + %d\n", acc+(a*i), acc+(a*i), acc, a*i);

  printf ("but getting %d\n", 0xfffffffd);

  return 0;
}
