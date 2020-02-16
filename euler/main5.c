#include <stdio.h>

int main(int argc, char ** argv)
{
   int m, n, i;
   unsigned long long limit;

   n = atoi(argv[1]);

   limit = 1;
   for (i = 1; i <= n; i++)
     {
	limit *= i;
     }
   printf("Max = %lld\n", limit);

   for (m = 0; m < limit; m++)
     {
	int found = 1;
	for (i = 1; i <= n; i++)
	  {
	     if (m % i) 
	       {
		  found = 0;
		  break;
	       }
	  }
	if(found)printf("Found one %lld\n", m);
     }
}
