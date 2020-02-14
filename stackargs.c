
int geta();
int getc();
int getd(int i, int *d, int *e);

int getb(int *b, int *z) {
  int a, c, d, e;

  a = geta();
  c = getc();
  getd(c, &d, &e);

  *b = 0;
  *z = 0;
  if (a == c) {
    *b = d;
  } else {
    *z = a * c * e;
  }

  return a;
}
