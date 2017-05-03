
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

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor vis;

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
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

TEST_F(TopologyTest, DomainsAncestor) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor vis;

  DYLOC_LOG_DEBUG("TopologyTest.ExcludeDomains", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  int myid = dyloc::myid().id;

  std::srand(myid);
  int random_unit_id = (myid + std::rand()) % dyloc::num_units();

  std::vector<std::string> unit_domain_tags;
  unit_domain_tags.push_back(
    dyloc::query_unit_locality(dyloc::myid().id).domain_tag);

  for (int nd = 0; nd < std::min<int>(3, topo.domains().size()); ++nd) {
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

} // namespace dyloc
} // namespace test

