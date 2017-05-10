
#include "domain_filter_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/topology.h>
#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graphviz.hpp>

#include <algorithm>


namespace dyloc {
namespace test {


TEST_F(DomainFilterTest, SelectLeaderDomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::num_units() < 2) { return; }

  auto & topo_all = dyloc::team_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo_all.domains());

  if (dyloc::myid().id == 0) {
    topo_all.depth_first_search(vis);
    graphviz_out(
      topo_all.graph(),
      "DomainFilterTest.SelectLeaderDomains.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  DYLOC_LOG_DEBUG("DomainFilterTest.SelectLeaderDomains", "> clone topology");
  dyloc::topology topo(topo_all);
  DYLOC_LOG_DEBUG("DomainFilterTest.SelectLeaderDomains", "< clone topology");

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);

  // Select first unit in NUMA domain as leader:
  std::vector<dart_global_unit_t> leader_unit_ids;
  std::vector<std::string>        leader_unit_domain_tags;
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                        numa_domain_tag);
    const auto & numa_domain = topo[numa_domain_tag];
    dart_global_unit_t leader_unit_id = numa_domain.unit_ids[0];
    leader_unit_ids.push_back(
      leader_unit_id);
    leader_unit_domain_tags.push_back(
      dyloc::query_unit_locality(leader_unit_id).domain_tag);
  }
  for (const auto & leader_unit_domain_tag : leader_unit_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                        leader_unit_domain_tag);
  }

  auto & leader_group_domain = topo.group_domains(
                                 leader_unit_domain_tags.begin(),
                                 leader_unit_domain_tags.end());
  // topo.select_domain(leader_group_domain.domain_tag);

  dart_barrier(DART_TEAM_ALL);

  if (dyloc::myid().id == 0) {
    std::cerr << '\n' << "Topology after grouping:" << '\n';
    topo_all.depth_first_search(vis);
    std::cerr << '\n' << "Topology after grouping:" << '\n';
    topo.depth_first_search(vis);
    std::cerr << '\n' << "NUMA scope leader units locality:" << '\n';
    for (const auto & leader_unit_id : leader_unit_ids) {
      auto leader_unit_loc = dyloc::query_unit_locality(leader_unit_id);
      DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                          leader_unit_id);
      std::cerr << leader_unit_loc << std::endl;
    }
    graphviz_out(
      topo.graph(), "DomainFilterTest.SelectLeaderDomains.grouped.dot");
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

