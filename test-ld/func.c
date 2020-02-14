#include <iostream>
#include <unistd.h>

using namespace std;
void* __dso_handle;

namespace test {
  extern int f(int i, int j, int k);
}
int brean();

int toom() {
  read(1, NULL, 1);
  return 1;
}

int boom() {
 try {
   int i, j, k;
   i = brean();
   for (int b = 0; b < i; b++) {
     j = brean();
     k = brean();
   }
   test::f(i, j, k);
   return 0;
 } catch (const std::overflow_error& e) {
    // this executes if f() throws std::overflow_error (same type rule)
    return 3;
 } catch (const std::runtime_error& e) {
    // this executes if f() throws std::underflow_error (base class rule)
    return 2;
 } catch (const std::exception& e) {
    return 1;
 }
}

int brean () {
  try
  {
    throw 20;
  }
  catch (int e)
  {
    cout << "An exception occurred. Exception Nr. " << e << '\n';
    throw 13;
  }
  return 0;
}

