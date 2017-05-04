
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

#include <algorithm>


namespace dyloc {
namespace test {

TEST_F(DomainGroupTest, GroupNUMADomains) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);

  auto & topo = dyloc::query_topology();
  locality_domain_dfs_output_visitor vis;

  DYLOC_LOG_DEBUG("TopologyTest.ScopeDomains", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  // Tags of locality domains in NUMA scope:
  auto numa_domain_tags = topo.scope_domain_tags(
                            DYLOC_LOCALITY_SCOPE_NUMA);
  for (const auto & numa_domain_tag : numa_domain_tags) {
    DYLOC_LOG_DEBUG_VAR("TopologyTest.ScopeDomains", numa_domain_tag);
  }

  topo.group_locality_domains(
    numa_domain_tags.begin(),
    numa_domain_tags.end());

  DYLOC_LOG_DEBUG("TopologyTest.ScopeDomains", "total domain hierarchy:");
  if (dyloc::myid().id == 0) {
    topo.depth_first_search(vis);
  }
  dart_barrier(DART_TEAM_ALL);

  dyloc::finalize();
}

} // namespace dyloc
} // namespace test

