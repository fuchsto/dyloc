#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_mapping.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <functional>


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
          = std::unordered_map<
              std::string,
              std::vector<dart_global_unit_t> >;

  using module_domain_list_t
          = std::vector<
                std::reference_wrapper<dyloc_host_domain_t> >;


  // mapping <node host name> -> <node domain>
  using node_domain_map_t
          = std::unordered_map<
              std::string,
              std::reference_wrapper<dyloc_host_domain_t> >;
  // mapping <module host name> -> <module domain>
  using module_domain_map_t
          = std::unordered_map<
              std::string,
              std::reference_wrapper<dyloc_host_domain_t> >;
  // mapping <node host name> -> [ <module domain>, ... ]
  using node_modules_domain_map_t
          = std::unordered_map<
              std::string,
              module_domain_list_t >;

 private:
  // Mapping host name to unit ids located at hosts.
  host_units_map_t          _host_units;
  // Mapping host name to basic host domain data.
  host_domain_map_t         _host_domains;

  node_domain_map_t         _node_domains;
  module_domain_map_t       _module_domains;

  node_modules_domain_map_t _node_module_domains;

  int _num_host_levels = 0;

 public:
  host_topology() = delete;
  host_topology(const unit_mapping & unit_map);

  inline const node_domain_map_t & nodes() const noexcept {
    return _node_domains;
  }

  inline const module_domain_map_t & modules() const noexcept {
    return _module_domains;
  }

  inline const module_domain_list_t & node_modules(
      const std::string & node_hostname) const noexcept {
    return _node_module_domains.at(node_hostname);
  }

  inline const std::vector<dart_global_unit_t> & unit_ids(
      const std::string & hostname) const noexcept {
    return _host_units.at(hostname);
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
