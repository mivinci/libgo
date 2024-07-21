#ifndef GMP_SCHED_H
#define GMP_SCHED_H

#include <stddef.h>
#include <stdint.h>

#include "atomic.h"
#include "timer.h"

#define G_MAX 256

#define G_IDLE 0
#define G_RUNNABLE 1
#define G_RUNNING 2
#define G_WAITING 3
#define G_DEAD 4

#define EV_READ 1
#define EV_WRITE 2

typedef struct G G;
typedef struct M M;
typedef struct P P;
typedef struct Sched Sched;
typedef struct Gobuf Gobuf;
typedef struct Stack Stack;

struct Sched {
  Lock lock;

  P *allp;
  int mmax;

  G *ghead;
  G *gtail;
  int glen;

  G *gfree;

  atomic_int nextgid;
};

struct Gobuf {
  uintptr_t sp;
  uintptr_t pc;
};


/* G stack [la, ha) */
struct Stack {
  uintptr_t la;
  uintptr_t ha;
};

struct G {
  Stack stack;
  G *next;
  M *m;
  int state;
  int flags;
  int id;
  Event ev;
  Gobuf ctx;
  void *arg;
};

struct M {
  int tid;
  P *p;
};

struct P {
  Timers t;
  uint64_t tick;
  G *g[G_MAX];
  int ghead;
  int gtail;
};

#endif
