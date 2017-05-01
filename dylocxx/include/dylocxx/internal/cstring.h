#ifndef DYLOCXX__INTERNAL__CSTRING_H__INCLUDED
#define DYLOCXX__INTERNAL__CSTRING_H__INCLUDED

#include <array>


namespace dyloc {

template <int N>
class cstring {
  std::array<char, N+1> _cstr;

public:
  cstring() { _cstr[0] = '\0'; }

  cstring(cstring const&)            = default;
  cstring(cstring&&)                 = default;
  cstring& operator=(cstring const&) = default;
  cstring& operator=(cstring&&)      = default;
  ~cstring()                         = default;

  inline char * data() const noexcept {
    return _cstr.data();
  }
};

} // namespace dyloc

#endif // DYLOCXX__INTERNAL__CSTRING_H__INCLUDED
