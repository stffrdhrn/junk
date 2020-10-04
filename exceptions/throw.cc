#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static volatile bool destr_called;
static volatile bool except_caught;

struct monitor
{
  // gcc is broken and would generate a warning without this dummy
  // constructor.
  monitor () { }
  ~monitor() { destr_called = true; }
};

static void do_throw() { throw 99; }

static void *
tf ()
{
  try
    {
      monitor m;

      while (1)
	do_throw();
    }
  catch (...)
    {
      except_caught = true;
    }

  return NULL;
}

int main() {

  tf ();

  if (!except_caught) {
    puts ("failed to catch exception");
  }
  if (!destr_called) {
    puts ("failed to call destructor");
  }

  puts ("done");

  return 0;
}
