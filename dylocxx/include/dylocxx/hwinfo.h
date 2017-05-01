#ifndef DYLOCXX__HWINFO_H__INCLUDED
#define DYLOCXX__HWINFO_H__INCLUDED

#include <dyloc/common/types.h>

#include <iostream>


namespace dyloc {

class hwinfo {

 private:
  static constexpr int MBYTES = 1024 * 1024;

 private:
  dyloc_hwinfo_t _hw;

 public:
  hwinfo();

  void collect();

  inline const dyloc_hwinfo_t * data() const noexcept {
    return &_hw;
  }
};

std::ostream & operator<<(
  std::ostream                 & os,
  const dyloc_locality_scope_t & scope);

std::ostream & operator<<(
  std::ostream         & os,
  const dyloc_hwinfo_t & hwinfo);

} // namespace dyloc

#endif // DYLOCXX__HWINFO_H__INCLUDED
