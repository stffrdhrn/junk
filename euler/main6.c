#include <stdio.h>

int main(int argc, char ** argv)
{
int n, i;
int sosq, sqos;

n = atoi(argv[1]);

for (i = 1; i <= n; i++)
  {
     sosq += i*i;
  }
for (i = 1; i <= n; i++)
  {
     sqos += i;
  }
sqos = sqos * sqos; 

printf("diff %d - %d = %d\n", sqos, sosq, sqos - sosq);
}
