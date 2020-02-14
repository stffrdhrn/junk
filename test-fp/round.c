#include <stdio.h>
#include <fenv.h>

int main() {
  fenv_t save_env;

  fegetenv(&save_env);

  printf ("env : %ld\n", save_env); 

  return 0;
}
