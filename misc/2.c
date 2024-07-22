#include <stdlib.h>

extern void go(void *, uintptr_t) asm("go");

static int a;

void goexit(void) {
  exit(0);
}

void f() {}

int main(void) {
  uintptr_t sp = (uintptr_t)malloc(1024);
  sp -= sizeof(void *);
  sp = sp & (~7);
  *(void **)sp = goexit;
  go(f, sp);
}
