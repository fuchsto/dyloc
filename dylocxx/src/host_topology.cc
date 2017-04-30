
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
  DYLOC_LOG_TRACE("dylocxx::host_topology.()",
                  "copying host names from", num_units, "units");
  for (size_t luid = 0; luid < num_units; ++luid) {
    dart_global_unit_t guid;
    DYLOC_ASSERT_RETURNS(
      dart_team_unit_l2g(team, luid, &guid),
      DART_OK);
    // Add global unit id to list of units of its host:
    std::string unit_hostname(unit_map[guid.id].hwinfo.host);
    DYLOC_LOG_TRACE("dylocxx::host_topology.()",
                    "team unit id:",   luid,
                    "global unit id:", guid.id,
                    "host:",           unit_hostname);

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
    host_dom.level           = 0;
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
        dart_team_unit_g2l(team, host_unit_gid, &luid),
        DART_OK);
      const auto & ul   = unit_map[luid];
      int unit_numa_id  = ul.hwinfo.numa_id;
      int unit_num_numa = ul.hwinfo.num_numa;

      if (unit_num_numa == 0) {
        unit_numa_id = 0;
      }

      DYLOC_LOG_TRACE("dylocxx::host_topology.()",
                      "mapping unit", luid.id,
                      "to host",      host_name,
                      "NUMA id:",     unit_numa_id);
      if (unit_numa_id >= 0) {
        host_numa_ids.insert(unit_numa_id);
      }
    }
    host_dom.numa_ids.resize(host_numa_ids.size());
    std::copy(host_numa_ids.begin(),
              host_numa_ids.end(),
              host_dom.numa_ids.begin());
  }

  this->_host_topo.num_host_levels = 0;
  this->_host_topo.num_nodes       = num_hosts;
  this->_host_topo.num_hosts       = num_hosts;
  this->_host_topo.num_units       = num_units;

  collect_topology(unit_map);

  DYLOC_LOG_DEBUG("dylocxx::host_topology.()", ">");
}

void host_topology::collect_topology(
  const unit_mapping & unit_map) {
  dart_team_t team = unit_map.team;

  /*
   * Initiate all-to-all exchange of module locations like Xeon Phi
   * hostnames and their assiocated NUMA domain in their parent node.
   *
   * Select one leader unit per node for communication:
   */
  dart_team_unit_t       local_leader_unit_lid = DART_UNDEFINED_TEAM_UNIT_ID;
  dart_team_unit_t       my_id;
  dart_group_t           leader_group;
  dart_group_t           local_group; // group of all units on local host
  dart_team_t            leader_team; // team of all node leaders

  DYLOC_ASSERT_RETURNS(
    dart_team_myid(team, &my_id),
    DART_OK);

  DYLOC_ASSERT_RETURNS(
    dart_group_create(&leader_group),
    DART_OK);
  DYLOC_ASSERT_RETURNS(
    dart_group_create(&local_group),
    DART_OK);

  /*
   * unit ID of leader unit (relative to the team specified in unit_mapping)
   * of the active unit's local compute node.
   * Example:
   *
   *  node 0:                     node 1:
   *    unit ids: { 0, 1, 2 }       unit ids: { 3, 4, 5 }
   *
   * leader unit at units 0,1,2: 0
   * leader unit at units 3,4,5: 3
   */

  const auto & my_uloc        = unit_map[my_id];
  const auto & local_hostname = my_uloc.hwinfo.host;

  DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                  "local host:", local_hostname);

  for (const auto & host_units_mapping : _host_units) {
    const auto  & host_name       = host_units_mapping.first;
    const auto  & host_unit_gids  = host_units_mapping.second;
    host_domain & host_dom        = _host_domains[host_name];

    dart_global_unit_t leader_unit_gid = host_unit_gids[0];
    DYLOC_ASSERT_RETURNS(
      dart_group_addmember(leader_group, leader_unit_gid),
      DART_OK);

    if (host_name == local_hostname) {
      /* set local leader: */
      DYLOC_ASSERT_RETURNS(
        dart_team_unit_g2l(team, leader_unit_gid,
                           &local_leader_unit_lid),
        DART_OK);
      /* collect units in local group: */
      for (dart_global_unit_t host_unit_gid : host_unit_gids) {
        DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                        "add unit", host_unit_gid.id,
                        "to local group");
        DYLOC_ASSERT_RETURNS(
          dart_group_addmember(local_group, host_unit_gid),
          DART_OK);
      }
    }
  }
  size_t num_leaders;
  DYLOC_ASSERT_RETURNS(
    dart_group_size(leader_group, &num_leaders),
    DART_OK);

  if (num_leaders > 1) {
    DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                    "create leader team");
    DYLOC_ASSERT_RETURNS(
      dart_team_create(team, leader_group, &leader_team),
      DART_OK);
    DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                    "leader team:", leader_team);
  } else {
    leader_team = team;
  }
  DYLOC_ASSERT_RETURNS(
    dart_group_destroy(&leader_group),
    DART_OK);

  int max_node_modules = 2;

  if (my_id.id == local_leader_unit_lid.id) {
    // Translate ID of active unit in parent team to unit ID
    // in leader team:
    dart_team_unit_t my_leader_id;
    DYLOC_ASSERT_RETURNS(
      dart_team_myid(leader_team, &my_leader_id),
      DART_OK);

    std::vector<dyloc_module_location_t> local_modules;
    local_topology(unit_map, local_modules);

    // Number of bytes to receive from each leader unit in allgatherv:
    std::vector<size_t> recvcounts(num_leaders);
    // Displacement at which to place data received from each leader:
    std::vector<size_t> displs(num_leaders);
    // Number of bytes to be sent by this leader unit:
    recvcounts[my_leader_id.id] = local_modules.size() *
                                    sizeof(dyloc_module_location_t);

    // Collect all local module locations into global module locations
    // from leaders.
    // Only required if there is more than one leader unit.
    std::vector<dyloc_module_location_t> module_locations;
    if (num_leaders > 1) {
      // All module locations to receive:
      module_locations.resize(max_node_modules * num_leaders);
      // Exchange number of bytes to be sent by every leader:
      DYLOC_ASSERT_RETURNS(
        dart_allgather(
          NULL,
          recvcounts.data(),
          1,
          DART_TYPE_SIZET,
          leader_team),
        DART_OK);

      displs[0] = 0;
      for (size_t lu = 1; lu < num_leaders; lu++) {
        DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                       "allgather:",
                       "leader unit", lu,
                       "sent:", recvcounts[lu]);
        displs[lu] = displs[lu-1] + recvcounts[lu];
      }
      // Finally, exchange local module locations between all leaders:
      DYLOC_ASSERT_RETURNS(
        dart_allgatherv(
          local_modules.data(),
          recvcounts[my_leader_id.id],
          DART_TYPE_BYTE,
          module_locations.data(),
          recvcounts.data(),
          displs.data(),
          leader_team),
        DART_OK);
    } else {
      module_locations = local_modules;
    }

    // Wait for exchange of module locations between all leaders:
    if (num_leaders > 1) {
      dart_barrier(leader_team);
    }


    if (num_leaders > 1) {
      DYLOC_LOG_TRACE("dylocxx::host_topology.update_module_locations",
                      "finalize leader team");
      DYLOC_ASSERT_RETURNS(
        dart_team_destroy(&leader_team),
        DART_OK);
    }
  }
  dart_barrier(team);
  /*
   * Broadcast updated host topology data from leader to all units at
   * local node:
   */
}

void host_topology::local_topology(
  const unit_mapping                   & unit_map,
  std::vector<dyloc_module_location_t> & module_locations) {
  dyloc__unused(unit_map);
  dyloc__unused(module_locations);
}

} // namespace dyloc
