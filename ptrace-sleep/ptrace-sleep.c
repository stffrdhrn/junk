#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/uio.h> /* For iovec */
#include <sys/procfs.h> /* for elf_gregset_t */

#define NT_PRSTATUS (void *) 1

static void error(const char *msg) {
  printf ("error: %s\n", msg);
  exit (1);
}

static int
child_function() {
  ptrace(PTRACE_TRACEME, 0, (void *) 0, (void *) 0);
  kill(getpid(), SIGSTOP);

  sleep(1);

  exit(0);
}

static int
fork_to_function(int (*function) (void *)) {
  int child_pid;

  child_pid = fork();

  if (child_pid == 0)
    function (NULL);

  if (child_pid == -1)
    error("fork failed");

  return child_pid;
}


int main() {
  int child_pid, ret, status;
  struct iovec iov;
  elf_gregset_t regs;

  child_pid = fork_to_function(child_function);

  /* Wait for child to reach the stopped state */
  ret = waitpid(child_pid, &status, 0);

  if (ret == -1)
    error("waitpid failed");
  else if (ret != child_pid)
    error("we didn't get the child we expected");
  else if (!WIFSTOPPED(status))
    error("the child is not stopped, got some other signal!");

  /* Now the child is stopped and can be traced. Continue
     the child and get registers 100 times or until failure
     previously this would fail when the child went to sleep
     and lost its task->state == __TASK_TRACED status.  */
  iov.iov_base = &regs;
  iov.iov_len = sizeof (regs);
  if (ptrace(PTRACE_GETREGSET, child_pid, NT_PRSTATUS,
             (void *) &iov) == -1)
    error("Couldn't get registers, before sleep");


  ret = ptrace (PTRACE_CONT, child_pid, (void *) 0, (void *) 0);
  if (ret != 0)
    error("fork resume failed");

  for (int i = 0; i < 100; i++) {
    struct timespec ts;

    if (ptrace(PTRACE_GETREGSET, child_pid, NT_PRSTATUS,
               (void *) &iov) == -1)
	error("Couldn't get registers");

    /* Sleep for 10 ms */
    ts.tv_sec = 0L;
    ts.tv_nsec = 10 * 1000 * 1000;
    nanosleep(&ts, NULL);
  }
  puts("done");
}
