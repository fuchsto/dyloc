
#include <dylocxx/topology.h>

#include <dylocxx/internal/logging.h>

#include <dyloc/common/types.h>

#include <boost/graph/graph_traits.hpp>

#include <vector>
#include <string>
#include <functional>
#include <algorithm>


namespace dyloc {

std::vector<std::string>
topology::scope_domain_tags(
  dyloc_locality_scope_t scope) const {
  std::vector<std::string> scope_dom_tags;
  std::vector<graph_vertex_t> vx_matches;
  const auto vx_range = vertices(_graph);
  std::for_each(
    vx_range.first, vx_range.second,
    [&](const graph_vertex_t & vx) {
      if (_domains.at(_graph[vx].domain_tag).scope
           == scope) {
        vx_matches.push_back(vx);
      }
    });

  DYLOC_LOG_DEBUG("dylocxx::topology.scope_domain_tags",
                  "num. domains matched:", vx_matches.size());
  std::for_each(
    vx_matches.begin(),
    vx_matches.end(),
    [&](const graph_vertex_t & vx) {
          scope_dom_tags.push_back(_graph[vx].domain_tag);
        });
  return scope_dom_tags;
}

} // namespace dyloc

