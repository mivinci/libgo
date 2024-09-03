#include <gtest/gtest.h>

#include "sys.h"

TEST(core__sys_xxx_test, sys_ncpu) {
  const int n = sys_ncpu();
  EXPECT_GT(n, 0);
}
