#ifndef DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED
#define DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED

#include <dylocxx/host_topology.h>

#include <dylocxx/internal/iterator.h>
#include <dylocxx/internal/logging.h>

#include <dyloc/common/types.h>

#include <dash/dart/if/dart.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>


namespace dyloc {

const dyloc::host_topology & team_host_topology(dart_team_t);

class locality_domain {
  typedef locality_domain self_t;

 public:
  std::string                     domain_tag;
  std::string                     host;
  dyloc_locality_scope_t          scope     = DYLOC_LOCALITY_SCOPE_UNDEFINED;
  int                             level     = -1;
  int                             g_index   = -1;
  int                             r_index   = -1;
  dart_team_t                     team      = DART_TEAM_NULL;
  std::vector<dart_global_unit_t> unit_ids;
//std::vector<int>                core_ids;
  int                             num_cores = -1;

  template <class LocalityDomain>
  class subdomain_iterator
  : public dyloc::index_iterator_base<
             locality_domain::subdomain_iterator<LocalityDomain>,
             LocalityDomain >
  {
    typedef subdomain_iterator<LocalityDomain>             self_t;
    typedef dyloc::index_iterator_base<self_t>             base_t;
    typedef typename base_t::reference                  reference;
    typedef typename base_t::const_reference      const_reference;

    LocalityDomain & _parent;
   public:
    subdomain_iterator(LocalityDomain & parent, int pos)
    : base_t(pos)
    , _parent(parent)
    { }

    reference dereference(int pos) {
      return _parent[pos];
    }
  };

  typedef subdomain_iterator<self_t>             iterator;
  typedef subdomain_iterator<const self_t> const_iterator;

 public:
  locality_domain()                        = default;
  locality_domain(const locality_domain &) = default;
  locality_domain(locality_domain &&)      = default;

  locality_domain & operator=(const locality_domain &) = default;
  locality_domain & operator=(locality_domain &&)      = default;

  locality_domain(
    dart_team_t team);

  locality_domain(
    const locality_domain  & parent,
    dyloc_locality_scope_t   scope,
    int                      child_index);

  inline const_iterator begin() const noexcept {
    return const_iterator(*this, 0);
  }

  inline const_iterator end() const noexcept {
    return const_iterator(*this, -1);
  }

  locality_domain       & operator[](int child_rel_index)       noexcept;
  const locality_domain & operator[](int child_rel_index) const noexcept;

  inline const dyloc_hwinfo_t & hwinfo() const {
    DYLOC_LOG_DEBUG_VAR("dylocxx::locality_domain.hwinfo()", host);
    return dyloc::team_host_topology(team).module_hwinfo(host);
  }

};

std::ostream & operator<<(
  std::ostream          & os,
  const locality_domain & ld);

} // namespace dyloc

#endif // DYLOCXX__LOCALITY_DOMAIN_H__INCLUDED
