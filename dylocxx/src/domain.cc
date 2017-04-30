
#include <dylocxx/domain.h>
#include <dylocxx/hwinfo.h>

#include <dylocxx/internal/logging.h>

#include <dyloc/common/types.h>

#include <vector>
#include <string>


namespace dyloc {

domain::domain() {
}

void domain::update_locality_domain_data() const noexcept {
  std::copy(_tag.begin(),   _tag.end(),   _dom.domain_tag);
  std::copy(_units.begin(), _units.end(), _dom.unit_ids);

  _dom.num_units  = _units.size();
  _dom.arity      = _children.size();
  // _dom.children   = _children.data();
}

} // namespace dyloc
