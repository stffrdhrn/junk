
CROSSTOOL ?= or1k-elf-
GCC ?= $(CROSSTOOL)gcc
SIM ?= $(CROSSTOOL)run
SPARSE ?= sparse

CFLAGS = -Wall -g -O2
SPARSE_FLAGS = -O2 --arch=openrisc -m32

all: xchg_test simple_test

run: xchg_test
	$(SIM) xchg_test

check: xchg_test.c simple_test.c
	$(SPARSE) $(SPARSE_FLAGS) xchg_test.c
	$(SPARSE) $(SPARSE_FLAGS) simple_test.c

xchg_test: xchg_test.c
	$(GCC) $(CFLAGS) -o xchg_test xchg_test.c

clean:
	rm -f xchg_test
