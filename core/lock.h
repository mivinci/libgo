#ifndef LIBGO_CORE_LOCK_H_
#define LIBGO_CORE_LOCK_H_

#include "../include/go/core.h"

typedef struct Lock Lock;

struct Lock {
  int key;
};

#if defined(GOOS_LINUX) || defined(GOOS_APPLE)
#include <sched.h>

void Lock_init(Lock *lock) {}

void Lock_acquire(Lock *lock) {}

void Lock_release(Lock *lock) {}
#else
void Lock_init(Lock *lock) {}

void Lock_acquire(Lock *lock) {}

void Lock_release(Lock *lock) {}
#endif

#endif // LIBGO_CORE_LOCK_H_
