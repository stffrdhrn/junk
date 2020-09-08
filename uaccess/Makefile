BIN1 = test_uaccess
BIN1.SRC = test.c

FLAGS = -I. -O3

WARN_EXTRA = -Werror

# make CROSS_COMPILE=or1k-smh-linux-gnu-
CROSS_COMPILE ?= or1k-elf-

check: $(BIN1)
	$(CROSS_COMPILE)run $^

# Make file from Richard Bradley https://github.com/hirdrac/makefile
include Makefile.mk
