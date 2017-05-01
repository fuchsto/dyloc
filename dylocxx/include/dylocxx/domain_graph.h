#ifndef DYLOCXX__DOMAIN_GRAPH_H__INCLUDED
#define DYLOCXX__DOMAIN_GRAPH_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

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
  const host_topology    & _host_topology;
  const unit_mapping     & _unit_mapping;
  locality_domain        & _root_domain;

  std::unordered_map<std::string, locality_domain *> _domains;
  
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
