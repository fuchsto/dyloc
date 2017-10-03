#ifndef DYLOCXX__TEST__TEST_BASE_H__INCLUDED
#define DYLOCXX__TEST__TEST_BASE_H__INCLUDED

#include <gtest/gtest.h>

#include <dylocxx/topology.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/locality_domain.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>
// #include <boost/graph/graphviz.hpp>

#include <string>
#include <iostream>
#include <iomanip>


namespace dyloc {
namespace test {

template <class DomainMap>
class locality_domain_dfs_output_visitor
: public boost::default_dfs_visitor {
  const DomainMap & _domains;
public:
  locality_domain_dfs_output_visitor(
    const DomainMap & domain_map)
  : _domains(domain_map) { }

  template <typename Vertex, typename Graph>
  void discover_vertex(Vertex u, const Graph & g) const {
    const std::string            & ldom_tag = g[u].domain_tag;
    const auto &                   ldom_it  = _domains.find(ldom_tag);
    if (ldom_it == _domains.end()) {
      DYLOC_LOG_ERROR("locality_domain_dfs_output_visitor.discover_vertex",
                      "domain not found:", ldom_tag);
    }
    dyloc::topology::vertex_state  ldom_st  = g[u].state;
    const dyloc::locality_domain & ldom     = ldom_it->second;
    std::cout << std::left  << std::setw(7)  << u              << " "
              << std::left  << std::setw(3)  << ldom_st        << " "
              << std::left  << std::setw(7)  << ldom.scope     << " "
              << std::left  << std::setw(4)  << ldom.level     << " [g:"
              << std::right << std::setw(2)  << ldom.g_index   << "]"
              << std::left  << std::setw(20) << ldom_tag       << " | cores:"
              << std::right << std::setw(3)  << ldom.num_cores << " "
                                             << "units:"
                                             << dyloc::make_range(
                                                 ldom.unit_ids.begin(),
                                                 ldom.unit_ids.end())
                                             << std::endl;
  }
};

template <class Graph>
void graphviz_out(const Graph & graph, const std::string & filename) {
#if 0
  std::ofstream of(filename);
  write_graphviz(of, graph,
                 boost::make_label_writer(
                   boost::get(
                     &topology::vertex_properties::domain_tag,
                     graph)),
                 boost::make_label_writer(
                   boost::get(
                     &topology::edge_properties::distance,
                     graph)));
#endif
}


class TestBase : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

} // namespace test
} // namespace dyloc

#endif // DYLOCXX__TEST__TEST_BASE_H__INCLUDED
