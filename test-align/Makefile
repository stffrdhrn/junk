# Makefile for TLS examples
# Targets:
#  - 'all'   build everything
#  - 'check' run tests
#  - 'clean' delete all build artifacts

CROSS_COMPILE ?= or1k-smh-linux-gnu-
FPIC          ?= -fpic
CFLAGS        = -Wall -Og -g -I. $(FPIC)
LDFLAGS       = -Wl,-rpath=. -Wl,--export-dynamic -L. -lpthread
CC            = $(CROSS_COMPILE)gcc
OBJDUMP       = $(CROSS_COMPILE)objdump
READELF       = $(CROSS_COMPILE)readelf

MAIN_SRC      = tst-tls5.c tst-tls1.c
LIB_SRC       = tst-tls5moda.c \
		tst-tls5modb.c \
		tst-tls5modc.c \
		tst-tls5modd.c
HEADERS       = tst-tls5.h

dyn_objs     := $(patsubst %.c,%.o,$(LIB_SRC) $(MAIN_SRC))

libs         := $(patsubst tst-%.c,lib%.so,$(LIB_SRC))
main_exe     := $(patsubst %.c,%,$(MAIN_SRC))

all_exe      := $(main_exe)
all_asm      := $(patsubst %.o,%.S,$(dyn_objs))
all_sym      := $(patsubst %.so,%.sym,$(libs))
all_results  := $(patsubst %,test-results-%.out,$(all_exe))

all: all_objs all_libs all_exe

all_exe: $(all_exe)
all_libs: $(libs)
all_objs: $(dyn_objs)

## Building object files
%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

## Building shared libs
lib%.so: tst-%.o
	$(CC) -shared -o $@ $< $(LDFLAGS)

## Building executables
%: %.o
	$(CC) -o $@ $< $(LDFLAGS)

## Building debug artifacts
%.s: %.c
	$(CC) $(CFLAGS) -S $<

%.sym: %.so
	readelf -s $< > $@

## Test Suite
check: all test-results.out

test-results-%.out: %
	echo "Running $<..." > $@
	( LD_PRELOAD=libtls5moda.so:libtls5modb.so:libtls5modc.so:libtls5modd.so ./$< && echo ok || echo not ok )  >> $@
	( LD_PRELOAD=libtls5modb.so:libtls5moda.so:libtls5modc.so:libtls5modd.so ./$< && echo ok || echo not ok )  >> $@
	( LD_PRELOAD=libtls5modb.so:libtls5modc.so:libtls5moda.so:libtls5modd.so ./$< && echo ok || echo not ok )  >> $@
	( LD_PRELOAD=libtls5modb.so:libtls5modc.so:libtls5modd.so:libtls5moda.so ./$< && echo ok || echo not ok )  >> $@

test-results.out: $(all_results)
	cat $^ > test-results.out

## Cleanup
clean:
	rm -f *.so *.o *.out *.sym *.s $(all_exe)
