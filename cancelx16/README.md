# Investigate tst-cancelx16

This GLIBC test was failing on OpenRISC, this is a standalone
binary to allow for easier testing.

## So Far

This test fails to call cleanup handlers registered by `pthread_cleanup_push()`,
there are two versions of the test in glibc:
  1. Test built without `-fexceptions` - WORKS
  1. Test build with `-fexceptions` - FAILS

The `-fexceptions` flag changes how `pthread_cleanup_push()` works,  when using
no flag jmpbuf is used.  When using the flag an exception mechanism is used:

From: `sysdeps/nptl/pthread.h`

```
/* Install a cleanup handler: ROUTINE will be called with arguments ARG
   when the thread is canceled or calls pthread_exit.  ROUTINE will also
   be called with arguments ARG when the matching pthread_cleanup_pop
   is executed with non-zero EXECUTE argument.

   pthread_cleanup_push and pthread_cleanup_pop are macros and must always
   be used in matching pairs at the same nesting level of braces.  */
#  define pthread_cleanup_push(routine, arg) \
  do {									      \
    struct __pthread_cleanup_frame __clframe				      \
      __attribute__ ((__cleanup__ (__pthread_cleanup_routine)))		      \
      = { .__cancel_routine = (routine), .__cancel_arg = (arg),	 	      \
	  .__do_it = 1 };

/* Remove a cleanup handler installed by the matching pthread_cleanup_push.
   If EXECUTE is non-zero, the handler function is called. */
#  define pthread_cleanup_pop(execute) \
    __clframe.__do_it = (execute);					      \
  } while (0)
```

Note this uses `__cleanup__` attribute, with function `__pthread_cleanup_routine`.

For ref: https://echorand.me/site/notes/articles/c_cleanup/cleanup_attribute_c.html

From:

```
 /* Function called to call the cleanup handler.  As an extern inline
   function the compiler is free to decide inlining the change when
   needed or fall back on the copy which must exist somewhere
   else.  */
__extern_inline void
__pthread_cleanup_routine (struct __pthread_cleanup_frame *__frame)
{
  if (__frame->__do_it)
    __frame->__cancel_routine (__frame->__cancel_arg);
}
```

So this is not being called.

## Thoughts

I think this may be similar to the exception bug we found in `exceptions`.

...

It was looking at the stack trace, starting from where we get the
cancel event:

```
(gdb) b uw_frame_state_for
Breakpoint 3 at 0x3003d43c: file ../../../libgcc/unwind-dw2.c, line 1256.
(gdb) c
Continuing.
[Detaching after fork from child process 178]
[New Thread 0x309ed420 (LWP 179)]

Thread 2 "tst-cancelx16-g" received signal SIG32, Real-time event 32.
[Switching to Thread 0x309ed420 (LWP 179)]
0x3015cde8 in __GI___libc_fcntl64 (fd=3, cmd=14)
    at ../sysdeps/unix/sysv/linux/fcntl64.c:49
warning: Source file is more recent than executable.
49          return SYSCALL_CANCEL (fcntl64, fd, cmd, arg);
(gdb) bt
#0  0x3015cde8 in __GI___libc_fcntl64 (fd=3, cmd=14)
    at ../sysdeps/unix/sysv/linux/fcntl64.c:49
#1  0x3015ced4 in __lockf64 (fd=<optimized out>, cmd=<optimized out>,
    len64=<optimized out>) at lockf64.c:52
#2  0x00002af0 in tf (arg=<optimized out>) at tst-cancelx16.c:54
#3  0x30050548 in start_thread (arg=<optimized out>) at pthread_create.c:463
#4  0x3017238c in __or1k_clone () from /lib/libc.so.6
```

I checked .o files for `__GI___libc_fcntl64`, and `__lockf64`.  I found that lockf64
was missing `eh_frames`.  Also here the Makefile was missing `-fexceptions`.

Adding it fixed the issue.
