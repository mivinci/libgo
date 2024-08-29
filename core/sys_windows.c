#include <windows.h>

#include "sys.h"

void setup_sys(void) {
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