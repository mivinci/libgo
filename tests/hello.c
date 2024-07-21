#include <stdio.h>

#include "gmp/gmp.h"

void f(void *arg) {
  int i;

  for (i = 0; i < 5; i++) {
    printf("%s\n", (char *)arg);
  }
}

int main(void) {
  GMP_init(0);
  GMP_go(f, "f1");
  GMP_go(f, "f2");
  GMP_go(f, "f3");
  return 0;
}
