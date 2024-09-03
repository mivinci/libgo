#include <gtest/gtest.h>
#include "../include/go/core.h"

void foo(int a) {}
void bar(int a, int b) {}

TEST(core__sched_test, spawn) {
  go(foo, 100);       // Like `go foo(100)` in Go
  Go(foo, 100);       // Go also works
  Go(bar, 100, 200);  // Support for un-fixed number of arguments
}
