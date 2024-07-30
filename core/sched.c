
#include <stdlib.h>
#include <string.h>

#include "gmp/gmp.h"

#include "core.h"
#include "lock.h"
#include "sched.h"

static Sched sched;
static __thread G *g;

extern void goinit(Gobuf *) asm("goinit");
extern void gogo(Gobuf *) asm("gogo");
extern int gosave(Gobuf *) asm("gosave");
static G *gget(P *);
static void gput(P *, G *, bool);
static G *globgpop(void);
static G *globgget(P *);
static void globgpush(G *);
static G *gfget(void);
static void gfput(G *);
static void lock(Lock *);
static void unlock(Lock *);
static void goexit(void);
static void resume(G *);

static G *getg(void) { return g; }
static void setg(G *gp) { g = gp; }

static void Sched_init(int np) {
  if (!np)
    panic("GMP(Sched_init): zero P");

  Lock_init(&sched.lock);

  sched.allp = (P *)malloc(np * sizeof(P));
  if (sched.allp)
    panic("GMP(Sched_init): malloc");

  list_head_init(&sched.allg);
  sched.mmax = np;
  sched.ghead = 0;
  sched.gtail = 0;
  sched.glen = 0;
  sched.gfree = 0;
  sched.nextgid = ATOMIC_VAR_INIT(0);
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

  gp = pp->nextg;
  if (gp) {
    pp->nextg = 0;
    return gp;
  }

  if (pp->ghead == pp->gtail)
    return 0;

  gp = pp->g[pp->ghead];
  pp->ghead = (pp->ghead + 1) % len(pp->g);
  return gp;
}

static void gput(P *pp, G *gp, bool next) {
  G *oldnextg;

  if (next)
    goto put;

  oldnextg = pp->nextg;
  if (oldnextg) {
    pp->nextg = gp;
    gp = oldnextg;
  }

put:
  if ((pp->gtail + 1) % len(pp->g) == pp->ghead) {
    lock(&sched.lock);
    globgpush(gp);
    unlock(&sched.lock);
    return;
  }
  pp->g[pp->gtail] = gp;
  pp->gtail = (pp->gtail + 1) % len(pp->g);
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
  max = len(pp->g) - (pp->gtail - pp->ghead);
  if (n > max)
    n = max;

  gp = globgpop();
  n--;
  while (n--)
    gput(pp, globgpop(), false);
  return gp;
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
  size_t size;
  if (!gp)
    return;

  /* G freelist can only have standard Gs */
  size = gp->stack.ha - gp->stack.la;
  if (size != STK_MIN) {
    list_del(&gp->link);
    free(gp);
    return;
  }

  memset(gp, 0, sizeof *gp);
  lock(&sched.lock);
  gp->next = sched.gfree;
  sched.gfree = gp;
  unlock(&sched.lock);
}

static G *spawn(uintptr_t fn, void *arg, int flags) {
  G *newg;
  uintptr_t sp;

  newg = gfget();
  if (!newg)
    newg = (G *)malloc(STK_MIN + CTX_MAX + sizeof *newg);
  if (!newg)
    panic("GMP(spawn): out of memory");

  lock(&sched.lock);
  list_add(&newg->link, &sched.allg);
  unlock(&sched.lock);

  newg->id = atomic_fetch_add(&sched.nextgid, 1);
  newg->buf.ctx = (uintptr_t)(newg + 1);
  newg->stack.la = newg->buf.ctx + CTX_MAX;
  newg->stack.ha = newg->stack.la + STK_MIN;
  newg->arg = arg;
  newg->state = G_IDLE;
  newg->flags = flags;
  newg->next = 0;
  newg->ev.fd = -1;
  newg->ev.when = -1;
  newg->ev.g = newg;

  sp = newg->stack.ha - PTR_SIZE; /* Leave space for the return address */
  sp = align_down(sp, STK_ALIGN);
  newg->buf.sp = sp;
  newg->buf.pc = fn;
  goinit(&newg->buf);
  return newg;
}

void GMP_spawn(GMP_Func f, void *arg, int flags) {
  G *newg;
  newg = spawn((uintptr_t)f, arg, flags);
  gput(getg()->m->p, newg, true);
}

void GMP_exit(int status) {
  G *gp;

  (void)(status);
  gp = getg();
  gp->state = G_DEAD;
  gfput(gp);
  schedule();
}

static G *checkstackgrow(G *gp) {
  G *newg;
  size_t size, newsize, newsp, sizeinuse;
  /* 512 bytes is a good choice to void memory overlap, hopefully */
  if (gp->buf.sp - gp->stack.la >= 512)
    return gp;
  /* No G stack can be expaned to more than 2MB */
  size = gp->stack.ha - gp->stack.la;
  if (size > 0x200000UL) /* 2MB */
    panic("GMP(checkstackgrow): stack overflow");
  /* Now we do actual stack expansion */
  sizeinuse = gp->stack.ha - gp->buf.sp;
  newsize = size * 2;
  newg = (G *)malloc(newsize);
  if (!newg)
    panic("GMP(checkstackgrow): out of memory");
  memcpy(newg, gp, sizeof *gp);
  newg->buf.ctx = newg + 1;
  newg->stack.la = newg->buf.ctx + CTX_MAX;
  newg->stack.ha = newg->stack.la + newsize;
  newg->buf.sp = newg->stack.ha - sizeinuse;
  memcpy((void *)newg->buf.sp, (void *)gp->buf.sp, sizeinuse);
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
  gogo(&gp->buf);
}

void GMP_yield(void) {
  G *gp;

  gp = getg();
  if (gosave(&gp->buf))
    return;
  gp->state = G_RUNNABLE;
  gput(gp->m->p, gp, false);
  schedule();
}
