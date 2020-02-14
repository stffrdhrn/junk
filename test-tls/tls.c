extern __thread int i;

__thread int x;

extern long a;

static __thread int b;
static __thread int barr[34];

void set_x(int v) {
   x = v;
}
int get_x() {
   return x;
}

int get_i() {
  return i;
}

long get_a() {
  return a;
}

void set_b(int val) {
  b = val;
}

int get_b() {
  return b;
}

void set_barr(int i, int val) {
  barr[i] = val;
}

int get_barri(int i) {
  return barr[i];
}
