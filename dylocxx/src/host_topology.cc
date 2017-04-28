
#include <dylocxx/unit_locality.h>
#include <dylocxx/host_topology.h>

#include <dyloc/common/host_topology.h>

#include <vector>
#include <string>


namespace dyloc {

host_topology::host_topology(const team_unit_localities & unit_map)
: _unit_map(unit_map) {

  dart_team_t team = unit_mapping.team;
  size_t num_units;

  DYLOC_LOG_TRACE("dylocxx::host_topology(): team:" << team);

  DYLOC_ASSERT_RETURNS(dart_team_size(team, &num_units), DART_OK);
  DYLOC_ASSERT_MSG(num_units == unit_mapping.num_units,
                  "Number of units in mapping differs from team size");

  // Map unit ids to their host name:
  //
  DYLOC_LOG_TRACE("dylocxx::host_topology: copying host names");
  for (size_t luid = 0; luid < num_units; ++luid) {
    dart_global_unit_t guid;
    DYLOC_ASSERT_RETURNS(
      dart_team_unit_l2g(team, luid, &guid),
      DART_OK);
    // Add global unit id to list of units of its host:
    std::string unit_hostname(_unit_map[luid].hwinfo().host);
    _host_units.insert(
        std::make_pair(unit_hostname,
                       std::vector());
    _host_units[unit_hostname].push_back(guid);
  }

  int num_hosts = _host_units.size();

  // Iterate hosts:
  //
  for (const auto & host_units_mapping : _host_units) {
    const auto         & host_name   = host_units_mapping.first;
    const auto         & host_units  = host_units_mapping.second;
    dart_host_domain_t & host_domain = _host_domains[host_name];

    host_domain.num_units = host_units.size();

    // host domain data:
    host_domain->host[0]   = '\0';
    host_domain->parent[0] = '\0';
    host_domain->num_numa  = 0;
    host_domain->level     = 0;
    host_domain->scope_pos.scope = DYLOC_LOCALITY_SCOPE_NODE;
    host_domain->scope_pos.index = 0;

    // write host name to host domain data:
    host_name.copy(host_domain->host, host_name.size());

    DYLOC_LOG_TRACE("dylocxx::host_topology(): mapping units to " <<
                    host_name);

    // NUMA ids occupied by units on the host:
    std::set<int> host_numa_ids;
    for (dart_global_unit_t host_unit_id : host_units) {
      dart_team_unit_t luid = { host_unit_id };
      const auto &     ul   = _unit_map[luid];

      int unit_numa_id      = ul->hwinfo.numa_id;

      DYLOC_LOG_TRACE("dylocxx::host_topology(): "
                     "mapping unit " << host_unit_id << " " <<
                     "to host '"     << _host_name << "', " <<
                     "NUMA id: "     << unit_numa_id);
      if (unit_numa_id >= 0) {
        host_numa_ids.insert(unit_numa_id);
      }
    }
    host_domain.num_numa = host_numa_ids.size();
    host_domain.numa_ids = new int[host_numa_ids.size()];
  }

  this->_host_topo.num_host_levels = 0;
  this->_host_topo.num_nodes       = num_hosts;
  this->_host_topo.num_hosts       = num_hosts;
  this->_host_topo.num_units       = num_units;

  update_module_locations(
    unit_mapping, topo);
}

host_topology::~host_topology() {
}

} // namespace dyloc
