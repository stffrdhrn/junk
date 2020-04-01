/* Create subprocess.
   Copyright (C) 2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <spawn.h>
#include <assert.h>

#include "subprocess.h"

static struct support_subprocess
support_suprocess_init (void)
{
  struct support_subprocess result;

  pipe (result.stdout_pipe);
  assert (result.stdout_pipe[0] > STDERR_FILENO);
  assert (result.stdout_pipe[1] > STDERR_FILENO);

  pipe (result.stderr_pipe);
  assert (result.stderr_pipe[0] > STDERR_FILENO);
  assert (result.stderr_pipe[1] > STDERR_FILENO);

  assert (fflush (stdout) == 0);
  assert (fflush (stderr) == 0);

  return result;
}

struct support_subprocess
support_subprocess (void (*callback) (void *), void *closure)
{
  struct support_subprocess result = support_suprocess_init ();

  result.pid = fork ();
  if (result.pid == 0)
    {
      close (result.stdout_pipe[0]);
      close (result.stderr_pipe[0]);
      dup2 (result.stdout_pipe[1], STDOUT_FILENO);
      dup2 (result.stderr_pipe[1], STDERR_FILENO);
      close (result.stdout_pipe[1]);
      close (result.stderr_pipe[1]);
      callback (closure);
      exit (0);
    }
  close (result.stdout_pipe[1]);
  close (result.stderr_pipe[1]);

  return result;
}

struct support_subprocess
support_subprogram (const char *file, char *const argv[])
{
  struct support_subprocess result = support_suprocess_init ();

  posix_spawn_file_actions_t fa;
  /* posix_spawn_file_actions_init does not fail.  */
  posix_spawn_file_actions_init (&fa);

  posix_spawn_file_actions_addclose (&fa, result.stdout_pipe[0]);
  posix_spawn_file_actions_addclose (&fa, result.stderr_pipe[0]);
  posix_spawn_file_actions_adddup2 (&fa, result.stdout_pipe[1], STDOUT_FILENO);
  posix_spawn_file_actions_adddup2 (&fa, result.stderr_pipe[1], STDERR_FILENO);
  posix_spawn_file_actions_addclose (&fa, result.stdout_pipe[1]);
  posix_spawn_file_actions_addclose (&fa, result.stderr_pipe[1]);

  posix_spawn (&result.pid, file, &fa, NULL, argv, NULL);

  close (result.stdout_pipe[1]);
  close (result.stderr_pipe[1]);

  return result;
}

int
support_process_wait (struct support_subprocess *proc)
{
  close (proc->stdout_pipe[0]);
  close (proc->stderr_pipe[0]);

  int status;
  waitpid (proc->pid, &status, 0);
  return status;
}


