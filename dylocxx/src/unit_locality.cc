
#include <dylocxx/unit_locality.h>
#include <dylocxx/hwinfo.h>

#include <dyloc/common/types.h>
#include <dash/dart/if/dart_types.h>

#include <iostream>
#include <sstream>


namespace dyloc {

std::ostream & operator<<(
  std::ostream & os,
  const dyloc_unit_locality_t & uloc) {
  std::ostringstream ss;
  ss << "dyloc_unit_locality_t { "
     << "tag:"    << uloc.domain_tag << " "
     << "unit:"   << uloc.unit.id << " (t:" << uloc.team << ") "
     << "hwinfo:" << uloc.hwinfo
     << " }";
  return operator<<(os, ss.str());
}

unit_locality::unit_locality(dart_team_unit_t u, dart_team_t t) {
  hwinfo unit_hwinfo;
  unit_hwinfo.collect();

  _uloc.domain_tag[0] = '\0';
  _uloc.unit          = u;
  _uloc.team          = t;
  _uloc.hwinfo        = *unit_hwinfo.data();
}

} // namespace dyloc

