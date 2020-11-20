## Cancel 24 tests

In the glibc cancel24 we were getting a failures due to exceptions
not getting caught.  This test zero's in on exception throwing and
catching.

### Tests

 - throw - simple test that throws and catches in a function - PASSES
 - throw-pthread - simple case that throws and catches in a pthread - PASSES
 - throw-pthread-sem - sam as cancel24, throws and catches in a pthread
   while waiting in a semaphore - PASSES

Currently this points to exceptions in semaphores not being properly raised?

### Building and Running

Running on your local platform

```
make
./throw
```

Running on baremetal sim

```
make CROSS_COMPILE=or1k-elf-
or1k-elf-run ./throw
```

Running on glibc qemu sim

```
make CROSS_COMPILE=or1k-smh-linux-gnu-
~/work/gnu-toolchain/scripts/qemu-or1k-libc ./throw
```

### Solution to failure

The issue was that the sem_wain (futex-internals part) was not built with
the `-fexceptions` flag.  This was a bug with glibc itself not or1k.
