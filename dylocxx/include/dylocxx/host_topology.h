#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_mapping.h>

#include <dyloc/common/host_topology.h>

#include <unordered_map>
#include <vector>
#include <string>


namespace dyloc {

class host_domain {
  friend class host_topology;

 private:
  dyloc_host_domain_t _host_dom;

 public:
  std::string                 host;
  std::string                 parent;
  std::vector<int>            numa_ids;
  int                         level;
  dyloc_locality_scope_pos_t  scope_pos;

 public:
  // TODO
  host_domain() { }

  inline const dyloc_host_domain_t * data() const noexcept {
    return &_host_dom;
  }
};

class host_topology {
  using host_domain_map_t
          = std::unordered_map<std::string, host_domain>;
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
