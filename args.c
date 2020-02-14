
int shifter (int a, int b, int c, int d, int e, int f, int g, int h);

int
barrel (int a, int b, int c, int d, int e, int f, int g, int h)
{
  return shifter (b, c, d, e, f, g, h, a);
}
