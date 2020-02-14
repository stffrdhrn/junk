#include <stdio.h>
#include <math.h>

int main(void)
{
  float x = 0.0f;
  float y;
  do
  {
    y = sinf(x);
    printf(" x = %f  y = %f\r\n", x, y);
    printf(" x = %24.17f  y = %24.17f\r\n", x, y);
    x = x + 1.0f;
  }
  while (x < 4.0f);
  return 0;
}
