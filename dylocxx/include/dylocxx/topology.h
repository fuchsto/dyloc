#ifndef DYLOCXX__TOPOLOGY_H__INCLUDED
#define DYLOCXX__TOPOLOGY_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>
#include <dylocxx/exception.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>
#include <dylocxx/internal/algorithm.h>
#include <dylocxx/internal/domain_tag.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
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
    logical,
    adjacent,
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
    std::string  domain_tag;
    vertex_state state;
  };

  struct edge_properties {
    edge_type    type;
    int          distance;
  };

  typedef int             Metric;
  typedef locality_domain Domain;


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
            boost::vecS,           // out edge list
            boost::vecS,           // vertex list
            boost::bidirectionalS, // directed graph (parent -> sub)
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

  friend std::ostream & operator<<(
    std::ostream                  & os,
    const dyloc::topology         & topo);

  friend std::ostream & operator<<(
    std::ostream                  & os,
    dyloc::topology::vertex_state   state);

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

 public:
  typedef std::unordered_map<std::string, locality_domain> domain_map;

 private:
  const unit_mapping                             * _unit_mapping;

  /// Topological structure, represents connections between locality
  /// domains, disregarding locality domain properties.
  graph_t                                          _graph;
  /// Locality domain property data, stores accumulated domain capabilities
  /// as flat hash map, independent from topological structure.
  domain_map                                       _domains;
  /// Maps domain tag to topology graph vertex.
  std::unordered_map<std::string, graph_vertex_t>  _domain_vertices;
  /// Maps global unit id to topology graph vertex.
  std::unordered_map<int,         graph_vertex_t>  _unit_vertices;
  
 public:
  topology() = delete;

  topology(
    dart_team_t           team,
    const host_topology & host_topo,
    const unit_mapping  & unit_map)
  : _unit_mapping(&unit_map) {
    build_hierarchy(team, host_topo);
  }
 
  topology(const topology & other)
  : _unit_mapping(other._unit_mapping)
  , _domains(other._domains)
  , _domain_vertices(other._domain_vertices)
  , _unit_vertices(other._unit_vertices) {
    DYLOC_LOG_DEBUG("dylocxx::topology.topology(other)", "copy constructor");
  	typedef graph_t::vertex_descriptor vertex_t;
    typedef std::map<vertex_t, vertex_t> vertex_map_t;
  	vertex_map_t vertexMap;
    boost::associative_property_map<vertex_map_t> vertexMapWrapper(vertexMap);
    boost::copy_graph(other._graph, _graph,
                      boost::orig_to_copy(vertexMapWrapper));
  }

  topology & operator=(const topology & rhs) = delete;
  topology(topology && other)           = default;
  topology & operator=(topology && rhs) = default;

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

  locality_domain & operator[](dart_global_unit_t uid) {
    return _domains.at(_graph[_unit_vertices.at(uid.id)].domain_tag);
  }

  const locality_domain & operator[](dart_global_unit_t uid) const {
    return _domains.at(_graph[_unit_vertices.at(uid.id)].domain_tag);
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

  /**
   * Return lowest common ancestor of the domains specified by the given
   * domain tags.
   */
  template <class Iterator, class Sentinel>
  const locality_domain & ancestor(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) const {
    // Find lowest common ancestor (longest common prefix) of
    // specified domain tag list:
    std::string domain_prefix = dyloc::htag::ancestor(domain_tag_first,
                                                      domain_tag_last);
    if (domain_prefix.size() > 1 && domain_prefix.back() == '.') {
      domain_prefix.resize(domain_prefix.size() - 1);
    } else if (domain_prefix.size() == 0) {
      domain_prefix = ".";
    }
    DYLOC_LOG_TRACE_VAR("dylocxx::topology.ancestor", domain_prefix);
    return _domains.at(domain_prefix);
  }

  const locality_domain & ancestor(
         const std::initializer_list<std::string> domain_tags) const {
    return ancestor(std::begin(domain_tags), std::end(domain_tags));
  }

  /**
   * Move domain to child nodes of the specified parent domain.
   */
  void move_domain(
    const std::string & domain_tag,
    const std::string & domain_tag_new_parent);

  /**
   * Move domains with specified domain tags into separate group domain.
   * The group domain will be created as child node of the grouped domains'
   * lowest common ancestor.
   */
  template <class Iterator, class Sentinel>
  locality_domain & group_domains(
    const Iterator & group_domain_tag_first,
    const Sentinel & group_domain_tag_last);

  /**
   * Move subset of a domain's immediate child nodes into separate group
   * subdomain.
   */
  template <class Iterator, class Sentinel>
  locality_domain & group_subdomains(
    const locality_domain & domain,
    const Iterator & subdomain_tag_first,
    const Sentinel & subdomain_tag_last);

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
    _domains.erase(_domains.find(tag));
  }

  template <class Iterator, class Sentinel>
  void select_domains(
         const Iterator & domain_tag_first,
         const Sentinel & domain_tag_last) {
    for (auto it = domain_tag_first; it != domain_tag_last; ++it) {
      _graph[_domain_vertices[*it]].state = vertex_state::selected;
    }
    remove_domains([&](const std::string & tag) {
                         return _graph[_domain_vertices[tag]].state !=
                                  vertex_state::selected;
                       });
  }

  void select_domain(
         const std::string & domain_tag) {
    _graph[_domain_vertices[domain_tag]].state = vertex_state::selected;
    for_each_ancestor(
      domain_tag,
      [&](const locality_domain & ancestor_dom) {
            _graph[_domain_vertices[ancestor_dom.domain_tag]].state
               = vertex_state::selected;
         });
    for_each_descendant(
      domain_tag,
      [&](const locality_domain & desc_dom) {
            _graph[_domain_vertices[desc_dom.domain_tag]].state
               = vertex_state::selected;
         });
    remove_domains(
      ".",
      [&](const locality_domain & dom) {
           return _graph[_domain_vertices[dom.domain_tag]].state !=
                    vertex_state::selected;
         });
#if TMP_DEBUGGING
    update_domain_attributes(".");
#else
    for_each_descendant(
      domain_tag,
      [&](const locality_domain & desc_dom) {
        update_domain_attributes(desc_dom.domain_tag);
      });
#endif
    update_domain_capacities(".");
  }

  template <class UnaryPredicate>
  void remove_domains(const std::string & tag, UnaryPredicate pred) {
    auto & domain = _domains[tag];
    if (!pred(domain)) {
      for_each_descendant_inv(
        tag,
        [&](const locality_domain & dom) {
          remove_domains(dom.domain_tag, pred);
        });
    } else {
      DYLOC_LOG_TRACE("dylocxx::topology.remove_domain",
                      "remove:", tag);
      _graph[_domain_vertices[tag]].state = vertex_state::hidden;
      boost::clear_vertex(_domain_vertices[tag], _graph);
      // boost::remove_vertex(_domain_vertices[tag], _graph);
      _domains.erase(_domains.find(tag));
      // _domain_vertices.erase(_domain_vertices.find(tag));
    }
  }

  template <class UnaryPredicate>
  void for_each_ancestor(const std::string & tag, UnaryPredicate func) {
    auto & domain_vx = _domain_vertices[tag];
    for (auto domain_edges = in_edges(domain_vx, _graph);
         domain_edges.first != domain_edges.second;
         ++domain_edges.first) {
      auto   parent_vx = source(*domain_edges.first, _graph);
      auto & parent    = _domains[_graph[parent_vx].domain_tag];
      func(parent);
      for_each_ancestor(parent.domain_tag, func);
    }
  }

  template <class UnaryPredicate>
  void for_each_descendant(const std::string & tag, UnaryPredicate func) {
    auto & domain_vx = _domain_vertices[tag];
    for (auto domain_edges = out_edges(domain_vx, _graph);
         domain_edges.first != domain_edges.second;
         ++domain_edges.first) {
      auto   child_vx = target(*domain_edges.first, _graph);
      auto & child    = _domains[_graph[child_vx].domain_tag];
      func(child);
      for_each_descendant(child.domain_tag, func);
    }
  }

  template <class UnaryPredicate>
  void for_each_descendant_inv(const std::string & tag, UnaryPredicate func) {
    auto & domain_vx = _domain_vertices[tag];
    for (auto domain_edges = out_edges(domain_vx, _graph);
         domain_edges.first != domain_edges.second;
         ++domain_edges.first) {
      auto   child_vx = target(*domain_edges.first, _graph);
      auto & child    = _domains[_graph[child_vx].domain_tag];
      for_each_descendant_inv(child.domain_tag, func);
      func(child);
    }
  }

  /**
   * Resolve tags of all domains at specified scope.
   */
  std::vector<std::string> scope_domain_tags(
         dyloc_locality_scope_t scope) const;


  void add_distance_metric(
        std::string metric_name,
        std::function<Metric(const Domain && source,
                             const Domain && target)> fn)
  {
    _distance_metrics[metric_name]=fn;
  }
  
  std::string list_distance_metrics(){
    std::string DMs="";
    for (auto m : _distance_metrics){
      DMs +=m.first;
    }    
    DYLOC_LOG_DEBUG("Dyloc::Topology.list_distance_metrics ", DMs);
    return DMs;
  }

 private:
  std::map<std::string,
           std::function<Metric(const Domain && source,
                                const Domain && target)>
          > _distance_metrics;

  void build_hierarchy(
          dart_team_t           team,
          const host_topology & host_topo);

  void build_node_level(
          const host_topology & host_topo,
          locality_domain     & node_domain,
          graph_vertex_t      & node_domain_vertex);

  void build_module_level(
          const host_topology & host_topo,
          locality_domain     & module_domain,
          graph_vertex_t      & node_domain_vertex,
          int                   module_scope_level);

  void relink_to_parent(
          const std::string & domain_tag,
          const std::string & domain_tag_new_parent);

  template <class Iterator, class Sentinel>
  void split_to_parent(
          const std::string & domain_tag,
          const Iterator    & subdomain_tag_first,
          const Sentinel    & subdomain_tag_last,
          const std::string & domain_tag_new_parent);

  void rename_domain(
          const std::string & tag,
          const std::string & new_tag);

  void update_domain_capacities(const std::string & tag);
  void update_domain_attributes(const std::string & tag);

  int  subdomain_distance(
          const std::string & parent_tag,
          const std::string & child_tag);
};

} // namespace dyloc

#include <dylocxx/impl/topology.impl.h>

#endif // DYLOCXX__TOPOLOGY_H__INCLUDED
