#ifndef DYLOCXX__RUNTIME_H__INCLUDED
#define DYLOCXX__RUNTIME_H__INCLUDED

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/locality_domain.h>
#include <dylocxx/topology.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart_types.h>

#include <vector>


namespace dyloc {

class runtime {
  std::unordered_map<dart_team_t, host_topology>   _host_topologies;
  std::unordered_map<dart_team_t, unit_mapping>    _unit_mappings;
  std::unordered_map<dart_team_t, topology>        _topologies;

 public:
  void initialize();
  void finalize();

  void initialize_locality(dart_team_t team);
  void finalize_locality(dart_team_t team);

  const dyloc::unit_locality & unit_locality(
          dart_team_t t,
          dart_team_unit_t u) {
    return _unit_mappings.at(t)[u];
  }

  const dyloc::unit_locality & unit_locality(
          dart_global_unit_t u) {
    // Unit id in team ALL is identical to global unit id:
    return _unit_mappings.at(DART_TEAM_ALL)[u.id];
  }

  topology & team_topology(
    dart_team_t t) {
    return _topologies.at(t);
  }
};

} // namespace dyloc

#endif // DYLOCXX__RUNTIME_H__INCLUDED
