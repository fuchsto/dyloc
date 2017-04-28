
#include <dylocxx/unit_mapping.h>
#include <dylocxx/unit_locality.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

unit_mapping::unit_mapping(dart_team_t t)
: team(t) {
  dart_ret_t       ret;
  dart_team_unit_t myid   = DART_UNDEFINED_TEAM_UNIT_ID;
  size_t           nunits = 0;
  DYLOC_ASSERT_RETURNS(dart_team_myid(team, &myid),   DART_OK);
  DYLOC_ASSERT_RETURNS(dart_team_size(team, &nunits), DART_OK);

  unit_localities.resize(nunits);
  unit_locality uloc(team);

  dart_barrier(team);

  /* all-to-all exchange of locality data across all units:
   * (send, recv, nbytes, team) */
  DYLOC_LOG_DEBUG("dylocxx::unit_mapping: dart_allgather");
  ret = dart_allgather(uloc.data(),
                       unit_localities.data(),
                       sizeof(dyloc_unit_locality_t),
                       DART_TYPE_BYTE,
                       team);

  dart_barrier(team);

}

} // namespace dyloc

