#include <math.h>
#include <stdio.h>

double calc(double a, double b) {
  return cos(a) * sin(b);
}

int main() {
  printf ("calc = %lf\n", calc(3.4f, 5.6f));
}
