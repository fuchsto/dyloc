#ifndef DYLOCXX__INTERNAL__ITERATOR_H__INCLUDED
#define DYLOCXX__INTERNAL__ITERATOR_H__INCLUDED

#include <iterator>


namespace dyloc {

template <
  class IteratorType,
  class ValueType    = typename std::iterator_traits<
                                  IteratorType
                                >::value_type,
  class IndexType    = std::ptrdiff_t,
  class Pointer      = ValueType *,
  class Reference    = ValueType & >
class index_iterator_base
: public std::iterator<
            std::random_access_iterator_tag,
            ValueType,
            IndexType,
            Pointer,
            Reference >
{
  typedef index_iterator_base<
            IteratorType,
            ValueType,
            IndexType,
            Pointer,
            Reference >          self_t;
  typedef IndexType              index_type;
  typedef IteratorType           derived_t;
 private:
  index_type _pos;

 private:
  derived_t & derived() {
    return static_cast<IteratorType &>(*this);
  }
  constexpr const derived_t & derived() const {
    return static_cast<const derived_t &>(*this);
  }

 public:
  typedef std::random_access_iterator_tag       iterator_category;

  typedef ValueType                                    value_type;
  typedef index_type                              difference_type;
  typedef Pointer                                         pointer;
  typedef const Pointer                             const_pointer;
  typedef Reference                                     reference;
  typedef const Reference                         const_reference;

 protected:
  constexpr index_iterator_base()                = delete;
  constexpr index_iterator_base(self_t &&)       = default;
  constexpr index_iterator_base(const self_t &)  = default;
  ~index_iterator_base()                         = default;
  self_t & operator=(self_t &&)                  = default;
  self_t & operator=(const self_t &)             = default;

  constexpr explicit index_iterator_base(index_type position)
  : _pos(position)
  { }

  constexpr index_type pos() const {
    return _pos;
  }

  constexpr reference operator*() const {
    return derived().dereference(_pos);
  }

  constexpr reference operator->() const {
    return derived().dereference(_pos);
  }

  constexpr reference operator[](int pos) const {
    return derived().dereference(pos);
  }

  derived_t & operator++() {
    _pos++;
    return derived();
  }

  derived_t & operator--() {
    _pos--;
    return derived();
  }

  derived_t & operator+=(int i) {
    _pos += i;
    return derived();
  }

  derived_t & operator-=(int i) {
    _pos -= i;
    return derived();
  }

  constexpr derived_t operator+(int i) const {
    return derived_t(derived(), _pos + i);
  }

  constexpr derived_t operator-(int i) const {
    return derived_t(derived(), _pos - i);
  }

  constexpr index_type operator+(const derived_t & rhs) const {
    return _pos + rhs._pos;
  }

  constexpr index_type operator-(const derived_t & rhs) const {
    return _pos - rhs._pos;
  }

  constexpr bool operator==(const derived_t & rhs) const {
    return _pos == rhs._pos;
  }

  constexpr bool operator!=(const derived_t & rhs) const {
    return _pos != rhs._pos;
  }

  constexpr bool operator<(const derived_t & rhs) const {
    return _pos < rhs._pos;
  }

  constexpr bool operator<=(const derived_t & rhs) const {
    return _pos <= rhs._pos;
  }

  constexpr bool operator>(const derived_t & rhs) const {
    return _pos > rhs._pos;
  }

  constexpr bool operator>=(const derived_t & rhs) const {
    return _pos >= rhs._pos;
  }
};

} // namespace dyloc

#endif // DYLOCXX__INTERNAL__ITERATOR_H__INCLUDED
