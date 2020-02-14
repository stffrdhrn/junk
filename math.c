#include <stdio.h>

#define LOOP_START 1
#define LOOP_END   1000

// This is a comment
// 4 / 2 = 2
// 4 x 2 = 8
// 4 + 2 = 6
// 4 - 2 = 2

unsigned int div(unsigned int a, unsigned int b) {
  return a / b;
}
unsigned int mul(unsigned int a, unsigned int b) {
  return a * b;
}
unsigned int mod(unsigned int a, unsigned int b) {
  return a % b;
}

void docalcs(unsigned int a, unsigned int b) {
  printf ("%d/%d = %d, %d*%d = %d, %d%%%d = %d\n",
	  a, b, div(a, b),
	  a, b, mul(a, b),
	  a, b, mod(a, b));
}

int main() {
  int i, j;

  for (i = LOOP_START; i < LOOP_END; i++)
    for (j = LOOP_START; j < LOOP_END; j++)
      docalcs(i, j);

  docalcs(400, 3);
}
