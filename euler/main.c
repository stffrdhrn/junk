#include <stdio.h>

int main(int argc, char ** argv) 
{

   long long limit = 317584931803L;
   int i = 2;

   printf("Limit = %lld\n", limit);

   for (;i < limit; i++) 
     {
	if (!(limit % i)) {
	     int n = 2;
	     int prime = 1;
	     for (; n < i; n++) 
	       {
		  if (!(i%n)) 
		    {
		       prime = 0;
		       break;
		    }
	       }
	     if (prime) printf("Prime factor = %lld\n", i);
	}
     }

   return 0;
}
