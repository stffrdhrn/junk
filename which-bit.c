#include <stdio.h>

int main() {

  unsigned int ops;
  unsigned int which;

  ops = 0x6;
  which = 0;

  printf("ops before  : %x\n", ops);
  printf("-ops        : %x\n", -ops);

  which = ops & -ops;

  printf("which before: %x\n", which);

  ops &= ~which;

  printf("ops after: %x\n", ops);

  return 0;
}
