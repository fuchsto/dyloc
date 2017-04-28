#ifndef DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED
#define DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <string>
#include <vector>
#include <memory>


namespace dyloc {

class locality_domain {
  typedef locality_domain self_t;

 public:
  std::string                     domain_tag;
  dyloc_locality_scope_t          scope     = DYLOC_LOCALITY_SCOPE_UNDEFINED;
  int                             level     = -1;
  int                             rel_index = -1;
  dart_team_t                     team      = DART_TEAM_NULL;
  std::shared_ptr<self_t>         parent    = nullptr;
  std::vector<dart_global_unit_t> unit_ids;
  std::vector<self_t>             children;
};

} // namespace dyloc

#endif // DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED
