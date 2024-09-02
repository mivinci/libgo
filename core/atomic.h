#ifndef LIBGO_CORE_ATOMIC_H_
#define LIBGO_CORE_ATOMIC_H_

#if __STDC_VERSION__ >= 201112L
#include <stdatomic.h>
#define Atomic_cas(p, old, new)                                            \
  atomic_compare_exchange_weak_explicit(p, old, new, memory_order_release, \
                                        memory_order_relaxed)
#define Atomic_load(p) atomic_load_explicit(p, memory_order_acquire)
#define Atomic_store(p, v) atomic_store_explicit(p, v, memory_order_release)
#else
#define Atomic_cas(p, old, new) \
  __atomic_compare_exchange_n(p, old, new, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)
#define Atomic_load(p) __atomic_load_n(p, __ATOMIC_ACQUIRE)
#define Atomic_store(p, v) __atomic_store_n(p, v, __ATOMIC_RELEASE)
#endif

#endif  // LIBGO_CORE_ATOMIC_H_
