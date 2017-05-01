
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>


namespace dyloc {
namespace test {


TEST_F(InitTest, ImmediateFinalize) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  dyloc::finalize();
}

TEST_F(InitTest, UnitLocality) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::myid().id == 0) {
    for (int u = 0; u < dyloc::num_units(); ++u) {
      const auto & uloc = dyloc::query_unit_locality(u);
      DYLOC_LOG_DEBUG("InitTest.UnitLocality", "unit locality:", uloc);
    }
  }

  dyloc::finalize();
}

TEST_F(InitTest, DomainGraph) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::myid().id == 0) {
    const auto & domain_graph = dyloc::query_locality_graph();
    boost::print_graph(domain_graph.graph());
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
