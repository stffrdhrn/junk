#include <stdio.h>

int main(int argc, char ** argv){
     int limit;
     long long sum;
     int n;

     limit = atoi(argv[1]);

     sum = 2;
     n = 3;
     for(;n < limit; n = n + 2) {
	  int prime = 1;
	  int d;

	  for (d = 2; d < n; d++) 
	    {
	       if (!(n%d)) 
		 {
		    prime = 0;
		    break;
		 }
	    }

	  if (prime) {
	       sum += n;
	  }
     }

     printf("Sum of all = %lld\n", sum);
}
