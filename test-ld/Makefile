
CROSS_COMPILE ?= or1k-smh-linux-gnu-

#LD=/home/shorne/work/gnu-toolchain/build-binutils/ld/ld-new
LD            = $(CROSS_COMPILE)ld
GXX           = $(CROSS_COMPILE)g++
READELF       = $(CROSS_COMPILE)readelf

all: libfunc.so
clean:
	rm -f libfunc.so func.o func.s

libfunc.so: func.o
	$(LD) func.o -shared -o libfunc.so

func.o: func.c
	$(GXX) -O2 -fPIC -c -o func.o func.c

func.s: func.c
	$(GXX) -O2 -fPIC -S -o func.s func.c

show-segments: libfunc.so
	$(READELF) -l libfunc.so

show-relocs: libfunc.so func.o
	$(READELF) -r func.o
	$(READELF) -r libfunc.so

show-debug: func.o
	$(READELF) --debug-dump=frames func.o
	$(READELF) -r func.o | grep -A6 eh_frame

#g++
#  -x c++
#  -g
#  -O2
#  -I.
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/config
#  -DLOCALEDIR="\"/home/shorne/work/gnu-toolchain/local/share/locale\""
#  -DHAVE_CONFIG_H
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../include/opcode
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../readline/..
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../zlib
#  -I../bfd
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../bfd
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../include
#  -I../libdecnumber
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../libdecnumber
#  -I/home/shorne/work/gnu-toolchain/binutils-gdb/gdb/../gnulib/import
#  -I../gnulib/import
#  -DTUI=1
#  -I/usr/include/python2.7
#  -I/usr/include/python2.7
#  -Wall
#  -Wpointer-arith
#  -Wno-unused
#  -Wunused-value
#  -Wunused-variable
#  -Wunused-function
#  -Wno-switch
#  -Wno-char-subscripts
#  -Wempty-body
#  -Wunused-but-set-parameter
#  -Wunused-but-set-variable
#  -Wno-sign-compare
#  -Wno-error=maybe-uninitialized
#  -Wsuggest-override
#  -Wimplicit-fallthrough=3
#  -Wduplicated-cond
#  -Wshadow=local
#  -Wdeprecated-copy
#  -Wdeprecated-copy-dtor
#  -Wredundant-move
#  -Wformat
#  -Wformat-nonliteral
#  -Werror
#  -c
#  -o filename-seen-cache.o
#  -MT filename-seen-cache.o
#  -MMD
#  -MP
#  -MF ./.deps/filename-seen-cache.Tpo
#  /home/shorne/work/gnu-toolchain/binutils-gdb/gdb/filename-seen-cache.c
