/* Test capturing output from a subprocess.
   Copyright (C) 2017-2019 Free Software Foundation, Inc.
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <paths.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "capture_subprocess.h"

/* Nonzero if the program gets called via 'exec'.  */
static int restart = 0;

/* Hold the four initial argument used to respawn the process.  */
static char *initial_argv[5];

/* Write one byte at *P to FD and advance *P.  Do nothing if *P is
   '\0'.  */
static void
transfer (const unsigned char **p, int fd)
{
  if (**p != '\0')
    {
      assert (write (fd, *p, 1) == 1);
      ++*p;
    }
}

/* Determine the order in which stdout and stderr are written.  */
enum write_mode { out_first, err_first, interleave,
                  write_mode_last =  interleave };

static const char *
write_mode_to_str (enum write_mode mode)
{
  switch (mode)
    {
    case out_first:  return "out_first";
    case err_first:  return "err_first";
    case interleave: return "interleave";
    default:         return "write_mode_last";
    }
}

static enum write_mode
str_to_write_mode (const char *mode)
{
  if (strcmp (mode, "out_first") == 0)
    return out_first;
  else if (strcmp (mode, "err_first") == 0)
    return err_first;
  else if (strcmp (mode, "interleave") == 0)
    return interleave;
  return write_mode_last;
}

/* Describe what to write in the subprocess.  */
struct test
{
  char *out;
  char *err;
  enum write_mode write_mode;
  int signal;
  int status;
};

_Noreturn static void
test_common (const struct test *test)
{
  bool mode_ok = false;
  switch (test->write_mode)
    {
    case out_first:
      assert (fputs (test->out, stdout) >= 0);
      assert (fflush (stdout) == 0);
      assert (fputs (test->err, stderr) >= 0);
      assert (fflush (stderr) == 0);
      mode_ok = true;
      break;
    case err_first:
      assert (fputs (test->err, stderr) >= 0);
      assert (fflush (stderr) == 0);
      assert (fputs (test->out, stdout) >= 0);
      assert (fflush (stdout) == 0);
      mode_ok = true;
      break;
    case interleave:
      {
        const unsigned char *pout = (const unsigned char *) test->out;
        const unsigned char *perr = (const unsigned char *) test->err;
        do
          {
            transfer (&pout, STDOUT_FILENO);
            transfer (&perr, STDERR_FILENO);
          }
        while (*pout != '\0' || *perr != '\0');
      }
      mode_ok = true;
      break;
    }
  assert (mode_ok);

  if (test->signal != 0)
    raise (test->signal);
  exit (test->status);
}

static int
parse_int (const char *str)
{
  char *endptr;
  long int ret = strtol (str, &endptr, 10);
  assert (ret >= 0 && ret <= INT_MAX);
  return ret;
}

/* For use with support_capture_subprogram.  */
_Noreturn static void
handle_restart (char *out, char *err, const char *write_mode,
		const char *signal, const char *status)
{
  struct test test =
    {
      out,
      err,
      str_to_write_mode (write_mode),
      parse_int (signal),
      parse_int (status)
    };
  test_common (&test);
}

/* For use with support_capture_subprocess.  */
_Noreturn static void
callback (void *closure)
{
  const struct test *test = closure;
  test_common (test);
}

/* Create a heap-allocated random string of letters.  */
static char *
random_string (size_t length)
{
  char *result = malloc (length + 1);
  for (size_t i = 0; i < length; ++i)
    result[i] = 'a' + (rand () % 26);
  result[length] = '\0';
  return result;
}

/* Check that the specific stream from the captured subprocess matches
   expectations.  */
static void
check_stream (const char *what, const char *stream,
              const char *expected)
{
  if (strcmp (stream, expected) != 0)
    {
      printf ("error: captured %s data incorrect\n"
              "  expected: %s\n"
              "  actual:   %s\n",
              what, expected, stream);
    }
  if (strlen (stream) != strlen (expected))
    {
      printf ("error: captured %s data length incorrect\n"
              "  expected: %zu\n"
              "  actual:   %zu\n",
              what, strlen (expected), strlen (stream));
    }
}

static struct support_capture_subprocess
do_subprocess (struct test *test)
{
  return support_capture_subprocess (callback, test);
}

static struct support_capture_subprocess
do_subprogram (const struct test *test)
{
  /* Three digits per byte plus null terminator.  */
  char signalstr[3 * sizeof(int) + 1];
  snprintf (signalstr, sizeof (signalstr), "%d", test->signal);
  char statusstr[3 * sizeof(int) + 1];
  snprintf (statusstr, sizeof (statusstr), "%d", test->status);

  int argc = 0;
  enum {
    /* 4 elements from initial_argv (path to ld.so, '--library-path', the
       path', and application name'), 2 for restart argument ('--direct',
       '--restart'), 5 arguments plus NULL.  */
    argv_size = 12
  };
  char *args[argv_size];

  for (char **arg = initial_argv; *arg != NULL; arg++)
    args[argc++] = *arg;

  args[argc++] = (char*) "--direct";
  args[argc++] = (char*) "--restart";

  args[argc++] = test->out;
  args[argc++] = test->err;
  args[argc++] = (char*) write_mode_to_str (test->write_mode);
  args[argc++] = signalstr;
  args[argc++] = statusstr;
  args[argc]   = NULL;
  assert (argc < argv_size);

//  printf ("%s", args[0]);
//  for (int i = 1; args[i] != NULL; i++)
//    printf(" %s \\\n", args[i]);

  return support_capture_subprogram (args[0], args);
}

enum test_type
{
  subprocess,
  subprogram,
};

static int
do_multiple_tests (enum test_type type)
{
  const size_t lengths[] = {0, 1, 17, 512, 20000, 9999};

  /* Test multiple combinations of support_capture_sub{process,program}.

     length_idx_stdout: Index into the lengths array above,
       controls how many bytes are written by the subprocess to
       standard output.
     length_idx_stderr: Same for standard error.
     write_mode: How standard output and standard error writes are
       ordered.
     signal: Exit with no signal if zero, with SIGTERM if one.
     status: Process exit status: 0 if zero, 3 if one.  */
  for (size_t length_idx_stdout = 0; lengths[length_idx_stdout] != 9999;
       ++length_idx_stdout)
    for (size_t length_idx_stderr = 0; lengths[length_idx_stderr] != 9999;
         ++length_idx_stderr)
      for (int write_mode = 0; write_mode < write_mode_last; ++write_mode)
        for (int signal = 0; signal < 2; ++signal)
          for (int status = 0; status < 2; ++status)
            {
              struct test test =
                {
                  .out = random_string (lengths[length_idx_stdout]),
                  .err = random_string (lengths[length_idx_stderr]),
                  .write_mode = write_mode,
                  .signal = signal * SIGTERM, /* 0 or SIGTERM.  */
                  .status = status * 3,       /* 0 or 3.  */
                };
              assert (strlen (test.out) == lengths[length_idx_stdout]);
              assert (strlen (test.err) == lengths[length_idx_stderr]);

	      fprintf (stdout,
"case: subprogram: %d stdout: %zd stderr: %zd wr_mode: %s signal: %d status: %d\n",
		       type, length_idx_stdout, length_idx_stderr,
		       write_mode_to_str(write_mode), signal, status);

	      struct support_capture_subprocess result
		= type == subprocess ? do_subprocess (&test)
				     : do_subprogram (&test);

              check_stream ("stdout", result.out_buf, test.out);
              check_stream ("stderr", result.err_buf, test.err);

              if (test.signal != 0)
                {
                  if (!WIFSIGNALED (result.status))
		    {
		      fprintf (stderr, "Not signalled!\n");
		      exit (1);
		    }
                  assert (WTERMSIG (result.status) == test.signal);
                }
              else
                {
                  assert (WIFEXITED (result.status));
		  if (!(WEXITSTATUS (result.status) == test.status))
		    {
		      fprintf (stderr, "Unexpected exist status result:%d expected:%d\n",
			       WEXITSTATUS (result.status),
			       test.status);
		    }
                }
	      support_capture_subprocess_free (&result);
              free (test.out);
              free (test.err);
            }
  return 0;
}

int
main (int argc, char *argv[])
{
  /* We must have either:

     - one or four parameters if called initially:
       + argv[1]: path for ld.so        optional
       + argv[2]: "--library-path"      optional
       + argv[3]: the library path      optional
       + argv[4]: the application name

     - six parameters left if called through re-execution:
       + argv[1]: the application name
       + argv[2]: the stdout to print
       + argv[3]: the stderr to print
       + argv[4]: the write mode to use
       + argv[5]: the signal to issue
       + argv[6]: the exit status code to use

     * When built with --enable-hardcoded-path-in-tests or issued without
       using the loader directly.
  */

  int opt;
  int long_index = 0;

  static struct option long_options[] = {
  /* name          , has_arg    , flags   , val */
    {"restart",      no_argument, &restart, 1},
    {"direct",       no_argument, NULL,    10},
    {0, 0, 0, 0}
  };

  while ((opt = getopt_long (argc, argv, "+",
			     long_options, &long_index)) != -1)
    {
      switch (opt)
	{
	  case '?':
	    fprintf (stderr, "Unkown arg: '%c'\n", opt);
	    fprintf (stderr, "Usage: %s [--restart] ld.so --library-path libpath app-name \n", argv[0]);
	    exit (EXIT_FAILURE);
	  default:
	    break;
	}
    }

  /* Correct for the possible parameters.  */
  argv[optind - 1] = argv[0];
  argv += optind - 1;
  argc -= optind - 1;

  if (argc != (restart ? 6 : 5) && argc != (restart ? 6 : 2))
    {
      fprintf (stderr, "wrong number of arguments (%d)", argc);
      exit (EXIT_FAILURE);
    }

  if (restart)
    {
      handle_restart (argv[1],  /* stdout  */
		      argv[2],  /* stderr  */
		      argv[3],  /* write_mode  */
		      argv[4],  /* signal  */
		      argv[5]); /* status  */
    }

  initial_argv[0] = argv[1]; /* path for ld.so  */
  initial_argv[1] = argv[2]; /* "--library-path"  */
  initial_argv[2] = argv[3]; /* the library path  */
  initial_argv[3] = argv[4]; /* the application name  */
  initial_argv[4] = NULL;

  do_multiple_tests (subprocess);
  do_multiple_tests (subprogram);

  return 0;
}

