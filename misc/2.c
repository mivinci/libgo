#include <stdlib.h>

extern void go(void *) asm("go");

static int a;

void goexit(void) {
  a = 200;
  exit(0);
}

void f() { a = 100; }

int main(void) {
  uintptr_t sp = (uintptr_t)malloc(1024);
  sp -= sizeof(void *);
  sp = sp & (~7);
  *(void **)sp = goexit;
  go(f);
}
