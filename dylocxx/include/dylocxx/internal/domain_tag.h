#ifndef DYLOC__INTERNAL__DOMAIN_TAG_H__INCLUDED
#define DYLOC__INTERNAL__DOMAIN_TAG_H__INCLUDED

#include <dylocxx/internal/algorithm.h>

#include <dylocxx/exception.h>

#include <string>
#include <algorithm>


namespace dyloc {

class htag {

  const std::string & _tag;

 public:
  htag(const std::string & tag) : _tag(tag) { }

  size_t length() {
    return std::count(_tag.begin(), _tag.end(), '.');
  }

  std::string head(int prefix_segments) {
    size_t prefix_len = 0;
    for (int seg = 0;
         prefix_len < _tag.length() && seg < prefix_segments; ++seg) {
      prefix_len = _tag.find('.', prefix_len + 1);
    }
    return _tag.substr(0, prefix_len);
  }

  std::string tail(int suffix_segments) {
    auto   suffix     = _tag;
    size_t suffix_pos = suffix.length() - 1;
    for (int seg = 0;
         suffix_pos > 0 && seg < suffix_segments; ++seg) {
      suffix_pos = _tag.rfind('.', suffix_pos - 1);
    }
    return _tag.substr(suffix_pos, _tag.length() - suffix_pos);
  }

  std::string sub(int seg_first, int seg_last) {
    auto pre = head(seg_first);
    return htag(pre).tail(seg_last - seg_first);
  }

  template <class Iterator, class Sentinel>
  static std::string ancestor(
    const Iterator & first,
    const Sentinel & last) {
    const auto num_domains = std::distance(first, last);
    if (num_domains == 0) {
      DYLOC_THROW(
        dyloc::exception::invalid_argument,
        "cannot resolve ancestor of empty domain set");
    } else if (num_domains == 1) {
      dyloc::htag ancestor_tag(*first);
      return ancestor_tag.head(ancestor_tag.length() - 1);
    }
    auto common_prefix = dyloc::longest_common_prefix(first, last);
    if (common_prefix.back() == '.') {
      common_prefix.resize(common_prefix.size() - 1);
    }
    return common_prefix;
  }

};

} // namespace dyloc

#endif // DYLOC__INTERNAL__DOMAIN_TAG_H__INCLUDED
