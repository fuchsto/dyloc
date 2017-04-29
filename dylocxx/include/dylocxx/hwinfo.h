#ifndef DYLOCXX__HWINFO_H__INCLUDED
#define DYLOCXX__HWINFO_H__INCLUDED

#include <dyloc/common/types.h>


namespace dyloc {

class hwinfo {

 private:
  static constexpr int MBYTES = 1024 * 1024;

 private:
  dyloc_hwinfo_t _hw;

 public:
  hwinfo();
  ~hwinfo();

  void collect();

  inline const dyloc_hwinfo_t * data() const noexcept {
    return &_hw;
  }
};

} // namespace dyloc

#endif // DYLOCXX__HWINFO_H__INCLUDED
