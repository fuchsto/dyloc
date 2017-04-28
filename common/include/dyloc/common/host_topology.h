#ifndef DYLOC__COMMON__HOST_TOPOLOGY_H__INCLUDED
#define DYLOC__COMMON__HOST_TOPOLOGY_H__INCLUDED

#include <dyloc/common/types.h>

#include <dash/dart/if/dart_types.h>


typedef struct
{
  char                       host[DART_LOCALITY_HOST_MAX_SIZE];
  char                       parent[DART_LOCALITY_HOST_MAX_SIZE];
  dyloc_locality_scope_pos_t scope_pos;
  int                        numa_ids[DART_LOCALITY_MAX_NUMA_ID];
  int                        num_numa;
  int                        level;
}
dyloc_host_domain_t;

typedef struct
{
  dart_global_unit_t       * units;
  int                        num_units;
}
dyloc_host_units_t;

typedef struct
{
  int                        num_nodes;
  int                        num_hosts;
  int                        num_host_levels;
  size_t                     num_units;
  char                    ** host_names;
  dyloc_host_units_t       * host_units;
  dyloc_host_domain_t      * host_domains;
}
dyloc_host_topology_t;

#endif /* DYLOC__COMMON__HOST_TOPOLOGY_H__INCLUDED */
