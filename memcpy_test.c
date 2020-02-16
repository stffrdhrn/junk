#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
#include <string.h>

//#define _MC_TEST 1
//typedef size_t __kernel_size_t;
//#define likely(n) n
//#include "arch/openrisc/lib/memcpy.c"

char * words = "this is some text and its 41 characters. ";

int test_memcpy(char * dest, char * src, size_t size) {
  int n = 0;
  int p = 0;

  for (; n < size; n++)
    src[n] = words[p++ % strlen(words)];

  src[n-1] = '\0';

//  printf("length of source string is : %d, size of source array : %d\n", strlen(src), size);
  __builtin_memcpy(dest, src, size);
//  printf("length of dest string is   : %d, size of dest array   : %d\n", strlen(dest), size);

  return (strlen(src) == strlen(dest)) && (0 == strcmp(src, dest));
}

int main(int argv, char ** argc) {
  char * dest;
  char * src;
  int size = 1;

  for (; size < 2000; size++) {
     dest = malloc(size);
     src = malloc(size);
     printf("== TEST # %d ==\n", size); 
     if(!test_memcpy(dest, src, size)) {
       printf("Fail: src   '%s'\n", src);
       printf("Fail: dest  '%s'\n", dest);
       break;
     }
     if((size > 1) && !test_memcpy(dest+1, src+1, size-1)) {
       printf("Faili off 1: src   '%s'\n", src);
       printf("Fail: off 1: dest  '%s'\n", dest);
       break;
     }
    
     if((size > 2) && !test_memcpy(dest+2, src+2, size-2)) {
       printf("Fail off 2: src   '%s'\n", src);
       printf("Fail off 2: dest  '%s'\n", dest);
       break;
     }
     
     if((size > 3) && !test_memcpy(dest+3, src+3, size-3)) {
       printf("Fail off 3: src   '%s'\n", src);
       printf("Fail off 4: dest  '%s'\n", dest);
       break;
     }
    free(dest);
     free(src);
  }

  //printf("source %x ,  dest %x \n", src, dest);
/*
  switch(4) {
    case 1: 
      memcpy_byte(dest, src, sizeof(src));
      break;
    case 2:
      memcpy_byte_unroll(dest, src, sizeof(src));
      break;
    case 3:
      memcpy_word(dest, src, sizeof(src));
      break;
    case 4:
      memcpy_word_unroll(dest, src, sizeof(src));
      break;
  }
*/
  printf("Done\n");
  return 0;
}
