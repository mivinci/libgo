
#include "runtime.h"

static Sched sched;

extern G *getg(void) asm("getg");
static G *gget(P *);
static void gput(P *, G *);
static G *globgpop(void);
static G *globgget(P *);
static void globgpush(G *);
void lock(Lock *);
void unlock(Lock *);

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
  ready(gp->next);
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


void lock(Lock *lp) {

}

void unlock(Lock *lp) {
  
}

