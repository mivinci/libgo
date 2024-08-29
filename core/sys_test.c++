#include <gtest/gtest.h>

#include "sys.h"

TEST(sys, sys_ncpu) {
  const int n = sys_ncpu();
  EXPECT_GT(n, 0);
}
