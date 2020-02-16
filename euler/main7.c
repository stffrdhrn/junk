#include <stdio.h>

int main(int argc, char ** argv) 
{
   int limit;
   unsigned long long i = 2l;
   int count = 1;

   limit = atoi(argv[1]);

   printf("Limit = %d\n", limit);

   while (count <= limit) 
     {
	     unsigned long long n = 2;
	     unsigned short prime = 1;
	     for (; n < i; n++) 
	       {
		  if (!(i%n)) 
		    {
		       prime = 0;
		       break;
		    }
	       }
	     if (prime) {
		  printf("%d. %11d\n", count, i);
		  count++;
	     }
	     i++;
     }

   return 0;
}
