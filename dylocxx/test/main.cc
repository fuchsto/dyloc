
#include "test_base.h"
#include "test_globals.h"

#include <gtest/gtest.h>


using ::testing::UnitTest;
using ::testing::TestEventListeners;


int main(int argc, char * argv[])
{
  TESTENV.argc = argc;
  TESTENV.argv = argv;

  // Init GoogleTest (strips gtest arguments from argv)
  ::testing::InitGoogleTest(&argc, argv);

  // Run Tests
  int ret = RUN_ALL_TESTS();

  return ret;
}

