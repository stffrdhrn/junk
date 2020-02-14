#include <stdlib.h>
#include <string.h>

static char * s1 = "This is it...\n";

int main() {
  char * t1 = NULL;
  int length = 0;

  t1 = strdup(s1);
  if (t1 != NULL)
    {
      length = strlen(t1);
      free(t1);
    }

  return length == strlen(s1);
}
