#ifndef LIBGO_CORE_ATOMIC_H_
#define LIBGO_CORE_ATOMIC_H_

int Atomic_load(int *p) __asm__("atomicload");
void Atomic_store(int *p, int v) __asm__("atomicstore");
extern bool Atomic_cas(int *p, int, int) __asm__("atomiccas");

#endif  // LIBGO_CORE_ATOMIC_H_
