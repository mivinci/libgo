#ifndef GMP_LOCK_H
#define GMP_LOCK_H

#define LOCKED 1
#define UNLOCKED 0
#define SPIN 4

#ifdef USE_PTHREAD
#include <pthread.h>
#define pthread_mutex_t Lock;
#define Lock_lock(p) pthread_mutex_lock(p)
#define Lock_unlock(p) pthread_mutex_unlock(p)
#else
#include "atomic.h"
#include <sched.h> /* by POSIX */
#define Cas(p, v1, v2)                                                         \
  atomic_compare_exchange_weak_explicit((p), (v1), (v2), memory_order_acquire, \
                                        memory_order_acquire)
typedef atomic_int Lock;

void Lock_init(Lock *p) { atomic_init(p, UNLOCKED); }

void Lock_lock(Lock *p) {
  int old, i;

  for (;;) {
    /* Spin for lock */
    old = UNLOCKED;
    for (i = 0; i < SPIN; i++) {
      if (Cas(p, &old, LOCKED))
        return;
    }
    /* Schedule for lock */
    old = UNLOCKED;
    for (i = 0; i < SPIN; i++) {
      if (Cas(p, &old, LOCKED))
        return;
    }
    sched_yield();
  }
}

void Lock_unlock(Lock *p) {
  atomic_store(p, UNLOCKED);
}
#endif /* USE_PTHREAD */
#endif /* GMP_LOCK_H */
