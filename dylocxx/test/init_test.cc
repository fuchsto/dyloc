
#include "init_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <iostream>
#include <iomanip>


namespace dyloc {
namespace test {

class custom_dfs_visitor : public boost::default_dfs_visitor {
public:
  template <typename Vertex, typename Graph>
  void discover_vertex(Vertex u, const Graph & g) const {
    dyloc::locality_domain * ldom     = g[u].domain;
    const std::string &      ldom_tag = g[u].domain_tag;
    std::cout << std::left << std::setw(7)  << ldom->scope << " "
              << std::left << std::setw(20) << ldom_tag << " | "
              << "units:"
              << dyloc::make_range(
                  ldom->unit_ids.begin(),
                  ldom->unit_ids.end())
              << std::endl;
  }
};


TEST_F(InitTest, ImmediateFinalize) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  dyloc::finalize();
}

TEST_F(InitTest, UnitLocality) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::myid().id == 0) {
    const auto & topo = dyloc::query_topology();
    custom_dfs_visitor vis;

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

TEST_F(InitTest, ExcludeDomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  custom_dfs_visitor vis;

  DYLOC_LOG_DEBUG("InitTest.ExcludeDomains", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  std::vector<std::string> excluded_domain_tags;
  excluded_domain_tags.push_back(".0.0");

  topo.exclude_domains(
    excluded_domain_tags.begin(),
    excluded_domain_tags.end());

  if (dyloc::myid().id == 0) {
    DYLOC_LOG_DEBUG("InitTest.ExcludeDomains",
                    "domain hierarchy after excludes:");
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test
