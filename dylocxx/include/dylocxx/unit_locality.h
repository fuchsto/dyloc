#ifndef DYLOCXX__UNIT_LOCALITY_H__INCLUDED
#define DYLOCXX__UNIT_LOCALITY_H__INCLUDED

#include <dylocxx/hwinfo.h>
#include <dyloc/common/types.h>
#include <dash/dart/if/dart_types.h>

#include <vector>
#include <iostream>


namespace dyloc {

/**
 * Collects hardware information of a single unit locality.
 *
 */
class unit_locality {

 private:
  dyloc_unit_locality_t _uloc;

 public:
  unit_locality() = default;
  unit_locality(const dyloc_unit_locality_t & uloc) : _uloc(uloc) { }

  /**
   * Obtain local hardware locality in unit scope.
   */
  unit_locality(dart_team_unit_t u, dart_team_t team);

  inline const dyloc_unit_locality_t * data() const noexcept {
    return &_uloc;
  }
};

std::ostream & operator<<(
  std::ostream                & os,
  const dyloc_unit_locality_t & uloc);

} // namespace dyloc

#endif // DYLOCXX__UNIT_LOCALITY_H__INCLUDED
