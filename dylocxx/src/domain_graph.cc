
#include <dylocxx/domain_graph.h>
#include <dylocxx/utility.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/vector_as_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <unordered_map>
#include <algorithm>
#include <vector>


namespace dyloc {

/* Outline of the domain graph construction procedure:
 *
 * 1. Build graph as domain containment hierarchy
 * 2. Use DFS visitor to collect domain capabilities
 *
 */

void domain_graph::build_hierarchy() {
  DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_hierarchy", "()");

  _root_domain.scope   = DYLOC_LOCALITY_SCOPE_GLOBAL;
  _root_domain.level   = 0;
  _root_domain.g_index = 0;
  _root_domain.r_index = 0;

  _domains.insert(std::make_pair(".", &_root_domain));

  auto root_domain_vertex
         = boost::add_vertex(
             { &_root_domain },
             _graph);

  int node_index = 0;
  for (auto & node_host_domain : _host_topology.nodes()) {
    const auto & node_hostname = node_host_domain.first;
    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_hierarchy",
                    "node host:", node_hostname);

    locality_domain node_domain(
        _root_domain,
        DYLOC_LOCALITY_SCOPE_NODE,
        node_index);
    node_domain.unit_ids = _host_topology.unit_ids(node_hostname);
    node_domain.host     = node_hostname;

    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_node_level",
                    "add domain:", node_domain);

    _root_domain.children.push_back(node_domain);

    _domains.insert(
        std::make_pair(
          _root_domain.children.back().domain_tag,
          &_root_domain.children.back()));

    auto node_domain_vertex
           = boost::add_vertex(
               { &_root_domain.children.back() },
               _graph);

    boost::add_edge(root_domain_vertex, node_domain_vertex,
                    { edge_type::contains, 1 },
                    _graph);

    build_node_level(
      _root_domain.children.back(),
      node_domain_vertex);

    ++node_index;
  }
}

void domain_graph::build_node_level(
  locality_domain & node_domain,
  graph_vertex_t  & node_domain_vertex) {
  DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_node_level",
                  "node:", node_domain.host);
  // units located at node:
  node_domain.unit_ids = _host_topology.unit_ids(node_domain.host);
  // modules located at node:
  auto & node_modules  = _host_topology.node_modules(node_domain.host);

  build_module_level(
    node_domain,
    node_domain_vertex,
    0);

  int module_index = 0;
  for (auto & node_module : node_modules) {
    const auto & module_hostname = node_module.get().host;
    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_node_level",
                    "module host name:", module_hostname);
    locality_domain module_domain(
        node_domain,
        DYLOC_LOCALITY_SCOPE_MODULE,
        module_index);
    module_domain.unit_ids = _host_topology.unit_ids(module_hostname);
    module_domain.host     = module_hostname;

    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_node_level",
                    "add domain:", module_domain);

    node_domain.children.push_back(module_domain);

    _domains.insert(
        std::make_pair(
          node_domain.children.back().domain_tag,
          &node_domain.children.back()));

    auto module_domain_vertex 
           = boost::add_vertex(
               { &node_domain.children.back() },
               _graph);

    boost::add_edge(node_domain_vertex, module_domain_vertex,
                    { edge_type::contains, 1 },
                    _graph);

    build_module_level(
      node_domain.children.back(),
      module_domain_vertex,
      0);

    ++module_index;
  }
}

void domain_graph::build_module_level(
  locality_domain & module_domain,
  graph_vertex_t  & module_domain_vertex,
  int               module_scope_level) {
  DYLOC_LOG_TRACE(
    "dylocxx::domain_graph.build_module_level",
    "module domain:", module_domain);
  // units located at node:
  module_domain.unit_ids = _host_topology.unit_ids(module_domain.host);
  /*
   * NOTE: Locality scopes may be heterogeneous but are expected
   *       to be homogeneous within a module domain.
   *       For example, this would be a valid use case:
   *
   *       module[0] { unit[0]: [CORE,CACHE,PACKAGE,NUMA],
   *                   unit[1]: [CORE,CACHE,PACKAGE,NUMA] }
   *       module[1] { unit[2]: [CORE,CACHE,CACHE,CACHE,NUMA],
   *                   unit[2]: [CORE,CACHE,CACHE,CACHE,NUMA] }
   */

  /* Collect scope lists of all units at the given module, converting:
   *
   *    u[0].scopes: [CORE:0, CACHE:4, CACHE:0, NUMA:0]
   *    u[1].scopes: [CORE:1, CACHE:5, CACHE:0, NUMA:0]
   *    u[2].scopes: [CORE:2, CACHE:6, CACHE:1, NUMA:0]
   *    u[3].scopes: [CORE:3, CACHE:7, CACHE:1, NUMA:0]
   *
   * to transposed structure:
   *
   *    level[0]: { scope:NUMA,  gids:    [       0       ],
   *                             sub_gids:[[  0   ,   1  ]]   }
   *
   *    level[1]: { scope:CACHE, gids:    [   0,      1   ],
   *                             sub_gids:[[4 , 5],[6 , 7]]   }
   *
   *    level[2]: { scope:CACHE, gids:    [ 4,  5,  6,  7 ],
   *                             sub_gids:[[0],[1],[2],[3]]   }
   *
   * such that subdomains of a domain with global index G are referenced
   * in sub_gids[G].
   */

  dart_team_unit_t module_leader_unit_id;
  DYLOC_ASSERT_RETURNS(
    dart_team_unit_g2l(module_domain.team, module_domain.unit_ids[0],
                       &module_leader_unit_id),
    DART_OK);

  // const auto & module_host_topology   = _host_topology.modules().at(
  //                                         module_domain.host).get();
  // const auto & module_numa_ids        = module_host_topology.numa_ids;

  const auto & module_leader_unit_loc = _unit_mapping[module_leader_unit_id];
  const auto & module_leader_hwinfo   = module_leader_unit_loc.hwinfo;

  int num_scopes = module_leader_hwinfo.num_scopes;

  std::vector<dyloc_locality_scope_t> module_scopes;
  module_scopes.reserve(num_scopes);
  for (int s = 0; s < num_scopes; s++) {
    module_scopes.push_back(
      module_leader_hwinfo.scopes[s].scope);
  }

  int subdomain_gid_idx = num_scopes - (module_scope_level + 1);

  DYLOC_LOG_TRACE(
    "dylocxx::domain_graph.build_module_level", "--",
    "current scope:", module_scopes[subdomain_gid_idx],
    "level", subdomain_gid_idx,
    "in module scopes:",
    dyloc::make_range(
      module_scopes.begin(),
      module_scopes.end()));
  DYLOC_LOG_TRACE(
    "dylocxx::domain_graph.build_module_level", "--",
    "module units:",
    dyloc::make_range(
      module_domain.unit_ids.begin(),
      module_domain.unit_ids.end()));

  /* Array of the global indices of the current module subdomains.
   * Maximum number of global indices, including duplicates, is number
   * of units:
   */
  std::vector<int> module_subdomain_gids;
  module_subdomain_gids.reserve(module_domain.unit_ids.size());

  for (auto module_unit_gid : module_domain.unit_ids) {
    dart_team_unit_t module_unit_lid
      = dyloc::g2l(module_domain.team, module_unit_gid);

    const auto & module_unit_loc    = _unit_mapping[module_unit_lid];
    const auto & module_unit_hwinfo = module_unit_loc.hwinfo;

    int unit_level_gid = module_unit_hwinfo.scopes[subdomain_gid_idx+1].index;
    int unit_sub_gid   = -1;
    if (subdomain_gid_idx >= 0) {
      unit_sub_gid = module_unit_hwinfo.scopes[subdomain_gid_idx].index;
    }
    // Ignore units that are not contained in current module domain:
    if (module_scope_level == 0 ||
        unit_level_gid == module_domain.g_index) {
      module_subdomain_gids.push_back(unit_sub_gid);
    }
  }
  std::sort(module_subdomain_gids.begin(),
            module_subdomain_gids.end());
  auto module_subdomain_gids_end = std::unique(
                                     module_subdomain_gids.begin(),
                                     module_subdomain_gids.end());
  auto num_subdomains            = std::distance(
                                     module_subdomain_gids.begin(),
                                     module_subdomain_gids_end);
  DYLOC_LOG_TRACE(
    "dylocxx::domain_graph.build_module_level", "--",
    "module subdomain gids:",
    dyloc::make_range(
      module_subdomain_gids.begin(),
      module_subdomain_gids_end));
  
  for (int sd = 0; sd < num_subdomains; ++sd) {
    locality_domain module_subdomain(
        module_domain,
        module_scopes[subdomain_gid_idx],
        sd);
    module_subdomain.host    = module_domain.host;
    module_subdomain.g_index = module_subdomain_gids[sd];

    for (auto module_unit_gid : module_domain.unit_ids) {
      dart_team_unit_t module_unit_lid
        = dyloc::g2l(module_domain.team, module_unit_gid);

      const auto & module_unit_loc    = _unit_mapping[module_unit_lid];
      const auto & module_unit_hwinfo = module_unit_loc.hwinfo;
      if (module_unit_hwinfo.scopes[subdomain_gid_idx].index ==
          module_subdomain.g_index) {
        module_subdomain.unit_ids.push_back(module_unit_gid);
      }
    }

    DYLOC_LOG_DEBUG("dylocxx::domain_graph.build_node_level", "--",
                    "add domain:", module_subdomain);

    module_domain.children.push_back(module_subdomain);

    _domains.insert(
        std::make_pair(
          module_domain.children.back().domain_tag,
          &module_domain.children.back()));

    auto module_subdomain_vertex 
           = boost::add_vertex(
               { &module_domain.children.back() },
               _graph);

    boost::add_edge(module_domain_vertex, module_subdomain_vertex,
                    { edge_type::contains, 1 },
                    _graph);

    if (subdomain_gid_idx <= 0) {
      // At CORE scope:
    } else {
      // Recurse down:
      build_module_level(
        module_domain.children.back(),
        module_subdomain_vertex,
        module_scope_level + 1);
    }
  }
}

} // namespace dyloc
