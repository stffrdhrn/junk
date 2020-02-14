#include <stdio.h>
#include <stdbool.h>

float a = 1.2f;
double da = 134.322f;

long long large = 1;

long mullarge(long long x) {
   return x * large;
}

float mula(int x) {
   float f = (float) x;
   return f * a;
}

bool test_gt(int x) { return x > a; }
bool test_ge(float x) { return x >= a; }
bool test_lt(int x) { return x < a; }
bool test_le(float x) { return x <= a; }
bool test_ne(int x) { return x != a; }
bool test_eq(float x) { return x == a; }

double mulda(int x) {
   double f = (double) x;
   return f * da;
}

bool testd_gt(int x) { return x > da; }
bool testd_ge(double x) { return x >= da; }
bool testd_lt(int x) { return x < da; }
bool testd_le(double x) { return x <= da; }
bool testd_ne(int x) { return x != da; }
bool testd_eq(double x) { return x == da; }


int main() {
  int b = 2;
  int db = 200;
  /* test alu */
  printf ("hello gcc from or1k fpu: %f * %d = %f: ", a, b, mula(b));
  if (mula(b) == 2.4f) {
    printf ("ok.\n");
  } else {
    printf ("oh no!.\n");
  }

  /* test storecc */
  printf ("test: gt %d\n", test_gt(5));
  printf ("test: ge %d\n", test_ge(5.0f));
  printf ("test: ge %d\n", test_ge(1.2f));
  printf ("test: lt %d\n", test_lt(1));
  printf ("test: le %d\n", test_le(1.0f));
  printf ("test: le %d\n", test_le(1.2f));
  printf ("test: ne %d\n", test_ne(1));
  printf ("test: eq %d\n", test_eq(1.2f));

  /* test branch */
  if (b > a) {
    printf ("Branching: ok.\n");
  }
  printf ("\n");

  /* Double */
  printf ("hello gcc from or1k fpu: %lf * %d = %lf: ", da, b, mulda(b));
  if (mulda(b) == 268.644012f) {
    printf ("ok.\n");
  } else {
    printf ("oh no!.\n");
  }

  /* test doubles */
  printf ("testd: gt %d\n", testd_gt(500));
  printf ("testd: ge %d\n", testd_ge(135.0f));
  printf ("testd: ge %d\n", testd_ge(134.322f));
  printf ("testd: lt %d\n", testd_lt(134));
  printf ("testd: le %d\n", testd_le(134.0f));
  printf ("testd: le %d\n", testd_le(134.322f));
  printf ("testd: ne %d\n", testd_ne(1));
  printf ("testd: eq %d\n", testd_eq(134.322f));

  if (db > da) {
    printf ("D-branching: ok.\n");
  }


  return 0;
}
