#include <stdlib.h>

extern void go(void *, uintptr_t) asm("go");

static int a;

void goexit(void) {
  exit(0);
}

void f() {}

int main(void) {
  uintptr_t top, sp;
  
  top = (uintptr_t)malloc(1024);
  sp = top + 1024;
  sp -= sizeof(void *);
  sp = sp & (~15);
  *(void **)sp = goexit;
  go(f, sp);
}
