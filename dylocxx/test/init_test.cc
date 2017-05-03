
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
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

  if (dyloc::myid().id == 0) {
    const auto & topo = dyloc::query_topology();
    locality_domain_dfs_output_visitor vis;

    // const auto & graph = topo.graph();
    // boost::print_graph(
    //   graph,
    //   boost::get(
    //     &topology::vertex_properties::domain,
    //     graph));
    // boost::depth_first_search(graph, visitor(vis));
    topo.depth_first_search(vis);

    for (int u = 0; u < dyloc::num_units(); ++u) {
      const auto & uloc = dyloc::query_unit_locality(u);
      DYLOC_LOG_DEBUG("InitTest.UnitLocality", uloc);
    }
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
