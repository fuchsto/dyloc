#ifndef DYLOCXX__UNIT_LOCALITY_H__INCLUDED
#define DYLOCXX__UNIT_LOCALITY_H__INCLUDED

#include <dylocxx/hwinfo.h>
#include <dyloc/common/types.h>
#include <dash/dart/if/dart_types.h>

#include <vector>


namespace dyloc {

class unit_locality {

 private:
  dyloc_unit_locality_t _uloc;

 public:
  unit_locality() = default;

  /**
   * Obtain local hardware locality in unit scope.
   */
  unit_locality(dart_team_t team);

  inline const dyloc_unit_locality_t * data() const noexcept {
    return &_uloc;
  }
};

} // namespace dyloc

#endif // DYLOCXX__UNIT_LOCALITY_H__INCLUDED
