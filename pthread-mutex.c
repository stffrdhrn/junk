#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>

   int main (int argc, char **argv)
   {
       printf ("#define __PTHREAD_MUTEX_NUSERS_OFFSET  %d\n", offsetof (pthread_mutex_t, __data.__nusers));
       printf ("#define __PTHREAD_MUTEX_KIND_OFFSET    %d\n", offsetof (pthread_mutex_t, __data.__kind));
       printf ("#define __PTHREAD_MUTEX_SPINS_OFFSET   %d\n", offsetof (pthread_mutex_t, __data.__spins));
#if __PTHREAD_MUTEX_LOCK_ELISION
       printf ("#define __PTHREAD_MUTEX_ELISION_OFFSET %d\n", offsetof (pthread_mutex_t, __data.__elision));
#endif
       printf ("#define __PTHREAD_MUTEX_LIST_OFFSET    %d\n", offsetof (pthread_mutex_t, __data.__list));
   }
