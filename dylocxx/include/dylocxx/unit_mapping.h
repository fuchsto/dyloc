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
  std::vector<unit_locality>  unit_localities;
  dart_team_t                 team;
  
 public:
  typedef std::vector<unit_locality>::iterator
    unit_locality_iterator;
  typedef std::vector<unit_locality>::const_iterator
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

  inline unit_locality_const_iterator begin() const {
    return unit_localities.begin();
  }

  inline unit_locality_const_iterator end() const {
    return unit_localities.end();
  }

  inline size_t size() const noexcept {
    return unit_localities.size();
  }

  inline const unit_locality & operator[](
      dart_team_unit_t luid) const {
    return unit_localities[luid.id];
  }

  inline unit_locality & operator[](
      dart_team_unit_t luid) {
    return unit_localities[luid.id];
  }
};

} // namespace dyloc

#endif // DYLOCXX__UNIT_MAPPING_H__INCLUDED
