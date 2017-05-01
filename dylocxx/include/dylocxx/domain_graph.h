#ifndef DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
#define DYLOCXX__DOMAIN_GRAPH_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/connected_components.hpp>

#include <unordered_map>


namespace dyloc {


/**
 * Extension to the hwloc topology data structure.
 *
 * \todo Consider specializing hwloc topology type via sub-classing.
 */
class domain_graph {
  typedef domain_graph self_t;

 private:
  enum class edge_type : int {
    unspecified  = 0,
    contains     = 100,
    sibling
  };

  struct vertex_property_t {
    locality_domain * domain;
  };

  struct edge_property_t {
    edge_type type;
    int       distance;
  };

  /*
   * Using boost graph with domain data as external properties, see:
   *   http://programmingexamples.net/wiki/CPP/Boost/BGL/GridGraphProperties
   */
  typedef boost::adjacency_list<
            boost::listS,          // out-edges stored in a std::list
            boost::vecS,           // vertex set stored here
            boost::directedS,      // directed graph (parent -> sub)
            vertex_property_t,     // vertex properties
            edge_property_t,       // edge properties
            boost::no_property,    // graph properties
            boost::listS           // edge storage
          >
    graph_t;

  typedef boost::property_map<
            graph_t,
            boost::vertex_index_t
          >::const_type
    index_map_t;

 private:
  const host_topology    & _host_topology;
  const unit_mapping     & _unit_mapping;
  locality_domain        & _root_domain;

  std::unordered_map<std::string, locality_domain *> _domains;

  graph_t _graph;
  
 public:
  domain_graph(
    const host_topology & host_topo,
    const unit_mapping  & unit_map,
    locality_domain     & team_global_dom)
  : _host_topology(host_topo)
  , _unit_mapping(unit_map)
  , _root_domain(team_global_dom) {
    build_hierarchy();
  }

 private:
  void build_hierarchy();
  void build_node_level_hierarchy(
         locality_domain & node_domain);
  void build_module_level_hierarchy(
         locality_domain & module_domain);
};


} // namespace dyloc

#endif // DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
