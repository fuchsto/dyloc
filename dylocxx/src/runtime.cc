
#include <dylocxx/runtime.h>

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/unit_locality.h>
#include <dylocxx/locality_domain.h>

#include <dylocxx/topology.h>

#include <dylocxx/adapter/dart.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

void runtime::initialize() {
  // TODO: initialize global host (hardware) topology here
  initialize_locality(DART_TEAM_ALL);
}

void runtime::finalize() {
  _unit_mappings.clear();
  _host_topologies.clear();
}

void runtime::initialize_locality(dart_team_t team) {
  DYLOC_LOG_DEBUG("dylocxx::runtime.initialize_locality", "() team:", team);

  DYLOC_LOG_DEBUG("dylocxx::runtime.initialize_locality", "unit mappings");
  _unit_mappings.insert(
      std::make_pair(team, unit_mapping(team)));

  DYLOC_LOG_DEBUG("dylocxx::runtime.initialize_locality", "host topologies");
  _host_topologies.insert(
      std::make_pair(
        team,
        host_topology(_unit_mappings.at(team))));

  DYLOC_LOG_DEBUG("dylocxx::runtime.initialize_locality", "domain graph");
  _topologies.insert(
      std::make_pair(
        team,
        topology(
          team,
          _host_topologies.at(team),
          _unit_mappings.at(team))));

  DYLOC_LOG_DEBUG("dylocxx::runtime.initialize_locality", ">");
}

void runtime::finalize_locality(dart_team_t team) {
  _topologies.erase(
      _topologies.find(team));
  _unit_mappings.erase(
      _unit_mappings.find(team));
  _host_topologies.erase(
      _host_topologies.find(team));
}

} // namespace dyloc

