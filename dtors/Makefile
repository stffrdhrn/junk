BIN1 = tst-align2
BIN1.SRC = tst-align2.c
BIN1.DEPS = libtst-alignmod2.so

BIN1.CFLAGS = -ltst-alignmod2

LIB1 = libtst-alignmod2
LIB1.TYPE = shared
LIB1.SRC = tst-alignmod2.c

WARN_EXTRA = -Werror

# make CROSS_COMPILE=or1k-smh-linux-gnu-
CROSS_COMPILE ?=

# Make file from Richard Bradley https://github.com/hirdrac/makefile
include Makefile.mk
