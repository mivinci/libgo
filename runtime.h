#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "atomic.h"

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
typedef struct Mutex Mutex;
typedef struct Gobuf Gobuf;
typedef struct Event Event;
typedef struct Timers Timers;

struct Sched {
  Mutex lock;

  P *allp;
  int mmax;

  G *ghead;
  G *gtail;
  int glen;

  G *gfree;
  
  atomic_int nextgid;
};

struct Mutex {
  
};

struct Gobuf {
  uintptr_t sp;
  uintptr_t pc;
  uintptr_t arg;
};

struct Event {
  int fd;
  long when;
  long period;
  void (*f)(void *, long);
  void *arg;
  G *g;
};

struct Timers {
  Event **heap;
  int len;
  int cap;
};

struct G {
  void *stackguard;
  G *next;
  M *m;
  int state;
  int id;
  Event ev;
  Gobuf ctx;
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

void Timers_init(Timers *);
uint64_t Timers_check(Timers *);

void Mutex_init(Mutex *);
void Mutex_lock(Mutex *);
void Mutex_unlock(Mutex *);

void Poller_init(void);
G *Poller_poll(uint64_t);
int Poller_open(Event *);

#endif
