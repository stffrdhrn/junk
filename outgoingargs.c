int manyargs(int a, /* r3 */
int b, /* r4 */
int c, /* r5 */
int d, /* r6 */
int e, /* r7 */
int f, /* r8 */
int g, /* stack[0] */
int h, int i); /* stack[4] */

extern int a, c, d;


int outgoingargs(int b) {
  int e,f,g,h,i;

  e = 1;
  f = 2;
  g = 3;
  h = 4;
  i = 0x99;

  return manyargs(a,b,c,d,e,f,g,h,i);
}
