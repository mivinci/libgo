#include <unistd.h>

#include "sys.h"

void sys_init(void) {
  // TODO:
}

int sys_ncpu(void) {
  return sysconf(_SC_NPROCESSORS_CONF);
}

int sys_spawn(void (*f)(), int siz) {
  // TODO:
  return 0;
}
