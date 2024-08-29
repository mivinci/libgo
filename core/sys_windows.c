#include <windows.h>

#include "sys.h"

void sys_init(void) {
  // TODO:
}

int sys_ncpu(void) {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return si.dwNumberOfProcessors;
}

int sys_spawn(void(*f)(), int siz) {
  // TODO:
  return 0;
}