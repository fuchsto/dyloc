#ifndef DYLOCXX__UNIT_MAPPING_H__INCLUDED
#define DYLOCXX__UNIT_MAPPING_H__INCLUDED

#include <dyloc/common/types.h>

#include <dash/dart/if/dart_types.h>

#include <vector>


namespace dyloc {

struct unit_mapping {
  std::vector<dyloc_unit_locality_t>  unit_localities;
  dart_team_t                         team;

  const dyloc_unit_locality_t & operator[](dart_team_unit_t luid) const {
    return unit_localities[luid.id];
  }
};

} // namespace dyloc

#endif // DYLOCXX__UNIT_MAPPING_H__INCLUDED
