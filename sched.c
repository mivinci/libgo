
#include <stdlib.h>
#include <string.h>

#include "gmp.h"
#include "runtime.h"

static Sched sched;

extern void gogo(Gobuf *) asm("gogo");
extern void gosave(Gobuf *) asm("gosave");
extern G *getg(void) asm("getg");
static G *gget(P *);
static void gput(P *, G *);
static G *globgpop(void);
static G *globgget(P *);
static void globgpush(G *);
static G *gfget(void);
static void gfput(G *);
static void ready(P *, G *);
static void lock(Mutex *);
static void unlock(Mutex *);
static void gexit(void);

static Sched_init(void) {
  
}

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

static void lock(Mutex *mp) {}

static void unlock(Mutex *mp) {}

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
  char *sp;
  void **ra;

  gp = gfget();
  if (!gp)
    gp = (G *)malloc(size + sizeof *gp);
  if (!gp)
    panic("GMP_spawn: out of memory");

  gp->id = atomic_fetch_add(&sched.nextgid, 1);
  gp->stackguard = gp + 1;
  gp->state = G_IDLE;
  gp->next = 0;
  gp->ev.fd = -1;
  gp->ev.when = -1;
  gp->ev.g = gp;

  sp = (char *)gp->stackguard + size - sizeof(void *);
  sp = (char *)((uintptr_t)sp & -16L);

  /**
   * NOTE: macOS requires the stack to be 16 bytes allgned
   * on 64-bit machines, so we have to leave 8 bytes for
   * instruction `push %rbp` that happens at the beginning
   * of each coroutine.
   */
  sp -= 8;

  ra = (void **)sp;
  *ra = (void *)gexit;

  gp->ctx.sp = (uintptr_t)sp;
  gp->ctx.pc = (uintptr_t)f;
  gp->ctx.arg = (uintptr_t)arg;

  ready(getg()->m->p, gp);
}

static void gexit(void) {
  G *gp;

  gp = getg();
  gp->state = G_DEAD;
  gfput(gp);
  schedule();
}
