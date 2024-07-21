
#include <stdlib.h>
#include <string.h>

#include "gmp/gmp.h"
#include "lock.h"
#include "sched.h"

static Sched sched;
static __thread G *g;

extern void gogo(Gobuf *, void *) asm("gogo");
extern int gosave(Gobuf *) asm("gosave");
static G *gget(P *);
static void gput(P *, G *);
static G *globgpop(void);
static G *globgget(P *);
static void globgpush(G *);
static G *gfget(void);
static void gfput(G *);
static void ready(P *, G *);
static void lock(Lock *);
static void unlock(Lock *);
static void gexit(void);
static void resume(G *);

static G *getg(void) { return g; }
static void setg(G *gp) { g = gp; }

static Sched_init(void) {}

void schedule(void) {
  G *gp;
  M *mp;
  P *pp;
  uint64_t ns;

  mp = getg()->m;
  pp = mp->p;

top:
  /* Execute ready timer events */
  ns = Timers_check(&pp->t);

  /* Find a runnable G from the global queue every 61 ticks
     to prevent G starvation */
  if (pp->tick % 61 == 0 && sched.glen > 0) {
    lock(&sched.lock);
    gp = globgget(pp);
    unlock(&sched.lock);
    if (gp)
      goto end;
  }

  /* Find a runnable G from the local queue */
  gp = gget(pp);
  if (gp)
    goto end;

  /* Find a runnable G from the global queue */
  gp = globgget(pp);
  if (gp)
    goto end;

  /* Pick out runnable Gs from IO events */
  gp = Poller_poll(ns);
  if (!gp)
    goto top;

  /* Put all other runnable Gs into the local queue
     for the next round of scheduling */
  ready(pp, gp->next);
  gp->next = 0;

end:
  pp->tick++;
  /* Resume execution to the runnable G */
  resume(gp);
}

static G *gget(P *pp) {
  G *gp;

  if (pp->ghead == pp->gtail)
    return 0;

  gp = pp->g[pp->ghead];
  pp->ghead = (pp->ghead + 1) % G_MAX;
  return gp;
}

static void gput(P *pp, G *gp) {
  if ((pp->gtail + 1) % G_MAX == pp->ghead) {
    lock(&sched.lock);
    globgpush(gp);
    unlock(&sched.lock);
  }

  pp->g[pp->gtail] = gp;
  pp->gtail = (pp->gtail + 1) % G_MAX;
}

static void globgpush(G *gp) {
  if (!gp)
    return;
  if (sched.gtail)
    sched.gtail->next = gp;
  else
    sched.ghead = gp;
  sched.gtail = gp;
  sched.glen++;
}

static G *globgpop(void) {
  G *gp;

  gp = sched.ghead;
  if (gp) {
    sched.ghead = gp->next;
    gp->next = 0;
  }
  if (gp == sched.gtail)
    sched.gtail = 0;
  if (gp)
    sched.glen--;
  return gp;
}

static G *globgget(P *pp) {
  G *gp;
  int n, max;

  if (sched.glen == 0)
    return 0;

  n = sched.glen / sched.mmax;
  max = G_MAX - (pp->gtail - pp->ghead);
  if (n > max)
    n = max;

  gp = globgpop();
  n--;
  while (n--)
    gput(pp, globgpop());
  return gp;
}

static void ready(P *pp, G *gp) {
  for (; gp; gp = gp->next) {
    gp->state = G_RUNNABLE;
    gput(pp, gp);
  }
}

static void lock(Lock *mp) {}

static void unlock(Lock *mp) {}

static G *gfget(void) {
  G *gp;

  lock(&sched.lock);
  gp = sched.gfree;
  if (gp)
    sched.gfree = gp->next;
  unlock(&sched.lock);
  return gp;
}

static void gfput(G *gp) {
  if (!gp)
    return;

  memset(gp, 0, sizeof *gp);
  lock(&sched.lock);
  gp->next = sched.gfree;
  sched.gfree = gp;
  unlock(&sched.lock);
}

void GMP_spawn(GMP_Func f, void *arg, int size, int flags) {
  G *gp;
  uintptr_t sp;

  gp = gfget();
  if (!gp)
    gp = (G *)malloc(size + sizeof *gp);
  if (!gp)
    panic("GMP(spawn): out of memory");

  gp->id = atomic_fetch_add(&sched.nextgid, 1);
  gp->stack.la = (uintptr_t)(gp + 1);
  gp->stack.ha = gp->stack.la + size;
  gp->arg = arg;
  gp->state = G_IDLE;
  gp->flags = 0;
  gp->next = 0;
  gp->ev.fd = -1;
  gp->ev.when = -1;
  gp->ev.g = gp;

  sp = gp->stack.ha - sizeof(void *);
  sp = sp & -16L; /* Round down SP by 8 bytes */

  /**
   * NOTE: macOS requires the stack to be 16 bytes allgned
   * on 64-bit machines, so we have to leave 8 bytes for
   * instruction `push %rbp` that happens at the beginning
   * of each coroutine.
   */
  sp -= 8;

  gp->ctx.sp = sp;
  gp->ctx.pc = (uintptr_t)f;

  *((void **)sp) = (void *)gexit;

  ready(getg()->m->p, gp);
}

static void gexit(void) {
  G *gp;

  gp = getg();
  gp->state = G_DEAD;
  gfput(gp);
  schedule();
}

static G *checkstackgrow(G *gp) {
  G *newg;
  size_t size, newsize, newsp, sizeinuse;
  /* 512 bytes is a good choice to void memory overlap, hopefully */
  if (gp->ctx.sp - gp->stack.la >= 512)
    return gp;
  /* No G stack can be expaned to more than 2MB */
  size = gp->stack.ha - gp->stack.la;
  if (size > 0x200000UL)
    panic("GMP(checkstackgrow): stack overflow");
  /* Now we do actual stack expansion */
  sizeinuse = gp->stack.ha - gp->ctx.sp;
  newsize = size * 2;
  newg = (G *)malloc(newsize);
  if (!newg)
    panic("GMP(checkstackgrow): out of memory");
  memcpy(newg, gp, sizeof *gp);
  newg->stack.la = newg + 1;
  newg->stack.ha = newg->stack.la + newsize;
  newg->ctx.sp = newg->stack.ha - sizeinuse;
  memcpy((void *)newg->ctx.sp, (void *)gp->ctx.sp, sizeinuse);
  free(gp);
  return newg;
}

static void resume(G *gp) {
  /* 
   * Try if or not we need to expand the G's stack 
   * before switching to it. 
   */
  gp = checkstackgrow(gp);
  gp->state = G_RUNNING;
  setg(gp);
  gogo(&gp->ctx, gp->arg);
}

void GMP_yield(void) {
  G *gp;

  gp = getg();
  if (gosave(&gp->ctx))
    return;
  gp->state = G_RUNNABLE;
  gput(gp->m->p, gp);
  schedule();
}
