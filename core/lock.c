#include "go/core.h"
#include "sys.h"

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
