#include <gtest/gtest.h>

#include "sys.h"

TEST(sys, sys_ncpu) {
  int n = sys_ncpu();
  EXPECT_GT(n, 0);
}
