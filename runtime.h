#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#define G_MAX 256

typedef struct G G;
typedef struct M M;
typedef struct P P;
typedef struct Sched Sched;
typedef struct Lock Lock;
typedef struct Event Event;
typedef struct Timers Timers;
typedef void *(*Allocator)(void *, size_t);

struct Lock {
  uintptr_t key;
};

struct Sched {
  Lock lock;

  Allocator alloc;
  P *allp;
  int mmax;

  G *ghead;
  G *gtail;
  int glen;
  

  uint64_t nextid;
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
  G *next;
  M *m;
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

void Locker_lock(Lock *);
void Locker_unlock(Lock *);

uint64_t Timers_check(Timers *);

void Poller_init(void);
G *Poller_poll(uint64_t);
int Poller_open(Event *);

#endif
