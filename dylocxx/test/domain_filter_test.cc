
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

  dart_barrier(DART_TEAM_ALL);

  // if (dyloc::num_units() < 2) { return; }

  auto & topo_all = dyloc::team_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis_all(topo_all.domains());

  if (dyloc::myid().id == 0) {
    topo_all.depth_first_search(vis_all);
    graphviz_out(
      topo_all.graph(),
      "DomainFilterTest.SelectLeaderDomains.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  DYLOC_LOG_DEBUG("DomainFilterTest.SelectLeaderDomains", "> clone topology");
  dyloc::topology topo_cp(topo_all);
  DYLOC_LOG_DEBUG("DomainFilterTest.SelectLeaderDomains", "< clone topology");

  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis_cp(topo_cp.domains());

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo_cp.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);

  // Select first unit in NUMA domain as leader:
  std::vector<dart_global_unit_t> leader_unit_ids;
  std::vector<std::string>        leader_unit_domain_tags;
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                        numa_domain_tag);
    const auto & numa_domain = topo_cp[numa_domain_tag];
    dart_global_unit_t leader_unit_id = numa_domain.unit_ids[0];
    leader_unit_ids.push_back(
      leader_unit_id);
    leader_unit_domain_tags.push_back(
      topo_cp[leader_unit_id].domain_tag);
  }
  for (const auto & leader_unit_domain_tag : leader_unit_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                        leader_unit_domain_tag);
  }

  auto & leader_group_domain = topo_cp.group_domains(
                                 leader_unit_domain_tags.begin(),
                                 leader_unit_domain_tags.end());
  // topo.select_domain(leader_group_domain.domain_tag);

  dart_barrier(DART_TEAM_ALL);

  if (dyloc::myid().id == 0) {
    std::cerr << "\n\n" << "Original topology after grouping:" << '\n';
    topo_all.depth_first_search(vis_all);
    std::cerr << "\n\n" << "Topology after grouping:" << '\n';
    topo_cp.depth_first_search(vis_cp);
    std::cerr << '\n' << "NUMA scope leader units locality:" << '\n';
    for (const auto & leader_unit_id : leader_unit_ids) {
      auto leader_unit_loc = topo_cp[leader_unit_id];
      DYLOC_LOG_DEBUG_VAR("DomainFilterTest.SelectLeaderDomains",
                          leader_unit_id);
      std::cerr << leader_unit_loc << std::endl;
    }
    graphviz_out(
      topo_cp.graph(), "DomainFilterTest.SelectLeaderDomains.grouped.dot");
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

