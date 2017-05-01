#ifndef DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
#define DYLOCXX__DOMAIN_GRAPH_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/topological_sort.hpp>


namespace dyloc {

/**
 * Extension to the hwloc topology data structure.
 *
 * \todo Consider specializing hwloc topology type via sub-classing.
 */
class domain_graph {
  typedef domain_graph self_t;

 private:
  const host_topology    & _host_topo;
  const unit_mapping     & _unit_map;
  const locality_domain  & _loc_dom;
  
 public:
  domain_graph(
    const host_topology    & host_topo,
    const unit_mapping     & unit_map,
    const locality_domain  & team_global_dom)
  : _host_topo(host_topo)
  , _unit_map(unit_map)
  , _loc_dom(team_global_dom)
  { }

};


} // namespace dyloc

#endif // DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
