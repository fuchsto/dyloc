#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_mapping.h>

#include <unordered_map>
#include <vector>
#include <string>


namespace dyloc {

typedef struct {
  char                       host[DART_LOCALITY_HOST_MAX_SIZE];
  char                       parent[DART_LOCALITY_HOST_MAX_SIZE];
  dyloc_locality_scope_pos_t scope_pos;
  int                        numa_ids[DART_LOCALITY_MAX_NUMA_ID];
  int                        num_numa;
  int                        num_cores;
  int                        level;
}
dyloc_host_domain_t;

class host_topology {
  using host_domain_map_t
          = std::vector<dyloc_host_domain_t>;
  using host_units_map_t
          = std::unordered_map<std::string, std::vector<dart_global_unit_t>>;

 private:
  // Mapping host name to unit ids located at hosts.
  host_units_map_t   _host_units;
  // Mapping host name to basic host domain data.
  host_domain_map_t  _host_domains;

  int _num_nodes;
  int _num_hosts;
  int _num_host_levels;

 private:
  const unit_mapping & _unit_map;

 public:
  host_topology(const unit_mapping & unit_map);

 private:
  void collect_topology(
         const unit_mapping & unit_map);
  void local_topology(
         const unit_mapping                   & unit_map,
         std::vector<dyloc_module_location_t> & module_locations);
};

} // namespace dyloc

#endif // DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
