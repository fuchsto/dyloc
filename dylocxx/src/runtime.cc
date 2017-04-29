
#include <dylocxx/runtime.h>

#include <dylocxx/host_topology.h>
#include <dylocxx/unit_mapping.h>
#include <dylocxx/locality_domain.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

void runtime::initialize_locality(dart_team_t team) {
  locality_domain team_global_dom;

  team_global_dom.scope      = DYLOC_LOCALITY_SCOPE_GLOBAL;
  team_global_dom.level      = 0;
  team_global_dom.rel_index  = 0;
  team_global_dom.team       = team;
  team_global_dom.domain_tag = ".";

  size_t num_units = 0;
  DYLOC_ASSERT_RETURNS(dart_team_size(team, &num_units), DART_OK);

  for (int u = 0; u < static_cast<int>(num_units); ++u) {
    dart_team_unit_t   luid = { u };
    dart_global_unit_t guid;
    DYLOC_ASSERT_RETURNS(
      dart_team_unit_l2g(team, luid, &guid),
      DART_OK);
    team_global_dom.unit_ids.push_back(guid);
  }

  _locality_domains.insert(
      std::make_pair(team, team_global_dom));

  _unit_mappings.insert(
      std::make_pair(team, unit_mapping(team)));

  _host_topologies.insert(
      std::make_pair(
        team,
        host_topology(_unit_mappings[team])));
}

void runtime::finalize_locality(dart_team_t team) {
  _locality_domains.erase(_locality_domains.find(team));
}

} // namespace dyloc

