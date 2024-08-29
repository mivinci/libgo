#include <gtest/gtest.h>

#include "sys.h"

TEST(sys_darwin, positive) {
  int n = sys_ncpu();
  EXPECT_GT(n, 0);
}
