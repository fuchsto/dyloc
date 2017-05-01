
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>


namespace dyloc {
namespace test {


TEST_F(InitTest, ImmediateFinalize) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  dyloc::finalize();
}

TEST_F(InitTest, UnitLocality) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  const auto & uloc = dyloc::query_unit_locality(
                        dyloc::dart_myid());

  DYLOC_LOG_DEBUG("InitTest.UnitLocality", "unit locality:", uloc);

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
