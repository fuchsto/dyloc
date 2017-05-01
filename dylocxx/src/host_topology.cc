
#include <dylocxx/unit_mapping.h>
#include <dylocxx/host_topology.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_team_group.h>

#include <vector>
#include <string>
#include <set>
#include <algorithm>

#ifdef DART_ENABLE_HWLOC
#  include <hwloc.h>
#  include <hwloc/helper.h>
#  include <dyloc/common/internal/hwloc.h>
#endif

namespace dyloc {

host_topology::host_topology(const unit_mapping & unit_map) {
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
    dyloc_host_domain_t host_dom;

    // host_dom.num_units = host_unit_gids.size();

    // host domain data:
    host_dom.level           = 0;
    host_dom.scope_pos.scope = DYLOC_LOCALITY_SCOPE_NODE;
    host_dom.scope_pos.index = 0;

    // write host name to host domain data:
    host_name.copy(host_dom.host, host_name.size());

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
    host_dom.num_numa = host_numa_ids.size();
    std::copy(host_numa_ids.begin(),
              host_numa_ids.end(),
              host_dom.numa_ids);

    _host_domains.push_back(std::move(host_dom));
  }

  _num_host_levels = 0;
  _num_nodes       = num_hosts;
  _num_hosts       = num_hosts;

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

  int num_hosts = _host_units.size();

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

  DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
                  "local host:", local_hostname);

  for (const auto & host_dom : _host_domains) {
    std::string host_name(host_dom.host);
    const auto  & host_unit_gids  = _host_units[host_name];

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
        DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
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
    DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
                    "create leader team");
    DYLOC_ASSERT_RETURNS(
      dart_team_create(team, leader_group, &leader_team),
      DART_OK);
    DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
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
        DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
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

    for (size_t lu = 0; lu < num_leaders; lu++) {
      /* Number of modules received from leader unit lu: */
      size_t lu_num_modules = recvcounts[lu] /
                              sizeof(dyloc_module_location_t);
      for (size_t m = 0; m < lu_num_modules; m++) {
        int m_displ = displs[lu] / sizeof(dyloc_module_location_t);
        const dyloc_module_location_t & module_loc =
                module_locations[m_displ + m];
        for (auto & host_dom : _host_domains) {
          if (host_dom.host == module_loc.module) {
            /* Classify host as module: */
            std::strcpy(host_dom.parent, module_loc.host);
            host_dom.scope_pos = module_loc.pos;
            host_dom.level     = 1;
            if (_num_host_levels < host_dom.level) {
              _num_host_levels = host_dom.level;
            }
            break;
          }
        }
      }
    }
    // Wait for exchange of module locations between all leaders and
    // finalize leader team:
    if (num_leaders > 1) {
      DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
                      "finalize leader team");
      dart_barrier(leader_team);
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
  if (DART_UNDEFINED_UNIT_ID != local_leader_unit_lid.id) {
    dart_team_t      local_team; 
    dart_team_unit_t host_topo_bcast_root = local_leader_unit_lid;
    dart_team_t      host_topo_bcast_team = team;
    if (num_hosts > 1) {
      DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
                      "create local team");
      DYLOC_ASSERT_RETURNS(
        dart_team_create(team, local_group, &local_team),
        DART_OK);
      /* Leader unit ID local team is always 0: */
      host_topo_bcast_team    = local_team;
      host_topo_bcast_root.id = 0;
    }

    DYLOC_ASSERT_RETURNS(
      dart_bcast(
        _host_domains.data(),
        sizeof(dyloc_host_domain_t) * _host_domains.size(),
        DART_TYPE_BYTE,
        host_topo_bcast_root,
        host_topo_bcast_team),
      DART_OK);

    if (num_hosts > 1) {
      DYLOC_LOG_TRACE("dylocxx::host_topology.collect_topology",
                      "finalize local team");
      DYLOC_ASSERT_RETURNS(
        dart_team_destroy(&local_team),
        DART_OK);
    }

    _num_nodes = num_hosts;
    for (const auto & host_dom : _host_domains) {
      if (host_dom.level > 0) {
        _num_nodes--;
      }
    }
  }
  DYLOC_ASSERT_RETURNS(
    dart_group_destroy(&local_group),
    DART_OK);

  /* Classify hostnames into categories 'node' and 'module'.
   * Typically, modules have the hostname of their nodes as prefix in their
   * hostname, e.g.:
   *
   *   computer-node-124           <-- node, heterogenous
   *   |- compute_node-124-sys     <-- module, homogenous
   *   |- compute-node-124-mic0    <-- module, homogenous
   *   '- compute-node-124-mic1    <-- module, homogenous
   *
   * Find shortest strings in array of distinct host names:
   */
  int hostname_min_len = std::numeric_limits<int>::max();
  int hostname_max_len = 0;
  for (auto & host_dom : _host_domains) {
    host_dom.level     = 0;
    host_dom.parent[0] = '\0';
    int hostname_len = strlen(host_dom.host);
    if (hostname_len < hostname_min_len) {
      hostname_min_len = hostname_len;
    }
    if (hostname_len > hostname_max_len) {
      hostname_max_len = hostname_len;
    }
  }

  _num_host_levels = 0;
  _num_nodes       = num_hosts;
  if (hostname_min_len != hostname_max_len) {
    _num_nodes = 0;
    int num_modules = 0;
    /* Match short hostnames as prefix of every other hostname: */
    for (auto & host_dom_top : _host_domains) {
      if (strlen(host_dom_top.host) == (size_t)hostname_min_len) {
        ++_num_nodes;
        /* Host name is node, find its modules in all other hostnames: */
        char * short_name = host_dom_top.host;
        for (auto & host_dom_sub : _host_domains) {
          char * other_name = host_dom_sub.host;
          /* Other hostname is longer and has short host name in prefix: */
          if (strlen(other_name) > (size_t)hostname_min_len &&
              strncmp(short_name, other_name, hostname_min_len) == 0) {
            num_modules++;
            /* Increment topology level of other host: */
            int node_level = host_dom_top.level + 1;
            if (node_level > _num_host_levels) {
              _num_host_levels = node_level;
            }
            host_dom_sub.level = node_level;
            /* Set short hostname as parent: */
            strncpy(host_dom_sub.parent, short_name,
                    DART_LOCALITY_HOST_MAX_SIZE);
          }
        }
      }
    }
    if (num_hosts > _num_nodes + num_modules) {
      /* some hosts are modules of node that is not in host names: */
      _num_nodes += num_hosts - (_num_nodes + num_modules);
    }
  }
}

void host_topology::local_topology(
  const unit_mapping                   & unit_map,
  std::vector<dyloc_module_location_t> & module_locations) {
#if defined(DART_ENABLE_HWLOC) && defined(DART_ENABLE_HWLOC_PCI)
  dyloc__unused(unit_map);

  hwloc_topology_t topology;
  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology,
#if HWLOC_API_VERSION < 0x00020000
                             HWLOC_TOPOLOGY_FLAG_IO_DEVICES
                           | HWLOC_TOPOLOGY_FLAG_IO_BRIDGES
#else
                             HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM
#endif
                          );
  hwloc_topology_load(topology);
  DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                  "hwloc: indexing PCI devices");
  /* Alternative: HWLOC_TYPE_DEPTH_PCI_DEVICE */
  int n_pcidev = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_PCI_DEVICE);

  DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                  "hwloc:", n_pcidev, "PCI devices found");
  for (int pcidev_idx = 0; pcidev_idx < n_pcidev; pcidev_idx++) {
    hwloc_obj_t coproc_obj =
      hwloc_get_obj_by_type(topology, HWLOC_OBJ_PCI_DEVICE, pcidev_idx);
    if (NULL != coproc_obj) {
      DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                      "hwloc: PCI device: (",
                      "name:",  coproc_obj->name,
                      "arity:", coproc_obj->arity,
                      ")");
      if (NULL != coproc_obj->name &&
          NULL != strstr(coproc_obj->name, "Xeon Phi")) {
        DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                        "hwloc: Xeon Phi device");
        if (coproc_obj->arity > 0) {
          for (int pd_i = 0; pd_i < (int)coproc_obj->arity; pd_i++) {
            hwloc_obj_t coproc_child_obj = coproc_obj->children[pd_i];
            DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                            "hwloc: Xeon Phi child node: (",
                            "name:",  coproc_child_obj->name,
                            "arity:", coproc_child_obj->arity,
                            ")");

            dyloc_module_location_t module_loc;
            char * hostname     = module_loc.host;
            char * mic_hostname = module_loc.module;
            char * mic_dev_name = coproc_child_obj->name;

            gethostname(hostname, DART_LOCALITY_HOST_MAX_SIZE);

            int n_chars_written = snprintf(
                                    mic_hostname, DART_LOCALITY_HOST_MAX_SIZE,
                                    "%s-%s", hostname, mic_dev_name);
            if (n_chars_written < 0 ||
                n_chars_written >= DART_LOCALITY_HOST_MAX_SIZE) {
              DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                              "MIC host name '", hostname, "-", mic_dev_name,
                              "' could not be assigned");
            }

            DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                            "hwloc:",
                            "Xeon Phi module hostname:", module_loc.module,
                            "node hostname:", module_loc.host);

            /* Get host of MIC device: */
            hwloc_obj_t mic_host_obj =
              hwloc_get_non_io_ancestor_obj(topology, coproc_obj);
            if (mic_host_obj != NULL) {
              module_loc.pos.scope =
                dyloc__hwloc_obj_type_to_scope(mic_host_obj->type);
              module_loc.pos.index = mic_host_obj->logical_index;
              DYLOC_LOG_TRACE("dylocxx::host_topology.local_topology",
                              "hwloc: Xeon Phi scope pos: ("
                              "type:", mic_host_obj->type,
                              "->",
                              "scope:", module_loc.pos.scope,
                              "idx:",   module_loc.pos.index,
                              ")");
            }
            module_locations.push_back(std::move(module_loc));
          }
        }
      }
    }
  }
  hwloc_topology_destroy(topology);
#else // ifdef DART_ENABLE_HWLOC
  dyloc__unused(unit_map);
  dyloc__unused(module_locations);
#endif // ifdef DART_ENABLE_HWLOC
}

} // namespace dyloc
