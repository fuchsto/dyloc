#ifndef DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
#define DYLOCXX__HOST_TOPOLOGY_H__INCLUDED

#include <dylocxx/unit_locality.h>

#include <dyloc/common/host_topology.h>

#include <unordered_map>
#include <vector>
#include <string>


namespace dyloc {

class host_domain {
  friend class host_topology;

 private:
  dyloc_host_domain_t _host_dom;

 private:
  std::string                 _host;
  std::string                 _parent;
  std::vector<int>            _numa_ids;
  int                         _level;
  dyloc_locality_scope_pos_t  _scope_pos;

 public:
  host_domain();

  inline const dyloc_host_domain_t * data() const noexcept {
    return &_host_dom;
  }
};

class host_topology {
 private:
  dyloc_host_topology_t _host_topo;

 private:
  // Mapping host name to unit ids located at hosts.
  std::unorderd_map<std::string, std::vector<dart_global_unit_t>> _host_units;
  // Mapping host name to basic host domain data.
  std::unorderd_map<std::string, host_domain>                   _host_domains;

 private:
  const team_unit_localities & _unit_map;

 public:
  host_topology(const unit_mapping & unit_map);

  inline const dyloc_host_topology_t * data() const noexcept {
    return &_host_topo;
  }

 private:
  void update_module_locations(const unit_mapping & unit_map);
};

} // namespace dyloc

#endif // DYLOCXX__HOST_TOPOLOGY_H__INCLUDED
