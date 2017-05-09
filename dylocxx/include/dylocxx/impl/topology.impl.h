#ifndef DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED
#define DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED

#include <dylocxx/topology.h>

#include <iterator>
#include <string>


namespace dyloc {

template <class Iterator, class Sentinel>
void topology::group_domains(
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
    DYLOC_LOG_DEBUG("dylocxx::topology.group_domains", "group subdomains");
    group_subdomains(
      group_domain_parent,
      group_domain_tag_first,
      group_domain_tag_last);
  } else {
    // At least one subdomain in group is not an immediate child node of
    // group parent domain:
    DYLOC_LOG_DEBUG("dylocxx::topology.group_domains", "group domains");

    int group_size = std::distance(group_domain_tag_first,
                                   group_domain_tag_last);
    std::vector<std::string> immediate_subdomain_tags;
    immediate_subdomain_tags.reserve(group_size);
    for (int sd = 0; sd < group_size; sd++) {
      auto group_subdomain_tag = group_domain_tag_first;
      std::advance(group_subdomain_tag, sd);

      DYLOC_LOG_DEBUG_VAR("dylocxx::topology.group_domains",
                          *group_subdomain_tag);

      immediate_subdomain_tags.push_back(*group_subdomain_tag);

      /* Position of second dot separator in tag of grouped domain
       * after the end of the parent domain tag, for example:
       *
       *   parent:          .0.1
       *   grouped domain:  .0.1.4.0
       *   dot_pos: 6 ------------'
       */
      auto sep_pos = group_subdomain_tag->find_first_of(
                       ".", group_domain_parent.domain_tag.length() + 1);
      if (sep_pos != std::string::npos) {
        DYLOC_LOG_DEBUG_VAR("dylocxx::topology.group_domains", sep_pos);
        immediate_subdomain_tags.back().resize(sep_pos);
      }
    }
    auto num_group_subdomains  = dyloc::count_unique(
                                   immediate_subdomain_tags.begin(),
                                   immediate_subdomain_tags.end());
    DYLOC_LOG_DEBUG_VAR("dylocxx::topology.group_domains", 
                        num_group_subdomains);

    locality_domain group_domain(
                      group_domain_parent,
                      DYLOC_LOCALITY_SCOPE_GROUP,
                      group_domain_parent_arity);

    DYLOC_LOG_DEBUG_VAR("dylocxx::topology.group_domains", 
                        group_domain.domain_tag);

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
                    { edge_type::contains, group_domain.level },
                    _graph);

    for (int gsd = 0; gsd < num_group_subdomains; gsd++) {
      // Query instance of the group domain's immediate subdomain:
      auto & group_subdomain_in = _domains[immediate_subdomain_tags[gsd]];
      // Use relink_to_parent() to partition subdomains into group
      relink_to_parent(
        group_subdomain_in.domain_tag,
        group_domain.domain_tag);
    }

    update_domain_attributes(group_domain.domain_tag);
  }

  update_domain_capacities();
}


template <class Iterator, class Sentinel>
void topology::group_subdomains(
       const locality_domain & domain,
       const Iterator & subdomain_tag_first,
       const Sentinel & subdomain_tag_last) {
  auto & domain_vx              = _domain_vertices[domain.domain_tag];
  auto   num_subdomains         = out_degree(domain_vx, _graph);
  size_t num_grouped_subdomains = std::distance(subdomain_tag_first,
                                                subdomain_tag_last);
  if (num_grouped_subdomains <= 0) {
    return;
  }
  DYLOC_LOG_DEBUG("dylocxx::topology.group_subdomains",
                  "parent domain:", domain, "arity:", num_subdomains);

  locality_domain group_domain(
                    domain,
                    DYLOC_LOCALITY_SCOPE_GROUP,
                    num_subdomains);

  DYLOC_LOG_DEBUG("dylocxx::topology.group_subdomains",
                  "add group domain", group_domain);

  _domains[group_domain.domain_tag] = group_domain;

  auto group_domain_vx = boost::add_vertex(
                           {
                             group_domain.domain_tag,
                             vertex_state::unspecified
                           },
                           _graph);

  _domain_vertices[group_domain.domain_tag] = group_domain_vx;

  boost::add_edge(domain_vx, group_domain_vx,
                  { edge_type::contains, group_domain.level },
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
}

} // namespace dyloc

#endif // DYLOC__IMPL__TOPOLOGY_IMPL_H__INCLUDED
