#ifndef DYLOCXX__UNIT_MAPPING_H__INCLUDED
#define DYLOCXX__UNIT_MAPPING_H__INCLUDED

#include <dylocxx/unit_locality.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart_types.h>

#include <vector>


namespace dyloc {

/**
 * Collectively gathers hardware information of unit localities in a
 * specified team.
 *
 */
struct unit_mapping {
  std::vector<dyloc_unit_locality_t>  unit_localities;
  dart_team_t                         team;
  
 public:
  typedef std::vector<dyloc_unit_locality_t>::iterator
    unit_locality_iterator;
  typedef std::vector<dyloc_unit_locality_t>::const_iterator
    unit_locality_const_iterator;

 public:
  unit_mapping()                                 = default;
  unit_mapping(const unit_mapping &)             = default;
  unit_mapping(unit_mapping &&)                  = default;
  unit_mapping & operator=(const unit_mapping &) = default;
  unit_mapping & operator=(unit_mapping &&)      = default;

  /**
   * Obtain global mapping of team units to hosts.
   */
  unit_mapping(dart_team_t team);

  unit_locality_const_iterator begin() const {
    return unit_localities.begin();
  }

  unit_locality_const_iterator end() const {
    return unit_localities.end();
  }

  const dyloc_unit_locality_t & operator[](dart_team_unit_t luid) const {
    return unit_localities[luid.id];
  }

  dyloc_unit_locality_t & operator[](dart_team_unit_t luid) {
    return unit_localities[luid.id];
  }
};

} // namespace dyloc

#endif // DYLOCXX__UNIT_MAPPING_H__INCLUDED
