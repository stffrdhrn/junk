BIN1 = test_capture_subprocess
BIN1.SRC = test_capture_subprocess.c \
	support_subprocess.c \
	support_capture_subprocess.c

WARN_EXTRA = -Werror

# make CROSS_COMPILE=or1k-smh-linux-gnu-
CROSS_COMPILE ?=

gcc_cc = $(CROSS_COMPILE)gcc

# Make file from Richard Bradley https://github.com/hirdrac/makefile
include Makefile.mk
