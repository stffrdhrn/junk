/* Copyright (C) 2002-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2002.

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

#define _GNU_SOURCE // for program_invocation_short_name
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// From support/main
#include <assert.h>
#include <getopt.h>
#include <malloc.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define OPT_COMMAND	10000
#define OPT_PIDFILE	10001

static const struct option default_options[] =
{
  { "verbose", no_argument, NULL, 'v' },
  { "command", required_argument, NULL, OPT_COMMAND },
  { "pidfile", required_argument, NULL, OPT_PIDFILE },
  { NULL, 0, NULL, 0 }
};

unsigned int test_verbose = 0;

const char *command;
const char *pidfile;
char pidfilename[] = "/tmp/tst-cancel7-XXXXXX";

static void *
tf (void *arg)
{
  const char *args = " --pidfile ";
  char *cmd = alloca (strlen (command) + strlen (args)
		      + strlen (pidfilename) + 1);

  strcpy (stpcpy (stpcpy (cmd, command), args), pidfilename);

  system (cmd);

  /* This call should never return.  */
  return NULL;
}


static void
sl (void)
{
  FILE *f = fopen (pidfile, "w");
  if (f == NULL)
    exit (1);

  fprintf (f, "%lld\n", (long long) getpid ());
  fflush (f);

  struct flock fl =
    {
      .l_type = F_WRLCK,
      .l_start = 0,
      .l_whence = SEEK_SET,
      .l_len = 1
    };
  if (fcntl (fileno (f), F_SETLK, &fl) != 0)
    exit (1);

  sigset_t ss;
  sigfillset (&ss);
  sigsuspend (&ss);
  exit (0);
}


static void
do_prepare (int argc, char *argv[])
{
  if (command == NULL)
    command = argv[0];

  if (pidfile)
    sl ();

  int fd = mkstemp (pidfilename);
  if (fd == -1)
    {
      puts ("mkstemp failed");
      exit (1);
    }

  write (fd, " ", 1);
  close (fd);
}


static int
do_test (void)
{
  void *r;
  pthread_t th;
  if (pthread_create (&th, NULL, tf, NULL) != 0)
    {
      puts ("pthread_create failed");
      return 1;
    }
  printf ("Before sleep pidfile thread. %ld\n", *((long *)(th+104)));

  do
    sleep (1);
  while (access (pidfilename, R_OK) != 0);

  printf ("Cancelling pidfile thread. %ld\n", *((long *)(th+104)));
  pthread_cancel (th);
  printf ("Cancelled pidfile thread. %ld\n", *((long *)(th+104)));
  pthread_join (th, &r);

  sleep (1);

  FILE *f = fopen (pidfilename, "r+");
  if (f == NULL)
    {
      puts ("no pidfile");
      return 1;
    }

  long long ll;
  if (fscanf (f, "%lld\n", &ll) != 1)
    {
      puts ("could not read pid");
      unlink (pidfilename);
      return 1;
    }

  struct flock fl =
    {
      .l_type = F_WRLCK,
      .l_start = 0,
      .l_whence = SEEK_SET,
      .l_len = 1
    };
  if (fcntl (fileno (f), F_GETLK, &fl) != 0)
    {
      puts ("F_GETLK failed");
      unlink (pidfilename);
      return 1;
    }

  if (fl.l_type != F_UNLCK)
    {
      printf ("child %lld still running\n", (long long) fl.l_pid);
      if (fl.l_pid == ll)
	kill (fl.l_pid, SIGKILL);

      unlink (pidfilename);
      return 1;
    }

  fclose (f);

  unlink (pidfilename);

  return r != PTHREAD_CANCELED;
}

static void
do_cleanup (void)
{
  FILE *f = fopen (pidfilename, "r+");
  long long ll;

  if (f != NULL && fscanf (f, "%lld\n", &ll) == 1)
    {
      struct flock fl =
	{
	  .l_type = F_WRLCK,
	  .l_start = 0,
	  .l_whence = SEEK_SET,
	  .l_len = 1
	};
      if (fcntl (fileno (f), F_GETLK, &fl) == 0 && fl.l_type != F_UNLCK
	  && fl.l_pid == ll)
	kill (fl.l_pid, SIGKILL);

      fclose (f);
    }

  unlink (pidfilename);
}

static void
cmdline_process (int c)
{
  switch (c)
    {
    case OPT_COMMAND:
      command = optarg;
      break;
    case OPT_PIDFILE:
      pidfile = optarg;
      break;
    }
}
/* Show people how to run the program.  */
static void
usage (const struct option *options)
{
  size_t i;

  printf ("Usage: %s [options]\n"
          "\n",
          program_invocation_short_name);
  printf ("Options:\n");
  for (i = 0; options[i].name; ++i)
    {
      int indent;

      indent = printf ("  --%s", options[i].name);
      if (options[i].has_arg == required_argument)
        indent += printf (" <arg>");
      printf ("%*s", 25 - indent, "");
      switch (options[i].val)
        {
        case 'v':
          printf ("Increase the output verbosity");
          break;
        }
      printf ("\n");
    }
}


int
main (int argc, char **argv)
{
  const struct option *options = default_options;
  int opt;

  while ((opt = getopt_long (argc, argv, "+", options, NULL))
	 != -1)
    switch (opt)
      {
      case '?':
        usage (options);
        exit (1);
      case 'v':
        ++test_verbose;
        break;
      default:
	cmdline_process (opt);
      }

  /* Correct for the possible parameters.  */
  argv[optind - 1] = argv[0];
  argv += optind - 1;
  argc -= optind - 1;

  do_prepare (argc, argv);
  atexit (do_cleanup);

  return do_test ();
}
