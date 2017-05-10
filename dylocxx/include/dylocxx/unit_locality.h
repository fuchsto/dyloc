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

  /**
   * Converting constructor.
   */
  unit_locality(const dyloc_unit_locality_t & uloc);

  unit_locality(const unit_locality & other)
  : unit_locality(*other.data())
  { }

  unit_locality & operator=(const unit_locality & uloc);

  /**
   * Obtain local hardware locality in unit scope.
   */
  unit_locality(dart_team_unit_t u, dart_team_t team);

  inline const dyloc_unit_locality_t * data() const noexcept {
    return &_uloc;
  }

  inline dyloc_unit_locality_t * data() noexcept {
    return &_uloc;
  }

  operator dyloc_unit_locality_t & () {
    return _uloc;
  }

  operator const dyloc_unit_locality_t & () const {
    return _uloc;
  }
};

std::ostream & operator<<(
  std::ostream                & os,
  const dyloc_unit_locality_t & uloc);

std::ostream & operator<<(
  std::ostream                & os,
  const dart_team_unit_t      & tuid);

std::ostream & operator<<(
  std::ostream                & os,
  const dart_global_unit_t    & guid);

} // namespace dyloc

#endif // DYLOCXX__UNIT_LOCALITY_H__INCLUDED
