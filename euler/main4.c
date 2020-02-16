#include <stdio.h>
#include <string.h>

static char num[16];

int ispalindrome(int n)
{
   int len;
   int i;

   num[0] = 0;
   sprintf(num, "%d", n);

   len = strlen(num);

   for (i = 0; i < len/2; i++) 
     {
	if (num[i] != num[len - i - 1])
	  return 0;
     }
   return 1;
}

int main(int argc, char ** argv) 
{
   unsigned int n;
   int i, j;

   n = atoi(argv[1]);

   for (i = n; i < 1000; i++)
     for (j = n; j < 1000; j++)
       if (ispalindrome(i * j)) printf("%d = %d * %d\n", i * j, i, j);

   return 0;
}
