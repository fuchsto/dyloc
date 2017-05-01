
#include <dylocxx/locality_domain.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <sstream>


namespace dyloc {

locality_domain::locality_domain(
  const locality_domain  & parent,
  dyloc_locality_scope_t   lscope,
  int                      child_index) {
  std::ostringstream ss;
  if (*parent.domain_tag.rbegin() != '.') {
    ss << '.';
  }
  ss << parent.domain_tag << child_index;

  scope      = lscope;
  level      = parent.level + 1;
  r_index    = child_index;
  g_index    = child_index;
  team       = parent.team;
  domain_tag = ss.str();
}

locality_domain::locality_domain(dart_team_t t) {
  scope      = DYLOC_LOCALITY_SCOPE_GLOBAL;
  level      = 0;
  r_index    = 0;
  g_index    = 0;
  team       = t;
  domain_tag = ".";

  size_t num_units = 0;
  DYLOC_ASSERT_RETURNS(dart_team_size(team, &num_units), DART_OK);
  DYLOC_LOG_DEBUG("dylocxx::locality_domain.()",
                  "team:",  team,
                  "units:", num_units);
  for (int u = 0; u < static_cast<int>(num_units); ++u) {
    dart_team_unit_t   luid = { u };
    dart_global_unit_t guid;
    DYLOC_ASSERT_RETURNS(
      dart_team_unit_l2g(team, luid, &guid),
      DART_OK);
    unit_ids.push_back(guid);
  }
}

} // namespace dyloc
