#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Roulette Table
//   1 - 36 , 0 , 00
//    1-10 - odd  red
//   11-18 - even red
//   19-28 - odd  red
//   29-36 - even red
//   0,00  - green

enum color {
  RED,
  BLACK,
  GREEN,
  CNONE
};

static const char * color_strings[] = {"red", "black", "green", "none"};

static const char * color_fmt[] = {"\033[0;31m%2s\033[0m",
                                             "%2s",
                                   "\033[0;32m%2s\033[0m",
                                             "%2s"};


enum parity {
  EVEN,
  ODD,
  PNONE
};

static const char * parity_strings[] = {"even", "odd", "none"};
static const char * value_strings[] =  {
                                         "0",  "1",  "2",  "3",  "4",  "5",  "6",
                                         "7",  "8",  "9", "10", "11", "12", "13",
                                        "14", "15", "16", "17", "18", "19", "20",
                                        "21", "22", "23", "24", "25", "26", "27",
                                        "28", "29", "30", "31", "32", "33", "34",
                                        "35", "36", "00",
                                       };

#define RT_VALUES 38
#define RT_MAX_HISTORY 10

static int rt_spin_history [RT_MAX_HISTORY];
static int rt_spin_count = 0;

void rt_init() {
  srand(time(NULL));
}

int rt_spin() {
  int value = rand() % RT_VALUES;
  rt_spin_history[rt_spin_count++ % RT_MAX_HISTORY] = value;
  return value;
}

/*
 * Get a spin value from history.  If HISTORY is requested
 * greater than RT_MAX_HISTORY or rt_spin_count -1 is returned
 * so watch out.
 *  HISTORY the history to go back, 0 for last roll
 *          1 for one role before.
 */
int rt_history_value(int history) {
  if (history >= RT_MAX_HISTORY)
    return -1;
  if (history >= rt_spin_count)
   return -1;

  return rt_spin_history[((rt_spin_count - 1 - history) % RT_MAX_HISTORY)];
}

enum color rt_color(int value) {
  if (value == 0 || value == 37) {
    return GREEN;
  }
  enum parity p = value % 2;

  if (value <= 10) {
    return p == ODD ? RED : BLACK;
  } else if (value <= 18) {
    return p == EVEN ? RED : BLACK;
  } else if (value <= 28) {
    return p == ODD ? RED : BLACK;
  } else {
    return p == EVEN ? RED : BLACK;
  }
}

/* Need to free this one */
char * rt_color_fmt(const char * string, enum color vcolor) {
  char buf[64];

  sprintf(buf, color_fmt[vcolor], string);

  return strdup(buf);
}

const char * rt_color_string(int value) {
  return color_strings[rt_color(value)];
}

const char * rt_value_string(int value) {
  return value_strings[value];
}

char * rt_value_fmt(int value) {
  if (value == -1)
   return strdup(" -");
  return rt_color_fmt(rt_value_string(value), rt_color(value));
}

// Strategy - always bet on black
static enum color abob_bet_color() {
  return BLACK;
}
static int abob_bet_amount() {
  return 100;
}

// Strategy - 3 red, black
enum color threered_bet_color() {
  if (rt_spin_count < 4)
    return CNONE;

  if (RED == rt_color(rt_history_value(0))
      && RED == rt_color(rt_history_value(1))
      && RED == rt_color(rt_history_value(2))
      && BLACK == rt_color(rt_history_value(3)))
    return BLACK;
  return CNONE;
}

static int threered_bet_amount() {
  if (threered_bet_color() == CNONE)
    return 0;
  return 100;
}

static void test() {
  for (int i = 0; i < 11; i++) {
    int value = rt_spin();

    if (i < 8) {
      printf ("Spin %2d:    %s\n", i+1, rt_value_fmt(value));
    } else {
      printf ("Spin %2d: %s %s\n", i+1, rt_value_fmt(rt_history_value(1)), rt_value_fmt(value));
    }
  }
}

#define bet_color threered_bet_color
#define bet_amount threered_bet_amount

///////////
// Start //

int main() {
  // Start with 10_000 dollarts.
  int balance = 10000;

  rt_init();

  for (int i = 0; i < 300; i++) {

    // Run strategy to get bet
    int bamt = bet_amount();
    enum color bc = bet_color();

    // Simulate the spin
    int value = rt_spin();
    bool won = false;

    // Check for winner
    if (rt_color(value) == bc) {
      won = true;
      balance += bamt;
    } else {
      balance -= bamt;
    }

    printf ("Spin %2d: %s %s %s %s %s : %s - bet: %s %5d : %s - balance: %d\n",
            i+1,
            rt_value_fmt(rt_history_value(5)),
            rt_value_fmt(rt_history_value(4)),
            rt_value_fmt(rt_history_value(3)),
            rt_value_fmt(rt_history_value(2)),
            rt_value_fmt(rt_history_value(1)),
            rt_value_fmt(value),
            color_strings[bc], bamt, won ? " WIN" : "LOSE", balance);
  }
  return 0;
}
