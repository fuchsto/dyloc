
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>


namespace dyloc {
namespace test {


TEST_F(InitTest, ImmediateFinalize) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
