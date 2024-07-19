#ifndef GMP_TIMER_H
#define GMP_TIMER_H

#include "event.h"

typedef struct Timers Timers;

struct Timers {
  Event **heap;
  int len;
  int cap;
};

void Timers_init(Timers *);
long Timers_check(Timers *);

#endif
