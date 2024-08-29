#include <unistd.h>

#include "sys.h"

void setup_sys(void) {
  // TODO:
}

int sys_ncpu(void) {
  return sysconf(_SC_NPROCESSORS_CONF);
}

int sys_spawn(void (*f)(), int siz) {
  // TODO:
  return 0;
}

int sys_tid(void) {
  return -1;
}
