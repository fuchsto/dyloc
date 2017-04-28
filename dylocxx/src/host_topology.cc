
#include <dylocxx/unit_mapping.h>
#include <dylocxx/host_topology.h>

#include <dyloc/common/host_topology.h>

#include <vector>
#include <string>


namespace dyloc {

host_topology::host_topology(const unit_mapping & unit_map)
: _unit_map(unit_map) {

  dart_team_t team = unit_mapping.team;
  size_t num_units;

  DYLOC_LOG_TRACE("dylocxx::host_topology(): team:" << team);

  DYLOC_ASSERT_RETURNS(dart_team_size(team, &num_units), DART_OK);
  DYLOC_ASSERT_MSG(num_units == unit_mapping.num_units,
                  "Number of units in mapping differs from team size");

  /* Copy host names of all units into array:
   */
  DYLOC_LOG_TRACE("dylocxx::host_topology: copying host names");
  for (size_t u = 0; u < num_units; ++u) {
    _host_units.insert(std::make_pair(_unit_map[u].hwinfo().host, u));
  }

  /* Map units to hosts domains:
   */
  this->_host_domains.reserve(num_hosts);

  for (int h = 0; h < num_hosts; ++h) {
    dart_host_domain_t * host_domain = &this->_host_domains[h];
    dart_host_units_t  * host_units  = &this->_host_units[h];
    /* Histogram of NUMA ids: */
    int numa_id_hist[DYLOC_LOCALITY_MAX_NUMA_ID] = { 0 };
    /* Allocate array with capacity of maximum units on a single host: */
    host_units->units      = malloc(sizeof(dart_global_unit_t)
                                          * max_host_units);
    host_units->num_units  = 0;
    host_domain->host[0]   = '\0';
    host_domain->parent[0] = '\0';
    host_domain->num_numa  = 0;
    host_domain->level     = 0;
    host_domain->scope_pos.scope = DYLOC_LOCALITY_SCOPE_NODE;
    host_domain->scope_pos.index = 0;

    memset(host_domain->numa_ids, 0,
           sizeof(int) * DYLOC_LOCALITY_MAX_NUMA_ID);
    strncpy(host_domain->host, hostnames[h], max_host_len);

    DYLOC_LOG_TRACE("dylocxx::host_topology(): mapping units to ",
                   hostnames[h]);
    /* Iterate over all units: */
    for (size_t u = 0; u < num_units; ++u) {
      dart_team_unit_t luid = { u };
      const auto &     ul   = _unit_map[luid];

      if (strncmp(ul->hwinfo.host, hostnames[h], max_host_len) == 0) {
        /* Unit is local to host at index h: */
        dart_global_unit_t guid;
        DYLOC_ASSERT_RETURNS(
          dart_team_unit_l2g(team, ul.unit, &guid),
          DART_OK);
        host_units->units[host_units->num_units] = guid;
        host_units->num_units++;

        int unit_numa_id = ul->hwinfo.numa_id;

        DYLOC_LOG_TRACE("dylocxx::host_topology(): "
                       "mapping unit " << u << " " <<
                       "to host '" << _host_names[h] << "', " <<
                       "NUMA id: " << unit_numa_id);
        if (unit_numa_id >= 0) {
          if (numa_id_hist[unit_numa_id] == 0) {
            host_domain->numa_ids[host_domain->num_numa] = unit_numa_id;
            host_domain->num_numa++;
          }
          numa_id_hist[unit_numa_id]++;
        }
      }
    }
    DYLOC_LOG_TRACE("dylocxx::host_topology(): "
                    "found " << host_domain->num_numa << 
                    "NUMA domains on host " << hostnames[h]);
    for (int n = 0; n < host_domain->num_numa; ++n) {
      DYLOC_LOG_TRACE("dylocxx::host_topology(): numa_id[" << n << "]:" <<
                      host_domain->numa_ids[n]);
    }

    /* Shrink unit array to required capacity: */
    if (host_units->num_units < max_host_units) {
      DYLOC_LOG_TRACE("dylocxx::host_topology(): shrinking node unit array "
                      "from " << max_host_units << " "
                      "to "   << host_units->num_units << " elements");
      // Either   realloc(addr != 0, n >= 0) -> free or realloc
      // or       realloc(addr  = 0, n >  0) -> malloc
      DYLOC_ASSERT(host_units->units     != NULL ||
                   host_units->num_units  > 0);
      if (host_units->num_units > 0) {
        host_units->units = realloc(host_units->units,
                                    host_units->num_units *
                                      sizeof(dart_global_unit_t));
        DYLOC_ASSERT(host_units->units != NULL);
      } else {
        free(host_units->units);
        host_units->units = NULL;
      }
    }
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
