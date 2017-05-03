#ifndef DYLOCXX__TEST__TEST_BASE_H__INCLUDED
#define DYLOCXX__TEST__TEST_BASE_H__INCLUDED

#include <gtest/gtest.h>

#include <dylocxx/unit_locality.h>
#include <dylocxx/locality_domain.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/utility.h>
#include <dylocxx/adapter/dart.h>
#include <dylocxx/internal/logging.h>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <string>
#include <iostream>
#include <iomanip>


namespace dyloc {
namespace test {

class locality_domain_dfs_output_visitor
  : public boost::default_dfs_visitor {
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
