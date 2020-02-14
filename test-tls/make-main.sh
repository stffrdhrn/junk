or1k-elf-gcc -g -O2 -c ../test.c
or1k-elf-gcc -g -O2 -c ../main.c
or1k-elf-objdump -dr main.o
or1k-elf-objdump -dr test.o
or1k-elf-gcc -g -O2 main.o test.o
