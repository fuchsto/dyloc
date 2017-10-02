#ifndef DYLOC__COMMON__TYPES_H__INCLUDED
#define DYLOC__COMMON__TYPES_H__INCLUDED

#include <stdlib.h>
#include <stdint.h>

#include <dash/dart/if/dart.h>


typedef enum
{
  DYLOC_OK = 0,
  DYLOC_ERR_INVALID,
  DYLOC_ERR_OTHER
} dyloc_ret_t;

/**
 * Scopes of locality domains.
 *
 * Enum values are ordered by scope level in the locality hierarchy.
 * Consequently, the comparison \c (scope_a > scope_b) is valid
 * and evaluates to \c true if \c scope_a is a parent scope of
 * \c scope_b.
 *
 */
typedef enum
{
  DYLOC_LOCALITY_SCOPE_UNDEFINED  =   -1,
  /** Global locality scope, components may be heterogenous. */
  DYLOC_LOCALITY_SCOPE_GLOBAL     =    0,
  /** Group of domains in specific locality scope, used as parent scope of
   *  domains in a user-defined group at any locality level. */
  DYLOC_LOCALITY_SCOPE_GROUP      =    1,
  /** Interconnect topology scope, components may be heterogenous. */
  DYLOC_LOCALITY_SCOPE_NETWORK    =   50,
  /** Node-level locality scope, components may be heterogenous. */
  DYLOC_LOCALITY_SCOPE_NODE       =  100,
  /** Locality in a group of hetereogenous components in different NUMA
   *  domains. */
  DYLOC_LOCALITY_SCOPE_MODULE     =  200,
  /** Locality of homogenous components in different NUMA domains. */
  DYLOC_LOCALITY_SCOPE_NUMA       =  300,
  /** Locality of homogenous components in the same NUMA domain at
   *  process-level, i.e. of a unit-addressable, homogenous entity.
   *  A single unit corresponds to a DYLOC (e.g. MPI) process and can
   *  occupy multiple homogenous cores, e.g. for multithreading. */
  DYLOC_LOCALITY_SCOPE_UNIT       =  400,
  /** Locality at level of physical processor package. Cannot be
   *  referenced by DYLOC directly. */
  DYLOC_LOCALITY_SCOPE_PACKAGE    =  500,
  /** Locality at processor uncore (system agent) level. Intel only.
   *  Cannot be referenced by DYLOC directly. */
  DYLOC_LOCALITY_SCOPE_UNCORE     =  510,
  /** Locality at level of physical CPU cache. Cannot be referenced by
   *  DYLOC directly. */
  DYLOC_LOCALITY_SCOPE_CACHE      =  530,
  /** Locality at physical processing core level. Cannot be referenced
   *  by DYLOC directly. */
  DYLOC_LOCALITY_SCOPE_CORE       =  550,
  /** Locality at logical CPU level (SMT thread). Cannot be referenced
   *  by DYLOC directly. */
  DYLOC_LOCALITY_SCOPE_CPU        =  600
}
dyloc_locality_scope_t;

/** Maximum size of a host name string in \ref dyloc_hwinfo_t */
#define DYLOC_LOCALITY_HOST_MAX_SIZE       ((int)(30))

/** Maximum size of a domain tag string in \ref dyloc_hwinfo_t */
#define DYLOC_LOCALITY_DOMAIN_TAG_MAX_SIZE ((int)(32))

/** Maximum number of domain scopes in \ref dyloc_hwinfo_t */
#define DYLOC_LOCALITY_MAX_DOMAIN_SCOPES   ((int)(16))

/** Maximum size of a domain tag string in \ref dyloc_hwinfo_t */
#define DYLOC_LOCALITY_UNIT_MAX_CPUS       ((int)(64))

/** Maximum number of NUMA domains supported */
#define DYLOC_LOCALITY_MAX_NUMA_ID         ((int)(16))

/** Maximum number of cache levels supported in \ref dyloc_hwinfo_t */
#define DYLOC_LOCALITY_MAX_CACHE_LEVELS    ((int)( 5))

typedef struct {
    /** The domain's scope identifier. */
    dyloc_locality_scope_t          scope;
    /** The domain's relative index among its siblings in the scope. */
    int                             index;
}
dyloc_locality_scope_pos_t;

/**
 * Invariant hardware locality capabilities for a single locality domain.
 *
 * The values of these properties refer to physical traits of the system
 * architecture and are not adjusted for a logical context such as topology
 * graph structure or unit placements.
 *
 * Note that \c dyloc_locality_domain_t must have static size as it is
 * used for an all-to-all exchange of locality data across all units
 * using \c dyloc_allgather.
 *
 * \ingroup DylocTypes
 */
typedef struct
{
    /** Hostname of the domain's node or 0 if unspecified. */
    char  host[DYLOC_LOCALITY_HOST_MAX_SIZE];

    /** Total number of CPUs in the associated domain. */
    int   num_cores;

    int   num_numa;

    int   numa_id;

    /** The unit's affine core, unique identifier within a processing
     *  module. */
    int   core_id;
    /** The unit's affine processing unit (SMP), unique identifier
     *  within a processing module. */
    int   cpu_id;

    /** Minimum clock frequency of CPUs in the domain. */
    int   min_cpu_mhz;
    /** Maximum clock frequency of CPUs in the domain. */
    int   max_cpu_mhz;

    /** Cache sizes by cache level (L1, L2, L3). */
    int   cache_sizes[DYLOC_LOCALITY_MAX_CACHE_LEVELS];
    /** Cache line sizes by cache level (L1, L2, L3). */
    int   cache_line_sizes[DYLOC_LOCALITY_MAX_CACHE_LEVELS];
    /** IDs of cache modules by level (L1, L2, L3), unique within domain. */
    int   cache_ids[DYLOC_LOCALITY_MAX_CACHE_LEVELS];

    /** Minimum number of CPU threads per core. */
    int   min_threads;
    /** Maximum number of CPU threads per core. */
    int   max_threads;

    /** Maximum local shared memory bandwidth in MB/s. */
    int   max_shmem_mbps;

    /** Maximum allocatable memory per node in bytes */
    int   system_memory_bytes;

    /** Maximum memory per numa node in bytes. */
    int   numa_memory_bytes;

    /** Ancestor locality scopes in bottom-up hierarchical order. */
    dyloc_locality_scope_pos_t scopes[DYLOC_LOCALITY_MAX_DOMAIN_SCOPES];

    int   num_scopes;
}
dyloc_hwinfo_t;

typedef struct
{
    /** Hostname of the module's parent node */
    char  host[DYLOC_LOCALITY_HOST_MAX_SIZE];

    /** Hostname of the module, including the parent hostname prefix. */
    char  module[DYLOC_LOCALITY_HOST_MAX_SIZE];

    /** The module's parent scope and its relative position in the scope. */
    dyloc_locality_scope_pos_t pos;
}
dyloc_module_location_t;

/**
 * A domain is a group of processing entities such as cores in a specific
 * NUMA domain or a Intel MIC entity.
 *
 * The domain data structure itself realizes a tree that represents a
 * well-defined containment hierarchy.
 * In this, a domain may consist of heterogenous child domains.
 * Processing entities in domains below scope `MODULE` are homogenous.
 *
 * Domains can be organized in an arbitrary logical structure based on an
 * external graph.
 *
 */
struct dyloc_locality_domain_s
{
    /**
     * Hostname of the domain's node or 0 if unspecified.
     * Invariant property.
     */
    char host[DYLOC_LOCALITY_HOST_MAX_SIZE];

    /**
     * Node (machine) index of the domain or -1 if domain contains
     * multiple compute nodes.
     * Invariant property.
     */
    int                               node_id;

    /**
     * Locality scope of the domain.
     * Invariant property.
     */
    dyloc_locality_scope_t            scope;




    /**
     * Hierarchical domain identifier, represented as dot-separated list
     * of relative indices on every level in the locality hierarchy.
     */
    char domain_tag[DYLOC_LOCALITY_DOMAIN_TAG_MAX_SIZE];

    struct dyloc_locality_domain_s ** aliases;

    int                               num_aliases;

    /** Level in the domain locality hierarchy. */
    int                               level;

    /** The domain's global index within its scope. */
    int                               global_index;

    /** The domain's index within its parent domain. */
    int                               relative_index;

    /** Pointer to descriptor of parent domain or 0 if no parent domain
     *  is specified. */
    struct dyloc_locality_domain_s  * parent;

    /** Number of subordinate domains. */
    int                               arity;
    /** Array of subordinate domains of size \c num_domains or 0 if no
     *  subdomains are specified. */
    struct dyloc_locality_domain_s ** children;

    /** Whether sub-domains have identical hardware configuration. */
    int                               is_symmetric;

    /** Team associated with the domain. */
    dart_team_t                       team;
    /** Number of units in the domain. */
    int                               num_units;
    /** Global IDs of units in the domain. */
    dart_global_unit_t              * unit_ids;

    /* The number of compute nodes in the domain. */
    int                               num_nodes;

    /* Number of cores in the domain. Cores may be heterogeneous unless
     * `is_symmetric` is different from 0. */
    int                               num_cores;

    /* The minimum size of the physical or logical shared memory
     * accessible by all units in the domain.
     */
    int                               shared_mem_bytes;
};
struct dyloc_locality_domain_s;
typedef struct dyloc_locality_domain_s
    dyloc_locality_domain_t;

/**
 * Locality and topology information of a single unit.
 * Processing entities grouped in a single unit are homogenous.
 * Each unit is a member of one specific locality domain.
 *
 * Note that \c dyloc_unit_locality_t must have static size as it is
 * used for an all-to-all exchange of locality data across all units
 * using \c dyloc_allgather.
 *
 * \ingroup DylocTypes
 */
typedef struct {
    /** Unit ID relative to team. */
    dart_team_unit_t          unit;

    /** Team ID. */
    dart_team_t               team;

    /** Hardware specification of the unit's affinity. */
    dyloc_hwinfo_t            hwinfo;
}
dyloc_unit_locality_t;

#endif // DYLOC__COMMON__TYPES_H__INCLUDED
