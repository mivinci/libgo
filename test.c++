#include <gtest/gtest.h>
#include "include/go/core.h"

int Go_main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

int main(int argc, char **argv) {
  return Go_boot((Go_Func) Go_main, argc, argv);
}