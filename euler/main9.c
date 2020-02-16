#include <stdio.h>

int main()
{
   int a, b, c;

   a = 1;
   for (; a < 500; a++) {
	b = a+1;
	for(;b < 500; b++) {
	     c = 1000 - b - a;
	     if ((c * c) == ((b * b) + (a * a))) {
		printf("Done %d, %d, %d = x%d\n", a, b, c, a*b*c);		  
		return 0;
	     }
	}
   }

   return 1;
}
