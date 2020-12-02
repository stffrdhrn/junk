# Investigate tst-cancel7

This GLIBC test was failing on OpenRISC, this is a standalone
binary to allow for easier testing.

## So Far

Initial investigation shows that it looks like the child thread
`pd->tid` value gets set to -1 (65535) sometime during the call
to `system()`.

`tst-cancel.strace-sleep4` - collected from or1k linux

```
13994 07:32:08 close(3)                 = 0
13994 07:32:08 mprotect(0x309ea000, 8192, PROT_READ) = 0
13994 07:32:08 getpid()                 = 13994

## Try to kill self thread group, failing with ESRCH
13994 07:32:08 tgkill(13994, 65535, SIGRTMIN) = -1 ESRCH (No such process)
13994 07:32:08 write(1, "error: xpthread_check_return.c:3"..., 35) = 35
13994 07:32:08 write(1, "pthread_cancel: No such process", 31) = 31
```

This test allowed us to collect an strace from x86, and we can see
the child tid does not get set to -1.

`tst-cancel7.strace.x86`

```
2923933 close(3)                        = 0
2923933 mprotect(0x7fc39c011000, 4096, PROT_READ) = 0
2923933 munmap(0x7fc39ca05000, 195197)  = 0
2923933 getpid()                        = 2923933
// Here we see the kill is correct 2923934 (not -1, so that points to or1k being
// wrong here)
2923933 tgkill(2923933, 2923934, SIGRTMIN) = 0
2923933 write(1, "Cancelled pidfile thread. 0\n", 28 <unfinished ...>
```

## Solution

The __or1k_cancel assembly wrapper had code to set TID to -1
when CLONE_VM arg was set.

Removing this code solved the issue.

To detect this I wrote a nifty qemu plugin 'tracewatch.c'.

Run QEMU with argument:
  "-plugin $QEMU/tests/plugin/libtracewatch.so,arg=store,arg=addr,arg=0x30007478,arg=addr_mask,arg=0xf000ffff,arg=tracepath,arg=-"
