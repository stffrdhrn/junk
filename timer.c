#include <time.h>
#include <stdio.h>

static long to_millis(struct timespec *time) {
  return (time->tv_sec * 1000) + (time->tv_nsec / 1000000);
}

static long to_micro(struct timespec *time) {
  return (time->tv_sec * 1000000) + (time->tv_nsec / 1000);
}

int main() {
  int i, j = 0;

  struct timespec before, after;

  clock_gettime(CLOCK_MONOTONIC, &before);
  /* Super complex algorithm */
  for (int i = 0; i < 100; i++) {
    j = (j+1) * (j+2);
  }
  clock_gettime(CLOCK_MONOTONIC, &after);

  printf("time to run algorithm %ld uSecs\n", to_micro(&after) - to_micro(&before));  

  return 0;
}
