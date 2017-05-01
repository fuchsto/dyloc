
#include <dylocxx/domain_graph.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <unordered_map>


namespace dyloc {

void domain_graph::build_hierarchy() {
  DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_hierarchy", "()");

  _root_domain.scope   = DYLOC_LOCALITY_SCOPE_GLOBAL;
  _root_domain.level   = 0;
  _root_domain.g_index = 0;
  _root_domain.r_index = 0;

  _domains.insert(std::make_pair(".", &_root_domain));

  int node_index = 0;
  for (auto & node_host_domain : _host_topology.nodes()) {
    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_hierarchy",
                    "node host:", node_host_domain.first);
    locality_domain node_domain(
        _root_domain,
        DYLOC_LOCALITY_SCOPE_NODE,
        node_index);
    node_domain.host = node_host_domain.first;

    _root_domain.children.push_back(node_domain);

    _domains.insert(
        std::make_pair(
          _root_domain.children.back().domain_tag,
          &_root_domain.children.back()));

    build_node_level_hierarchy(_root_domain.children.back());

    ++node_index;
  }
}

void domain_graph::build_node_level_hierarchy(
       locality_domain & node_domain) {
}

} // namespace dyloc
