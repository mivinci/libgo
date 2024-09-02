#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../core/sched.h"

int main(int argc, char **argv) {
  FILE *fp;
  int n;

  assert(argc == 2);

  fp = fopen(argv[1], "w");
  assert(fp);

  n = fprintf(fp,
              "// This file is generated. DO NOT EDIT!\n"
              "#define G_m       %ld\n"
              "#define G_sched   %ld\n"
              "#define M_g0      %ld\n"
              "#define Gobuf_sp  %ld\n"
              "#define Gobuf_pc  %ld\n"
              "#define Gobuf_ctx %ld\n"
              "#define Gobuf_g   %ld\n"
              "",
              __builtin_offsetof(G, m), __builtin_offsetof(G, sched),
              __builtin_offsetof(M, g0), __builtin_offsetof(Gobuf, sp), __builtin_offsetof(Gobuf, pc), __builtin_offsetof(Gobuf, ctx),
              __builtin_offsetof(Gobuf, g));

  assert(n > 0);
  fclose(fp);
}
