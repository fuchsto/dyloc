
#include <dylocxx/unit_mapping.h>
#include <dylocxx/host_topology.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dyloc/common/host_topology.h>

#include <dash/dart/if/dart_types.h>

#include <vector>
#include <string>
#include <set>
#include <algorithm>


namespace dyloc {

host_topology::host_topology(const unit_mapping & unit_map)
: _unit_map(unit_map) {
  DYLOC_LOG_DEBUG("dylocxx::host_topology.()", "()");

  dart_team_t team = unit_map.team;
  DYLOC_LOG_TRACE("dylocxx::host_topology.()", "team:", team);

  size_t num_units;
  DYLOC_ASSERT_RETURNS(dart_team_size(team, &num_units), DART_OK);
  DYLOC_ASSERT_MSG(num_units == unit_mapping.num_units,
                   "Number of units in mapping differs from team size");

  // Map unit ids to their host name:
  //
  DYLOC_LOG_TRACE("dylocxx::host_topology.()", "copying host names");
  for (size_t luid = 0; luid < num_units; ++luid) {
    dart_global_unit_t guid;
    DYLOC_ASSERT_RETURNS(
      dart_team_unit_l2g(team, luid, &guid),
      DART_OK);
    // Add global unit id to list of units of its host:
    std::string unit_hostname(_unit_map[luid].hwinfo.host);
    _host_units.insert(
        std::make_pair(unit_hostname,
                       std::vector<dart_global_unit_t>()));
    _host_units[unit_hostname].push_back(guid);
  }

  int num_hosts = _host_units.size();

  // Iterate hosts:
  //
  for (const auto & host_units_mapping : _host_units) {
    const auto  & host_name       = host_units_mapping.first;
    const auto  & host_unit_gids  = host_units_mapping.second;
    host_domain & host_dom        = _host_domains[host_name];

    // host_dom.num_units = host_unit_gids.size();

    // host domain data:
    // host_dom.host[0]   = '\0';
    // host_dom.parent[0] = '\0';
    // host_dom.num_numa  = 0;
    host_dom.level     = 0;
    host_dom.scope_pos.scope = DYLOC_LOCALITY_SCOPE_NODE;
    host_dom.scope_pos.index = 0;

    // write host name to host domain data:
    host_dom.host = host_name;

    DYLOC_LOG_TRACE("dylocxx::host_topology.()",
                    "mapping units to", host_name);

    // NUMA ids occupied by units on the host:
    std::set<int> host_numa_ids;
    for (dart_global_unit_t host_unit_gid : host_unit_gids) {
      dart_team_unit_t luid;
      DYLOC_ASSERT_RETURNS(
        dart_team_unit_l2g(team, luid, &host_unit_gid),
        DART_OK);
      const auto & ul  = _unit_map[luid];
      int unit_numa_id = ul.hwinfo.numa_id;

      DYLOC_LOG_TRACE("dylocxx::host_topology.()",
                      "mapping unit", host_unit_gid.id,
                      "to host",      host_name,
                      "NUMA id:",     unit_numa_id);
      if (unit_numa_id >= 0) {
        host_numa_ids.insert(unit_numa_id);
      }
    }
    // host_dom.num_numa = host_numa_ids.size();
    std::copy(host_numa_ids.begin(),
              host_numa_ids.end(),
              host_dom.numa_ids.begin());
  }

  this->_host_topo.num_host_levels = 0;
  this->_host_topo.num_nodes       = num_hosts;
  this->_host_topo.num_hosts       = num_hosts;
  this->_host_topo.num_units       = num_units;

  update_module_locations(unit_map);
}

void host_topology::update_module_locations(
  const unit_mapping & unit_map) {
}

} // namespace dyloc
