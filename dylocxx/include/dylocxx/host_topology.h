#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_mapping.h>

#include <dyloc/common/host_topology.h>

#include <unordered_map>
#include <vector>
#include <string>


namespace dyloc {

class host_topology : dyloc_host_domain_t {
 private:
  dyloc_host_topology_t _host_topo;

 private:
  // Mapping host name to unit ids located at hosts.
  std::unorderd_multimap<std::string, dart_global_unit_t> _host_units;
  std::vector<dyloc_host_domain_t>                        _host_domains;

 private:
  const unit_mapping & _unit_map;

 public:
  host_topology(const unit_mapping & unit_map);

  inline const dyloc_host_topology_t * data() const noexcept {
    return &_host_topo;
  }

 private:
  void update_module_locations(const unit_mapping & unit_map);
};

} // namespace dyloc

#endif DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
