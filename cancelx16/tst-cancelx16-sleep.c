/* Copyright (C) 2003-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2003.

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
   <https://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>


static int called;

static void
cl (void *arg)
{
  called = 1;
}


static void *
tf (void *arg)
{
  pthread_cleanup_push (cl, NULL);

  /* This call should never return.  */
  sleep (10);

  pthread_cleanup_pop (0);

  return NULL;
}


int
main (int argc, char ** argv)
{
  pthread_t th;
  if (pthread_create (&th, NULL, tf, NULL) != 0)
    {
      puts ("create failed");
      return 1;
    }

  /* Delay.  */
  sleep (1);

  if (pthread_cancel (th) != 0)
    {
      puts ("cancel failed");
      return 1;
    }

  void *result;
  if (pthread_join (th, &result) != 0)
    {
      puts ("join failed");
      return 1;
    }
  if (result != PTHREAD_CANCELED)
    {
      puts ("thread not canceled");
      return 1;
    }
  if (called == 0)
    {
      puts ("cleanup handler not called");
      return 1;
    }

  return 0;
}
