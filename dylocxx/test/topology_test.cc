
#include "topology_test.h"
#include "test_globals.h"

#include <dylocxx/init.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <algorithm>


namespace dyloc {
namespace test {

TEST_F(TopologyTest, ExcludeDomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::team_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  DYLOC_LOG_DEBUG("TopologyTest.ExcludeDomains", "total domain hierarchy:");
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
    DYLOC_LOG_DEBUG("TopologyTest.ExcludeDomains",
                    "domain hierarchy after excludes:");
    topo.depth_first_search(vis);
  }

  dyloc::finalize();
}

TEST_F(TopologyTest, DomainsAncestor) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::team_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  DYLOC_LOG_DEBUG("TopologyTest.DomainsAncestor", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  int myid = dyloc::myid().id;

  std::vector<std::string> unit_domain_tags;
  unit_domain_tags.push_back(
    dyloc::query_unit_locality(dyloc::myid().id).domain_tag);

  for (int nd = 0; nd < std::min<int>(3, topo.domains().size()); ++nd) {
    std::srand((nd + 1) * (myid + 1));
    int random_unit_id = std::rand() % dyloc::num_units();
    unit_domain_tags.push_back(
      dyloc::query_unit_locality(random_unit_id).domain_tag);
  }

  const auto & ancestor = topo.ancestor(
                            unit_domain_tags.begin(),
                            unit_domain_tags.end());
  for (const auto & unit_domain_tag : unit_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("TopologyTest.DomainsAncestor", unit_domain_tag);
  }
  DYLOC_LOG_DEBUG_VAR("TopologyTest.DomainsAncestor", ancestor);

  dyloc::finalize();
}

TEST_F(TopologyTest, ScopeDomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::team_topology();
  locality_domain_dfs_output_visitor<typename topology::domain_map>
    vis(topo.domains());

  DYLOC_LOG_DEBUG("TopologyTest.ScopeDomains", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  // Tags of locality domains in CORE scope:
  auto core_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_CORE);
  for (const auto & core_domain_tag : core_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("TopologyTest.ScopeDomains", core_domain_tag);
  }

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("TopologyTest.ScopeDomains", numa_domain_tag);
  }

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

