#ifndef DYLOCXX__UTILITY_H__INCLUDED
#define DYLOCXX__UTILITY_H__INCLUDED

#include <iostream>
#include <sstream>
#include <iterator>


namespace dyloc {

template <
  typename Iterator,
  typename Sentinel >
class range {
  typedef range<Iterator, Sentinel> self_t;

  const Iterator & _begin;
  const Sentinel & _end;

public:
  typedef Iterator iterator;
  typedef Sentinel sentinel;

public:
  constexpr range(const iterator & begin, const sentinel & end)
  : _begin(begin)
  , _end(end)
  { }

  constexpr const iterator & begin() const { return _begin; }
  constexpr const iterator & end()   const { return _end;   }
};

template <class Iterator, class Sentinel>
dyloc::range<Iterator, Sentinel> make_range(
  const Iterator & first,
  const Sentinel & last) {
  return dyloc::range<Iterator, Sentinel>(first, last);
}

template <class Iterator, class Sentinel>
std::ostream & operator<<(
  std::ostream                           & os,
  const dyloc::range<Iterator, Sentinel> & range)
{
  typedef typename std::iterator_traits<Iterator>::value_type
    value_t;

  std::ostringstream ss;
  int pos = 0;
  ss << " { ";
  for (auto it = std::begin(range); it != std::end(range); ++it, ++pos) {
    ss << static_cast<const value_t>(*it) << " ";
  }
  ss << "}";
  return operator<<(os, ss.str());
}

template <class Iterator, class Sentinel>
std::ostream & operator<<(
  std::ostream                      & os,
  dyloc::range<Iterator, Sentinel> && range)
{
  typedef dyloc::range<Iterator, Sentinel>
    range_t;
  typedef typename std::iterator_traits<Iterator>::value_type
    value_t;

  auto && rng = std::forward<range_t>(range);

  std::ostringstream ss;
  int pos = 0;
  ss << " { ";
  for (auto it = std::begin(rng); it != std::end(rng); ++it, ++pos) {
    ss << static_cast<const value_t>(*it) << " ";
  }
  ss << "}";
  return operator<<(os, ss.str());
}

} // namespace dyloc

#endif // DYLOCXX__UTILITY_H__INCLUDED
