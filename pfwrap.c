#include <stdarg.h>
#include <stdio.h>

int
pf_wrapper (int ret, char *fmt, ...)
{
  va_list args;

  va_start (args, fmt);
  vprintf (fmt, args);
  va_end (args);
  return ret;
}

