#ifndef DYLOCXX__ADAPTER__HWLOC_H__INCLUDED
#define DYLOCXX__ADAPTER__HWLOC_H__INCLUDED

#ifdef DYLOC_ENABLE_HWLOC

#include <dylocxx/topology.h>
#include <dylocxx/locality_domain.h>

#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_team_group.h>

#include <hwloc.h>
#include <hwloc/helper.h>


namespace dyloc {

hwloc_topology_t & operator<<(
  hwloc_topology_t      & hwloc_topo,
  const dyloc::topology & topo) {
  // TODO: initialize hwloc topology from domain topology
  dyloc__unused(topo);
  return hwloc_topo;
}

/**
 * Convert dyloc locality domain to hwloc topology object.
 */
hwloc_obj_t & operator<<(
  hwloc_obj_t                  & hwloc_obj,
  const dyloc::locality_domain & domain) {
  // TODO: initialize hwloc object from locality domain
  dyloc__unused(domain);
  return hwloc_obj;
}

} // namespace dyloc

#endif // DYLOC_ENABLE_HWLOC
#endif // DYLOCXX__ADAPTER__HWLOC_H__INCLUDED
