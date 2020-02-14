float a;
double d;

float geta() {
  return a;
}

float getd() {
  return d;
}

float addtoa(float b) {
  float x;
  x = a+b;
  a = x;
  return a;
}

short is_biggerthana(float b) {
  return a > b;
}

short is_biggerthand(double b) {
  return d > b;
}

float convertf(int a) {
  return (float) a;
}

double convertd(long long a) {
  return (double) a;
}

int converti(float a) {
  return (int) a;
}

extern float getother();

float getsomething(float x) {
  float ret = 0.0f;

  if (x <= a)
    ret = getother();

  return ret;
}
