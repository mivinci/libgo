#include "sched.h"

static G g0;
static M m0;


int Go_boot(void(*entry)(), int argc, char **argv) {
  // TODO:
  // 1) construct g0 and m0
  // 2) bind g0 and m0
  // 3) initialize scheduler
  // 4) create the first G using `entry`
  // 5) start m0

  // For now, just call `entry` to make everything we just have work
  ((void(*)(int, char **)) entry)(argc, argv);
  return 0;
}
