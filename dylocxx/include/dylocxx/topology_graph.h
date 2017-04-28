#ifndef DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
#define DYLOCXX__DOMAIN_GRAPH_H__INCLUDED


#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/connected_components.h>
#include <boost/graph/edge_list.h>
#include <boost/graph/adjacency_list.h>
#include <boost/graph/vector_as_graph.h>
#include <boost/graph/topological_sort.h>


namespace dyloc {

/**
 * Extension to the hwloc topology data structure.
 *
 * \todo Consider specializing hwloc topology type via sub-classing.
 */
class domain_graph {
  typedef domain_graph self_t;

 private:
  
};


} // namespace dyloc

#endif // DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
