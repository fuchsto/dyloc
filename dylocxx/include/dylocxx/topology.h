#ifndef DYLOCXX__TOPOLOGY_H__INCLUDED
#define DYLOCXX__TOPOLOGY_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>
#include <dylocxx/internal/algorithm.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_traits.hpp>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <string>


namespace dyloc {


/**
 * Extension to the hwloc topology data structure.
 */
class topology {
  typedef topology self_t;

 public:
  enum class edge_type : int {
    unspecified  = 0,
    contains     = 100,
    sibling,
    alias
  };

  enum class vertex_state : int {
    unspecified  = 0,
    hidden       = 100,
    moved,
    selected
  };

  struct vertex_properties {
    std::string       domain_tag;
    // TODO: pointer will be invalidated when domain graph is copied.
    locality_domain * domain;
    vertex_state      state;
  };

  struct edge_properties {
    edge_type type;
    int       distance;
  };

  /*
   * Using boost graph with domain data as external properties, see:
   *
   *   http://programmingexamples.net/wiki/CPP/Boost/BGL/GridGraphProperties
   *
   * For boost::adjacency_list and the implications of its underlying
   * container types, see:
   *
   *   http://www.boost.org/doc/libs/1_58_0/libs/graph/doc/adjacency_list.html
   *
   *   "In general, if you want your vertex and edge descriptors to be stable
   *    (never invalidated) then use listS or setS for the VertexList and
   *    OutEdgeList template parameters of adjacency_list. If you are not as
   *    concerned about descriptor and iterator stability, and are more
   *    concerned about memory consumption and graph traversal speed, use
   *    vecS for the VertexList and/or OutEdgeList template parameters."
   *
   */
  typedef boost::adjacency_list<
            boost::listS,          // out-edges stored in a std::list
            boost::vecS,           // vertex set stored here
            boost::directedS,      // directed graph (parent -> sub)
            vertex_properties,     // vertex properties
            edge_properties,       // edge properties
            boost::no_property,    // graph properties
            boost::listS           // edge storage
          >
    graph_t;

  typedef boost::property_map<
            graph_t,
            boost::vertex_index_t
          >::const_type
    index_map_t;

  typedef boost::graph_traits<graph_t>::vertex_descriptor
    graph_vertex_t;

  typedef boost::graph_traits<graph_t>::edge_descriptor
    graph_edge_t;

 private:
  template <class Visitor>
  class selective_dfs_visitor : public boost::default_dfs_visitor {
    Visitor _v;
   public:
    selective_dfs_visitor(Visitor & v) : _v(v) { }

    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph & g) const {
      if (g[u].state != vertex_state::hidden) {
        _v.discover_vertex(u,g);
      }
    }
  };

  /* Example usage:
   *
   *   vertex_matching_dfs_visitor<graph_t> vx_match_vis(
   *     // match predicate:
   *        [&](const graph_vertex_t & u, const graph_t & g) {
   *          const auto & vx_domain_tag = g[u].domain_tag;
   *          auto         vx_scope      = _domains.at(vx_domain_tag).scope;
   *          return (vx_scope == scope);
   *        },
   *     // function to call for every match:
   *        [](const graph_vertex_t & u, const graph_t & g) {
   *          std::cout << "matched: " << g[u].domain_tag << '\n';
   *        });
   *
   *   boost::depth_first_search(_graph, visitor(vx_match_vis));
   *
   */
  template <class Graph = graph_t>
  class vertex_matching_dfs_visitor
    : public boost::default_dfs_visitor {
    typedef typename boost::graph_traits<Graph>::vertex_descriptor
      vertex_t;
    typedef std::function<bool(const vertex_t &, const Graph & g)>
      match_pred_t;
    typedef std::function<void(const vertex_t &, const Graph & g)>
      match_callback_t;

    match_pred_t     _match_pred;
    match_callback_t _match_callback;
   public:
    vertex_matching_dfs_visitor(
      const match_pred_t     & match_pred,
      const match_callback_t & match_callback)
    : _match_pred(match_pred)
    , _match_callback(match_callback)
    { }

    void discover_vertex(const vertex_t & u, const Graph & g) {
      if (_match_pred(u, g)) { _match_callback(u, g); }
    }
  };

 private:
  host_topology    & _host_topology;
  unit_mapping     & _unit_mapping;
  locality_domain  & _root_domain;

  // Topological structure, represents connections between locality
  // domains, disregarding locality domain properties.
  graph_t _graph;

  // Locality domain property data, stores accumulated domain capabilities
  // as flat hash map, independent from topological structure.
  std::unordered_map<std::string, locality_domain> _domains;
  std::unordered_map<std::string, graph_vertex_t>  _domain_vertices;
  
 public:
  topology() = delete;

  topology(
    host_topology   & host_topo,
    unit_mapping    & unit_map,
    locality_domain & team_global_dom)
  : _host_topology(host_topo)
  , _unit_mapping(unit_map)
  , _root_domain(team_global_dom) {
    build_hierarchy();
  }

#if 0
  // TODO: copying disabled until pointer invalidation in
  //       struct vertex_properties is fixed.
 
  topology(const topology & other)
  : _host_topology(other._host_topology)
  , _unit_mapping(other._unit_mapping)
  , _root_domain(other._root_domain)
  , _domains(other._domains) {
    boost::copy_graph(other._graph, _graph);
  }

  topology & operator=(const topology & rhs) {
    _host_topology = rhs._host_topology;
    _unit_mapping  = rhs._unit_mapping;
    _root_domain   = rhs._root_domain;
    _domains       = rhs._domains;
    boost::copy_graph(rhs._graph, _graph);
    return *this;
  }
#endif

  inline const graph_t & graph() const noexcept {
    return _graph;
  }

  const std::unordered_map<std::string, locality_domain> & domains() const {
    return _domains;
  }

  locality_domain & operator[](const std::string & tag) {
    return _domains[tag];
  }

  const locality_domain & operator[](const std::string & tag) const {
    return _domains.at(tag);
  }

  template <class Visitor>
  void depth_first_search(Visitor & vis) {
    selective_dfs_visitor<Visitor> sel_vis(vis);
    boost::depth_first_search(_graph, visitor(sel_vis));
  }

  template <class Visitor>
  void depth_first_search(Visitor & vis) const {
    selective_dfs_visitor<Visitor> sel_vis(vis);
    boost::depth_first_search(_graph, visitor(sel_vis));
  }

  template <class FilterPredicate>
  void filter(
         const FilterPredicate & filter) {
    dyloc__unused(filter);
    // filtered_graph<graph_t, filter> fg(_graph, filter);
  }

  template <class Iterator, class Sentinel>
  const locality_domain & ancestor(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) const {
    // Find lowest common ancestor (longest common prefix) of
    // specified domain tag list:
    std::string domain_prefix = dyloc::longest_common_prefix(
                                  domain_tag_first,
                                  domain_tag_last);
    if (domain_prefix.back() == '.') {
      domain_prefix.pop_back();
    }
    return _domains.at(domain_prefix);
  }

  /**
   * Move domains with specified domain tags into separate group domain.
   * The group domain will be created as child node of the grouped domains'
   * lowest common ancestor.
   */
  template <class Iterator, class Sentinel>
  void group_domains(
         const Iterator & group_domain_tag_first,
         const Sentinel & group_domain_tag_last) {
    auto group_domains_ancestor_tag = dyloc::longest_common_prefix(
                                        group_domain_tag_first,
                                        group_domain_tag_last);
    if (group_domains_ancestor_tag.back() == '.') {
      group_domains_ancestor_tag.pop_back();
    }
    // Add group domain to children of the grouped domains' lowest common
    // ancestor:
    auto & group_domain_parent       = _domains.at(
                                         group_domains_ancestor_tag);
    auto   group_domain_parent_vx    = _domain_vertices[
                                         group_domain_parent.domain_tag];
    auto   group_domain_parent_arity = boost::out_degree(
                                         group_domain_parent_vx, _graph);

    // Find parents of specified subdomains that are an immediate child node
    // of the input domain:
    //
    int  num_group_parent_domain_tag_parts =
             std::count(
               group_domains_ancestor_tag.begin(),
               group_domains_ancestor_tag.end(), '.');
    // Test if group domains ancestor is immediate parent of all grouped
    // domains:
    auto indirect_domain_tag_it =
             std::find_if(
               group_domain_tag_first,
               group_domain_tag_last,
               [&](const std::string & tag) {
                    return ( std::count(tag.begin(), tag.end(), '.') !=
                             num_group_parent_domain_tag_parts + 1);
                   });

    if (indirect_domain_tag_it == group_domain_tag_last) {
      // Subdomains in group are immediate child nodes of group parent
      // domain:
      group_subdomains(
        group_domain_parent,
        group_domain_tag_first,
        group_domain_tag_last);
    } else {
      // At least one subdomain in group is immediate child nodes of group
      // parent domain:
      locality_domain group_domain(
                        group_domain_parent,
                        DYLOC_LOCALITY_SCOPE_GROUP,
                        0); // TODO: num_grouped_subdomains);
    }
  }

  void move_domain(
      const std::string & domain_tag,
      const std::string & domain_tag_old_parent,
      const std::string & domain_tag_new_parent) {
    // Remove edge to current parent:
    boost::remove_edge(
      _domain_vertices[domain_tag_old_parent],
      _domain_vertices[domain_tag],
      _graph);
    // Add edge to new parent:
    boost::add_edge(
      _domain_vertices[domain_tag_new_parent],
      _domain_vertices[domain_tag],
      { edge_type::contains, 1 },
      _graph);
  }

  /**
   * Move subset of a domain's immediate child nodes into separate group
   * subdomain.
   */
  template <class Iterator, class Sentinel>
  void group_subdomains(
         const locality_domain & domain,
         const Iterator & subdomain_tag_first,
         const Sentinel & subdomain_tag_last) {
    auto & domain_vx              = _domain_vertices[domain.domain_tag];
    auto   num_subdomains         = boost::out_degree(domain_vx, _graph);
    size_t num_grouped_subdomains = std::distance(subdomain_tag_first,
                                                  subdomain_tag_last);
    if (num_grouped_subdomains <= 0) {
      return;
    }

    locality_domain group_domain(
                      domain,
                      DYLOC_LOCALITY_SCOPE_GROUP,
                      num_subdomains);

    _domains[group_domain.domain_tag] = group_domain;

    auto group_domain_vx = boost::add_vertex(
                             {
                               group_domain.domain_tag,
                               &_domains[group_domain.domain_tag],
                               vertex_state::unspecified
                             },
                             _graph);

    // Move grouped subdomains to group domain children:
    //
    for (auto group_subdom_tag = subdomain_tag_first;
         group_subdom_tag != subdomain_tag_last;
         ++group_subdom_tag) {
      move_domain(
        *group_subdom_tag,        // domain to move
        domain.domain_tag,        // old parent domain
        group_domain.domain_tag); // new parent domain
    }
  }

  template <class Iterator, class Sentinel>
  void exclude_domains(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) {
    for (auto it = domain_tag_first; it != domain_tag_last; ++it) {
      exclude_domain(*it);
    }
  }

  void exclude_domain(const std::string & tag) {
    _graph[_domain_vertices[tag]].state = vertex_state::hidden;
    auto sub_v_range = boost::adjacent_vertices(
                         _domain_vertices[tag],
                         _graph);
    for (auto sv = sub_v_range.first; sv != sub_v_range.second; ++sv) {
      _graph[*sv].state = vertex_state::hidden;
      exclude_domain(_graph[*sv].domain_tag);
    }
  }

  template <class Iterator, class Sentinel>
  void select_domains(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) {
    for (auto it = domain_tag_first; it != domain_tag_last; ++it) {
      _graph[_domain_vertices[*it]].state = vertex_state::selected;
    }
  }

  std::vector<std::string> scope_domain_tags(
         dyloc_locality_scope_t scope) const;

 private:
  void build_hierarchy();

  void build_node_level(
         locality_domain & node_domain,
         graph_vertex_t  & node_domain_vertex);

  void build_module_level(
         locality_domain & module_domain,
         graph_vertex_t  & node_domain_vertex,
         int               module_scope_level);

};

} // namespace dyloc

#endif // DYLOCXX__TOPOLOGY_H__INCLUDED
