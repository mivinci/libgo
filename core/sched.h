#ifndef LIBGO_CORE_SCHED_H_
#define LIBGO_CORE_SCHED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/go/core.h"
#include "lock.h"
#include "types.h"

typedef struct G G;
typedef struct M M;
typedef struct P P;

typedef struct Sched Sched;
typedef struct Stack Stack;
typedef struct Gobuf Gobuf;

struct Sched {
  Lock lock;

  // Global G queue
  G *ghead;
  G *gtail;
  int ng;

  // Idle Ms
  M *midle;
  int nmidle;

  // Idle Ps
  P *pidle;
  int npidle;
};

struct Stack {
  uintptr la;
  uintptr ha;
};

struct Gobuf {
  uintptr sp;
  uintptr pc;
  uintptr lr;  // Linker register for arm64
  uintptr ctx;
  uintptr g;  // The G switched to
};

struct G {
  Gobuf sched;
  Stack stack;
  M *m;
  G *prev;
  G *next;
  G *allgnext;
  int status;
  uintptr gopc;
};

struct M {
  G *g0;
  P *p;
  M *next;
  int tid;
  uintptr arg[6];
};

struct P {
  P *next;
  M *m;
  G *g[256];
  int ghead;
  int gtail;
  G *gfree;
  int ngfree;
  int status;
};

extern void mcall(void (*)(G *)) __asm__("mcall");
extern void gospawn(Go_Func, ...) __asm__("gospawn");
extern void gostart(Gobuf *, Go_Func) __asm__("gostart");
extern int gosave(Gobuf *) __asm__("gosave");
extern void gogo(Gobuf *) __asm__("gogo");
void goexit(void);
void mstart(void);
void schedule(void);

#ifdef __cplusplus
};
#endif

#endif  // LIBGO_CORE_SCHED_H_
