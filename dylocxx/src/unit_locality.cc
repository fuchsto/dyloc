
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dyloc/common/types.h>
#include <dash/dart/if/dart_types.h>


namespace dyloc {

unit_locality::unit_locality(dart_team_t t) {
  hwinfo unit_hwinfo;
  unit_hwinfo.collect();
}

} // namespace dyloc

