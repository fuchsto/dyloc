#ifndef DYLOC__INTERNAL__ALGORITHM_H__INCLUDED
#define DYLOC__INTERNAL__ALGORITHM_H__INCLUDED

#include <string>
#include <iterator>
#include <set>


namespace dyloc {

template <class Iterator, class Sentinel>
std::string longest_common_prefix(
       const Iterator & string_first,
       const Sentinel & string_last) {
  std::string prefix;
  size_t      pos  = 0;
  for (; pos < string_first->length(); ++pos) {
    char c = (*string_first)[pos];
    // Test if character at position `pos` is identical in all
    // domain tags:
    for (auto it = std::next(string_first); it != string_last; ++it) {
      if (pos >= it->length() || (*it)[pos] != c) {
        return prefix;
      }
    }
    prefix.push_back(c);
  }
  return prefix;
}

template<class ForwardIt>
int count_unique(ForwardIt first, ForwardIt last) {
  if (first == last) { return 0; }
  using value_t = typename std::iterator_traits<ForwardIt>::value_type;
  std::set<value_t> seen;
  seen.insert(first, last);
  return seen.size();
}

} // namespace dyloc

#endif // DYLOC__INTERNAL__ALGORITHM_H__INCLUDED
