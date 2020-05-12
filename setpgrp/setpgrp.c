#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
   pid_t pid = getpid();
   pid_t pgid = getpgrp();

   if (setpgrp() != 0)
     printf ("setpgrp of %d to: %d failed: %m\n", pid, pgid);
   else
     printf ("setpgrp of %d to: %d worked.\n", pid, pgid);
   return 0;
}
