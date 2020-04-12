BIN1 = test_capture_subprocess
BIN1.SRC = test_capture_subprocess.c \
	support_subprocess.c \
	support_capture_subprocess.c

BIN2 = sigmask
BIN2.SRC = sigmask.c

WARN_EXTRA = -Werror

# make CROSS_COMPILE=or1k-smh-linux-gnu-
CROSS_COMPILE ?=

# Make file from Richard Bradley https://github.com/hirdrac/makefile
include Makefile.mk
