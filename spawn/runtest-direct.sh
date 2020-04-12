#!/bin/sh

# Make and run the test for openrisc

DIR=`dirname $0`

LIBRARY_PATH=/home/shorne/work/gnu-toolchain/build-glibc

make CROSS_COMPILE=or1k-smh-linux-gnu-

if [ "$STRACE" ] ; then
  RUN_STRACE="/strace -fo /strace.txt"
  export STRACE=
fi

run() {
  echo "exec: $@"
  exec $RUN_STRACE $@
}

if [ "$1" ] ; then
  run /home/shorne/work/gnu-toolchain/build-glibc/elf/ld.so.1 \
    --library-path $LIBRARY_PATH \
    $DIR/test_capture_subprocess $@
else
  run /home/shorne/work/gnu-toolchain/build-glibc/elf/ld.so.1 \
    --library-path $LIBRARY_PATH \
    $DIR/test_capture_subprocess
fi
