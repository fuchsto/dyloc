#ifndef DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED
#define DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED

#include <dylocxx/topology.h>
#include <dylocxx/exception.h>

#include <dylocxx/internal/domain_tag.h>

#include <set>
#include <string>
#include <iterator>


namespace dyloc {

template <class Iterator, class Sentinel>
locality_domain & topology::group_domains(
       const Iterator & group_domain_tag_first,
       const Sentinel & group_domain_tag_last) {
  if (std::distance(group_domain_tag_first, group_domain_tag_last) == 0) {
    DYLOC_THROW(
        dyloc::exception::invalid_argument,
        "cannot create empty group");
  }
  // Add group domain to children of the grouped domains' lowest common
  // ancestor:
  auto & group_domain_parent       = ancestor(group_domain_tag_first,
                                              group_domain_tag_last);
  auto   group_domain_parent_vx    = _domain_vertices[
                                       group_domain_parent.domain_tag];
  auto   group_domain_parent_arity = out_degree(
                                       group_domain_parent_vx, _graph);

  DYLOC_LOG_DEBUG("dylocxx::topology.group_domains",
                  "group parent domain:", group_domain_parent,
                  "arity:", group_domain_parent_arity);

  // Find parents of specified subdomains that are an immediate child node
  // of the input domain:
  //
  int  num_group_parent_domain_tag_parts =
         std::count(
           group_domain_parent.domain_tag.begin(),
           group_domain_parent.domain_tag.end(), '.');
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
    DYLOC_LOG_TRACE("dylocxx::topology.group_domains", "group subdomains");
    return group_subdomains(
             group_domain_parent,
             group_domain_tag_first,
             group_domain_tag_last);
  } else {
    // At least one subdomain in group is not an immediate child node of
    // group parent domain:
    DYLOC_LOG_TRACE("dylocxx::topology.group_domains", "group domains");

    int group_size = std::distance(group_domain_tag_first,
                                   group_domain_tag_last);
    /* Subdomains in group are in indirect child nodes of group parent
     * domain.
     * Find immediate child nodes that are parents of group subdomains.
     * Example:
     *
     *     group domains: { .0.1.2.0, .0.1.3.1, .0.2.0 }
     *
     *     --> group ancestor subdomains:
     *           { .0.1, .0.2 }
     *         group split domain tags:
     *           {
     *             .0.1 -> { .0.1.2.0, .0.1.3.1 },
     *             .0.2 -> { .0.2.0 }
     *           }
     */
    std::unordered_map<std::string, std::vector<std::string>>
      group_subdomain_tags;
    for (int sd = 0; sd < group_size; sd++) {
      auto group_subdomain_tag_it = group_domain_tag_first;
      std::advance(group_subdomain_tag_it, sd);

      std::string group_subdomain_tag = *group_subdomain_tag_it;

      /* Position of second dot separator in tag of grouped domain
       * after the end of the parent domain tag, for example:
       *
       *   parent:          .0.1
       *   grouped domain:  .0.1.4.0
       *   dot_pos: 6 ------------'
       */
      auto sep_pos = group_subdomain_tag.find_first_of(
                       ".", group_domain_parent.domain_tag.length() + 1);
      if (sep_pos != std::string::npos) {
        group_subdomain_tag.resize(sep_pos);
      }
      DYLOC_LOG_TRACE_VAR("dylocxx::topology.group_domains",
                          group_subdomain_tag);

      group_subdomain_tags.insert(
        std::make_pair(group_subdomain_tag,
                       std::vector<std::string>()));
      group_subdomain_tags[group_subdomain_tag]
        .push_back(*group_subdomain_tag_it);
    }
    DYLOC_LOG_TRACE_VAR("dylocxx::topology.group_domains", 
                        group_subdomain_tags.size());

    locality_domain group_domain(
                      group_domain_parent,
                      DYLOC_LOCALITY_SCOPE_GROUP,
                      group_domain_parent_arity);
    group_domain.level = group_domain_parent.level;

    DYLOC_LOG_TRACE("dylocxx::topology.group_domains", 
                    "add group domain:", group_domain);

    _domains[group_domain.domain_tag] = group_domain;
    auto group_domain_parent_vertex
           = _domain_vertices[group_domain_parent.domain_tag];
    auto group_domain_vertex 
           = boost::add_vertex(
               { group_domain.domain_tag,
                 vertex_state::unspecified },
               _graph);

    _domain_vertices[group_domain.domain_tag] = group_domain_vertex;

    boost::add_edge(group_domain_parent_vertex, group_domain_vertex,
                    { edge_type::contains, 0 },
                    _graph);

    for (const auto & group_subdom: group_subdomain_tags) {
      const auto & group_subdom_tag = group_subdom.first;
      const auto & group_subdomains = group_subdom.second;
      // Partition subdomains into group:
      split_to_parent(
        group_subdom_tag,         // e.g. .0.2
        group_domain_tag_first,   // e.g. { .0.2.0.1, .0.2.1.1, .0.2.1.2 }
        group_domain_tag_last,
        group_domain.domain_tag); // e.g. .0.3
    }

    update_domain_attributes(group_domain.domain_tag);
    update_domain_capacities(group_domain_parent.domain_tag);

    return _domains[group_domain.domain_tag];
  }
}

template <class Iterator, class Sentinel>
void topology::split_to_parent(
  const std::string & src_domain_tag,
  const Iterator    & src_subdomain_tag_first,
  const Sentinel    & src_subdomain_tag_last,
  const std::string & dst_domain_tag) {
  DYLOC_LOG_DEBUG("dylocxx::topology.split_to_parent",
                  "src:", src_domain_tag,
                  "dst:", dst_domain_tag);
  auto src_domain_tag_len  = htag(src_domain_tag).length();
  int dst_subdomain_rindex = out_degree(_domain_vertices[dst_domain_tag],
                                        _graph);
  locality_domain dst_subdomain(_domains[dst_domain_tag],
                                _domains[src_domain_tag].scope,
                                dst_subdomain_rindex);
  _domains[dst_subdomain.domain_tag] = dst_subdomain;

  DYLOC_LOG_DEBUG_VAR("dylocxx::topology.split_to_parent",
                      dst_subdomain.domain_tag);
  DYLOC_LOG_DEBUG_VAR("dylocxx::topology.split_to_parent",
                      dst_subdomain.scope);

  auto dst_domain_vertex
         = _domain_vertices[dst_domain_tag];
  auto dst_subdomain_vertex 
         = boost::add_vertex(
             { dst_subdomain.domain_tag,
               vertex_state::unspecified },
             _graph);

  _domain_vertices[dst_subdomain.domain_tag] = dst_subdomain_vertex;

  boost::add_edge(dst_domain_vertex, dst_subdomain_vertex,
                  { edge_type::contains,
                    subdomain_distance(dst_domain_tag,
                                       dst_subdomain.domain_tag) },
                  _graph);
  // TODO: add alias-edge

  for (auto src_subdomain_tag_it = src_subdomain_tag_first;
       src_subdomain_tag_it != src_subdomain_tag_last;
       ++src_subdomain_tag_it) {
    if (src_subdomain_tag_it->find(src_domain_tag, 0) != 0) {
      continue;
    }
    auto src_subdomain_tag = htag(*src_subdomain_tag_it)
                             .head(src_domain_tag_len + 1);
    DYLOC_LOG_DEBUG_VAR("dylocxx::topology.split_to_parent",
                        src_subdomain_tag);
    if (src_subdomain_tag.length() < src_subdomain_tag_it->length()) {
      split_to_parent(src_subdomain_tag,
                      src_subdomain_tag_first,
                      src_subdomain_tag_last,
                      dst_subdomain.domain_tag);
    } else {
      relink_to_parent(src_subdomain_tag, dst_subdomain.domain_tag);
    }
  }
}


template <class Iterator, class Sentinel>
locality_domain & topology::group_subdomains(
       const locality_domain & domain,
       const Iterator & subdomain_tag_first,
       const Sentinel & subdomain_tag_last) {
  auto & domain_vx              = _domain_vertices[domain.domain_tag];
  auto   num_subdomains         = out_degree(domain_vx, _graph);
  size_t num_grouped_subdomains = std::distance(subdomain_tag_first,
                                                subdomain_tag_last);
  if (num_grouped_subdomains <= 0) {
    DYLOC_THROW(
        dyloc::exception::invalid_argument,
        "cannot create empty group");
  }
  DYLOC_LOG_TRACE("dylocxx::topology.group_subdomains",
                  "parent domain:", domain, "arity:", num_subdomains);

  locality_domain group_domain(
                    domain,
                    DYLOC_LOCALITY_SCOPE_GROUP,
                    num_subdomains);

  DYLOC_LOG_TRACE("dylocxx::topology.group_subdomains",
                  "add group domain:", group_domain);

  _domains[group_domain.domain_tag] = group_domain;

  auto group_domain_vx = boost::add_vertex(
                           {
                             group_domain.domain_tag,
                             vertex_state::unspecified
                           },
                           _graph);

  _domain_vertices[group_domain.domain_tag] = group_domain_vx;

  boost::add_edge(domain_vx, group_domain_vx,
                  { edge_type::contains, 0 },
                  _graph);

  // Move grouped subdomains to group domain children:
  //
  for (auto group_subdom_tag = subdomain_tag_first;
       group_subdom_tag != subdomain_tag_last;
       ++group_subdom_tag) {
    relink_to_parent(
      *group_subdom_tag,        // domain to move
      group_domain.domain_tag); // new parent domain
  }

  update_domain_attributes(group_domain.domain_tag);
  update_domain_capacities(domain.domain_tag);

  return _domains[group_domain.domain_tag];
}

} // namespace dyloc

#endif // DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED
