#include <stdio.h>
#include <stdlib.h>

int count_get(unsigned int i, int * count_mem) {
     int count;

     if ( i == 1 ) return 1;
     if ((i < 1024 * 1024) && (count_mem[i - 1] != 0 )) {
	  return count_mem[i - 1];
     }

     if ( i % 2 ) 
       {
	  count = 1 + count_get((3 * i) + 1, count_mem);
       }
     else 
       {
	  count = 1 + count_get(i / 2, count_mem);
       }

     if ((i < 1024 * 1024)) count_mem[i-1] = count;          
     return count;
}

int main(int argc, char ** argv) {
     int * count_mem;
     int count_max;
     int limit;
     int i_max;
     int i;

     count_max = 0;

     limit = atoi(argv[1]);

     count_mem = calloc(sizeof(int), 1024 * 1024);

     for ( i = 1; i < limit; i++ )
       {
	  int count;

	  count = count_mem[i - 1];
	  if (count == 0) {
	       count = count_get(i, count_mem);
	  } 

	  if (count > count_max) {
	       count_max = count;
	       i_max = i;
	  }
       }

     printf("Max I is %d, seq %d\n", i_max, count_max);

     return 0;
}
