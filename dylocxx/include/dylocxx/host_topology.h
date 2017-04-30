#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_mapping.h>

#include <dyloc/common/host_topology.h>

#include <unordered_map>
#include <vector>
#include <string>

/*
    typedef struct
    {
      char                       host[DART_LOCALITY_HOST_MAX_SIZE];
      char                       parent[DART_LOCALITY_HOST_MAX_SIZE];
      dyloc_locality_scope_pos_t scope_pos;
      int                        numa_ids[DART_LOCALITY_MAX_NUMA_ID];
      int                        num_numa;
      int                        level;
    }
    dyloc_host_domain_t;

    typedef struct
    {
      dart_global_unit_t       * units;
      int                        num_units;
    }
    dyloc_host_units_t;

    typedef struct
    {
      int                        num_nodes;
      int                        num_hosts;
      int                        num_host_levels;
      size_t                     num_units;
      char                    ** host_names;
      dyloc_host_units_t       * host_units;
      dyloc_host_domain_t      * host_domains;
    }
    dyloc_host_topology_t;
*/


namespace dyloc {

class host_topology {
  using host_domain_map_t
          = std::vector<dyloc_host_domain_t>;
  using host_units_map_t
          = std::unordered_map<std::string, std::vector<dart_global_unit_t>>;

 private:
  dyloc_host_topology_t _host_topo;

 private:
  // Mapping host name to unit ids located at hosts.
  host_units_map_t   _host_units;
  // Mapping host name to basic host domain data.
  host_domain_map_t  _host_domains;

 private:
  const unit_mapping & _unit_map;

 public:
  host_topology(const unit_mapping & unit_map);

  inline const dyloc_host_topology_t * data() const noexcept {
    return &_host_topo;
  }

 private:
  void collect_topology(
         const unit_mapping & unit_map);
  void local_topology(
         const unit_mapping                   & unit_map,
         std::vector<dyloc_module_location_t> & module_locations);
};

} // namespace dyloc

#endif // DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
