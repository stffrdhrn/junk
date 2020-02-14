#include <stdint.h>

union Mem {
  volatile uint8_t va;
  uint8_t a;
} mem;

int max_va(int b) {
  return mem.va > b ? mem.va : b;
}

int max_a(int b) {
  return mem.a > b ? mem.a : b;
}
