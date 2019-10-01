

#include <algorithm>
#include <memory>

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


namespace dyloc {
namespace test {


//Jakub
TEST_F(TopologyTest, DistanceMetric) {
  dyloc::init(&TESTENV.argc, &TESTENV.argv);
  auto & topo = dyloc::team_topology();
  DYLOC_LOG_DEBUG("TopologyTest.DistanceMetric", "Total Distance:");
//TODO
  
  auto dist_fn= [](int a){ return a+1; };
  std::string str="TestMetric";
  topo.add_distance_metric(str, dist_fn);
  ASSERT_EQ(0, str.compare(topo.list_distance_metrics()));
  ASSERT_EQ(43, dist_fn(42));
  dyloc::finalize();
}




} // namespace dyloc
} // namespace test

