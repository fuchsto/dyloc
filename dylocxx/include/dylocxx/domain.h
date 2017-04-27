#ifndef DYLOCXX__DOMAIN_H__INCLUDED
#define DYLOCXX__DOMAIN_H__INCLUDED

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <vector>
#include <string>


namespace dyloc {

class domain {
 typedef domain self_t;

 private:
  std::string                     _host;
  std::string                     _tag;
  std::vector<self_t>             _children;
  std::vector<dart_global_unit_t> _units;

  mutable dyloc_locality_domain_t _dom;

 public:
  domain();
  domain(const self_t  & other);
  domain(self_t       && other);

  self_t & operator=(const self_t  & rhs);
  self_t & operator=(self_t       && rhs);

  bool operator==(const self_t & rhs) const;
  bool operator!=(const self_t & rhs) const;

  inline const dyloc_locality_domain_t * data() const noexcept {
    update_locality_domain_data();
    return &_dom;
  }

  inline const std::string & domain_tag() const noexcept {
    return _tag;
  }

 private:
  void update_locality_domain_data() const noexcept;
};

} // namespace dyloc

#endif // DYLOCXX__DOMAIN_H__INCLUDED
