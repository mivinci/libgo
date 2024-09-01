#include "sched.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "atomic.h"
#include "sys.h"

__thread G *g;  // Current G running

static G g0;
static M m0;
static P *allp;
static G *allg;        // For debug
static Lock allglock;  // Protects allg
static Sched sched;

enum {
  kGIdle,
  kGRunnable,
  kGRunning,
  kGWaiting,
  kGDead,
};

enum {
  kPIdle,
  kPRunning,
};

static void setup_allp(int);
static void acquirep(P *);
static P *releasep(void);
static P *pidleget(void);
static void pidleput(P *);

// Set up and bind g0 with m0, called at asm_xxx.s
void setup_g0m0(uintptr sp) {
  // Setup g0 stack
  g0.stack.ha = sp;
  g0.stack.la = sp - (8 * 1024);  // 8KB

  // Bind g0 and m0
  g0.m = &m0;
  m0.g0 = &g0;

  // Set the current G running
  g = &g0;
}

// Set up the scheduler, called at asm_xxx.s
// This function must be called after calling `setup_sys`
void setup_sched(void) {
  int n, np;
  char *p;

  memset(&sched, 0, sizeof sched);
  Lock_init(&sched.lock);

  np = sys_ncpu();
  p = getenv("GOMAXPROC");
  if (p && (n = atoi(p)) > 0) {
    if (n > np) np = n;
  }
  setup_allp(np);
}

void setup_allp(int np) {
  P *p;
  int i;

  allp = malloc(np * sizeof(P));
  GOASSERT(allp);

  for (i = np - 1; i > 0; i--) {
    p = allp + i;
    memset(p, 0, sizeof(P));
    p->status = kPIdle;
    pidleput(p);
  }
  // Bind the first P with this M (m0)
  acquirep(allp + i);
}

void setup_sig(void) {
  GOASSERT(g->m == &m0, "Bad m running setup_sig");
  // TODO: register signal handler
}

void acquirep(P *p) {
  GOASSERT(p->m || p->status == kPIdle, "P already in use");
  g->m->p = p;
  p->m = g->m;
  p->status = kPRunning;
}

P *releasep(void) {
  P *p;

  p = g->m->p;
  GOASSERT(p->m == g->m || p->status == kPRunning, "Bad P to release");
  g->m->p = NULL;
  p->m = NULL;
  p->status = kPIdle;
  return p;
}

static P *pidleget(void) {
  P *p;

  p = sched.pidle;
  if (p) {
    sched.pidle = p->next;
    GOADDX(&sched.npidle, -1);
  }
  return p;
}

static void pidleput(P *p) {
  p->next = sched.pidle;
  sched.pidle = p;
  GOADDX(&sched.npidle, 1);
}

static G *gfget(P *p) {
  G *gp;

  gp = p->gfree;
  if (gp) {
    p->gfree = gp->next;
    p->ngfree--;
  }
  return gp;
}

static void gfput(P *p, G *gp) {
  g->next = p->gfree;
  p->gfree = g;
  p->ngfree++;
}

static G *globgget(void) {
  G *gp;

  // TODO:
  return gp;
}

// Put a G on the global G queue.
// When calling, sched.lock MUST be held.
static void globgput(G *gp) {
  gp->next = NULL;
  if (sched.gtail)
    sched.gtail->next = gp;
  else
    sched.ghead = gp;
  sched.gtail = gp;
  sched.ng++;
}

// Try put a G on a local G queue.
// If it's full, put onto the global G queue.
// Executed only by the owner P.
static void gput(P *p, G *gp) {
  int h, t;

  h = Atomic_load(&p->ghead);
  t = p->gtail;
  if (t - h < LEN(p->g)) {
    p->g[t % LEN(p->g)] = gp;
    Atomic_store(&p->gtail, t + 1);
    return;
  }
  // TODO: Put gp on global G queue
}

static G *gget(P *p) {
  G *gp;
  int h, t;

  for (;;) {
    h = Atomic_load(&p->ghead);
    t = p->gtail;
    if (t == h) return NULL;
    gp = p->g[h & LEN(p->g)];
    if (Atomic_cas(&p->ghead, h, h + 1)) return gp;
  }
}

static void allgadd(G *gp) {
  Lock_acquire(&allglock);
  gp->allgnext = allg;
  allg = gp;
  Lock_release(&allglock);
}

static G *malg(int size) {
  G *newg;
  void *stk;
  // TODO: use memory pool
  GOASSERT(g == g->m->g0, "Bad G running malg");
  newg = malloc(sizeof(G));
  GOASSERT(newg);
  stk = malloc(size);
  GOASSERT(stk);
  newg->stack.la = (uintptr)stk;
  newg->stack.ha = (uintptr)(stk + size);
  return newg;
}

// gospawn continuation on M
void gospawn_m(G *) {
  Go_Func fn;
  int size;
  void *argp, *callerpc;
  G *newg;
  P *p;
  char *sp;

  fn = (Go_Func)(g->m->arg[0]);
  size = (int)(g->m->arg[1]);
  argp = (void *)(g->m->arg[2]);
  callerpc = (void *)(g->m->arg[3]);

  // Round up to 16 bytes
  size = (size + 15) & ~15;

  // GOPTRSIZ is to store LR (arm64) or RA (x86_64)
  // 256 is to leave extra space for no reason
  GOASSERT(fn, "NULL fn");
  GOASSERT(size < (GOSTKMIN - GOPTRSIZ - 256), "too many function arguments");

  p = g->m->p;
  if (!(newg = gfget(p))) {
    newg = malg(GOSTKMIN);
    allgadd(newg);
  }

  sp = (char *)newg->stack.ha;
  sp -= size;
  memmove(sp, argp, size);

  newg->sched.sp = (uintptr)sp;
  newg->sched.pc = (uintptr)goexit;
  gostart(&newg->sched, fn);
  newg->gopc = (uintptr)callerpc;

  gput(p, newg);
}

void gospawn1(Go_Func fn, int size, void *argp, void *callerpc) {
  g->m->arg[0] = (uintptr)fn;
  g->m->arg[1] = (uintptr)size;
  g->m->arg[2] = (uintptr)argp;
  g->m->arg[3] = (uintptr)callerpc;
  mcall(gospawn_m);
}

void goexit(void) {}

void mstart(void) {
  GOASSERT(g == g->m->g0, "Bad g running mstart");
  // Save context for `mcall`
  gosave(&g->sched);
  // Register signal handler if this M is m0
  if (g->m == &m0) setup_sig();
  // Initialize this M
  g->m->tid = sys_tid();
  // Once we call `schedule`, we are never coming back
  schedule();
}

void schedule(void) {
  // TODO:
}
