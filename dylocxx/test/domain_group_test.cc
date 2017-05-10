
#include "domain_group_test.h"
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

TEST_F(DomainGroupTest, GroupSubdomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  DYLOC_LOG_DEBUG("DomainGroupTest.GroupSubdomains",
                  "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupSubdomains.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainGroupTest.GroupSubdomains", numa_domain_tag);
  }

  topo.group_domains(
    numa_domain_tags.begin(),
    numa_domain_tags.end());

  dart_barrier(DART_TEAM_ALL);

  DYLOC_LOG_DEBUG("DomainGroupTest.GroupSubdomains",
                  "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupSubdomains.grouped.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

TEST_F(DomainGroupTest, GroupAsymmetric) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::num_units() < 2) { return; }

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupAsymmetric.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);

  int myid = dyloc::myid().id;

  std::vector<std::string> unit_domain_tags;
  for (int nd = 0; nd < std::min<int>(3, topo.domains().size()); ++nd) {
    std::srand((nd + 1) * (myid + 1));
    int random_unit_id = std::rand() % dyloc::num_units();
    unit_domain_tags.push_back(
      dyloc::query_unit_locality(random_unit_id).domain_tag);
  }
  std::sort(unit_domain_tags.begin(),
            unit_domain_tags.end());
  unit_domain_tags.resize(
      std::distance(unit_domain_tags.begin(),
                    std::unique(unit_domain_tags.begin(),
                                unit_domain_tags.end())));
  for (const auto & unit_domain_tag : unit_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainGroupTest.GroupAsymmetric", unit_domain_tag);
  }

  topo.group_domains(
    unit_domain_tags.begin(),
    unit_domain_tags.end());

  dart_barrier(DART_TEAM_ALL);

  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupAsymmetric.grouped.dot");
  }

  dyloc::finalize();
}

TEST_F(DomainGroupTest, GroupNUMALeaders) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  if (dyloc::num_units() < 2) { return; }

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupNUMALeaders.original.dot");
  }
  dart_barrier(DART_TEAM_ALL);


  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);

  // Select first unit in NUMA domain as leader:
  std::vector<std::string> leader_unit_domain_tags;
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainGroupTest.GroupNUMALeaders", numa_domain_tag);
    const auto & numa_domain = topo[numa_domain_tag];
    dart_global_unit_t leader_unit_id = numa_domain.unit_ids[0];
    leader_unit_domain_tags.push_back(
      dyloc::query_unit_locality(leader_unit_id).domain_tag);
  }
  for (const auto & leader_unit_domain_tag : leader_unit_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("DomainGroupTest.GroupNUMALeaders",
                        leader_unit_domain_tag);
  }

  topo.group_domains(
    leader_unit_domain_tags.begin(),
    leader_unit_domain_tags.end());

  dart_barrier(DART_TEAM_ALL);

  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
    graphviz_out(
      topo.graph(), "DomainGroupTest.GroupNUMALeaders.grouped.dot");
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

