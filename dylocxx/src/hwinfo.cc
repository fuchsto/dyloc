
#include <dyloc/common/config.h>

#ifdef DYLOC__PLATFORM__LINUX
/* _GNU_SOURCE required for sched_getcpu() */
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  include <sched.h>
#endif

#include <dylocxx/hwinfo.h>
#include <dylocxx/exception.h>

#include <dyloc/common/types.h>
#include <dyloc/common/internal/hwloc.h>
#include <dyloc/common/internal/papi.h>

#include <dylocxx/internal/logging.h>


#ifdef DYLOC_ENABLE_LIKWID
#  include <likwid.h>
#endif

#ifdef DYLOC_ENABLE_HWLOC
#  include <hwloc.h>
#  include <hwloc/helper.h>
#endif

#ifdef DYLOC_ENABLE_PAPI
#  include <papi.h>
#endif

#ifdef DYLOC_ENABLE_NUMA
#  include <utmpx.h>
#  include <numa.h>
#endif

#include <iostream>
#include <sstream>


namespace dyloc {

std::ostream & operator<<(
  std::ostream                 & os,
  const dyloc_locality_scope_t & scope) {
  std::ostringstream ss;
  switch (scope) {
    case DYLOC_LOCALITY_SCOPE_GLOBAL:  ss << "GLOBAL"; break;
    case DYLOC_LOCALITY_SCOPE_GROUP:   ss << "GROUP"; break;
    case DYLOC_LOCALITY_SCOPE_UNIT:    ss << "UNIT"; break;
    case DYLOC_LOCALITY_SCOPE_MODULE:  ss << "MODULE"; break;
    case DYLOC_LOCALITY_SCOPE_NETWORK: ss << "NETWORK"; break;
    case DYLOC_LOCALITY_SCOPE_NODE:    ss << "NODE"; break;
    case DYLOC_LOCALITY_SCOPE_PACKAGE: ss << "PACKAGE"; break;
    case DYLOC_LOCALITY_SCOPE_NUMA:    ss << "NUMA"; break;
    case DYLOC_LOCALITY_SCOPE_UNCORE:  ss << "UNCORE"; break;
    case DYLOC_LOCALITY_SCOPE_CORE:    ss << "CORE"; break;
    case DYLOC_LOCALITY_SCOPE_CPU:     ss << "CPU"; break;
    case DYLOC_LOCALITY_SCOPE_CACHE:   ss << "CACHE"; break;
    default:                           ss << "UNDEFINED"; break;
  }
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream                     & os,
  const dyloc_locality_scope_pos_t & scope) {
  std::ostringstream ss;
  ss << scope.scope << ":" << scope.index;
  return operator<<(os, ss.str());
}

std::ostream & operator<<(
  std::ostream & os,
  const dyloc_hwinfo_t & hwinfo) {
  std::ostringstream ss;
  ss << "dyloc_hwinfo_t { "
     << "host:"  << hwinfo.host    << " "
     << "cpu:"   << hwinfo.cpu_id  << " "
     << "core:"  << hwinfo.core_id << " "
     << "numa:"  << hwinfo.numa_id << " "
     << "scope:" << "( ";
  for (int s = 0; s < hwinfo.num_scopes; ++s) {
    ss << hwinfo.scopes[s] << " ";
  }
  ss << ") }";
  return operator<<(os, ss.str());
}

hwinfo::hwinfo() {
  _hw.num_numa            = -1;
  _hw.numa_id             = -1;
  _hw.num_cores           = -1;
  _hw.core_id             = -1;
  _hw.cpu_id              = -1;
  _hw.min_cpu_mhz         = -1;
  _hw.max_cpu_mhz         = -1;
  _hw.min_threads         = -1;
  _hw.max_threads         = -1;
  _hw.cache_ids[0]        = -1;
  _hw.cache_ids[1]        = -1;
  _hw.cache_ids[2]        = -1;
  _hw.cache_sizes[0]      = -1;
  _hw.cache_sizes[1]      = -1;
  _hw.cache_sizes[2]      = -1;
  _hw.cache_line_sizes[0] = -1;
  _hw.cache_line_sizes[1] = -1;
  _hw.cache_line_sizes[2] = -1;
  _hw.max_shmem_mbps      = -1;
  _hw.system_memory_bytes = -1;
  _hw.numa_memory_bytes   = -1;
  _hw.num_scopes          = -1;

  dyloc_locality_scope_pos_t undef_scope;
  undef_scope.scope = DYLOC_LOCALITY_SCOPE_UNDEFINED;
  undef_scope.index = -1;
  for (int s = 0; s < DYLOC_LOCALITY_MAX_DOMAIN_SCOPES; s++) {
    _hw.scopes[s] = undef_scope;
  }
}

void hwinfo::collect() {
  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "()");

  gethostname(_hw.host, DYLOC_LOCALITY_HOST_MAX_SIZE);
  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "hostname:", _hw.host);

#ifdef DYLOC_ENABLE_HWLOC
  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "using hwloc");

  hwloc_topology_t topology;
  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology,
#if HWLOC_API_VERSION < 0x00020000
                             HWLOC_TOPOLOGY_FLAG_IO_DEVICES
                           | HWLOC_TOPOLOGY_FLAG_IO_BRIDGES
  /*                       | HWLOC_TOPOLOGY_FLAG_WHOLE_IO  */
#else
                             HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM
#endif
                          );
  hwloc_topology_load(topology);

  /* hwloc can resolve the physical index (os_index) of the active unit,
   * not the logical index.
   * Queries in the topology hierarchy require the logical index, however.
   * As a workaround, units scan the topology for the logical index of the
   * CPU object that has a matching physical index.
   */

  /* Get PU of active thread: */
  hwloc_cpuset_t cpuset = hwloc_bitmap_alloc();
  int flags     = 0; // HWLOC_CPUBIND_PROCESS;
  int cpu_os_id = -1;
  int ret       = hwloc_get_last_cpu_location(topology, cpuset, flags);
  if (!ret) {
    cpu_os_id = hwloc_bitmap_first(cpuset);
  }
  hwloc_bitmap_free(cpuset);

  hwloc_obj_t cpu_obj;
  for (cpu_obj =
         hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, 0);
       cpu_obj;
       cpu_obj = cpu_obj->next_cousin) {
    if ((int)cpu_obj->os_index == cpu_os_id) {
      _hw.cpu_id = cpu_obj->logical_index;
      break;
    }
  }

  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "hwloc: CPU id",
                  "physical:", cpu_os_id,
                  "logical:",  _hw.cpu_id);

  /* PU (cpu_id) to CORE (core_id) object: */
  hwloc_obj_t core_obj;
  for (core_obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, _hw.cpu_id);
       core_obj;
       core_obj = core_obj->parent) {
    if (core_obj->type == HWLOC_OBJ_CORE) { break; }
  }
  if (core_obj) {
    _hw.core_id = core_obj->logical_index;

    DYLOC_LOG_TRACE("dylocxx::hwinfo.collect",
                    "hwloc: core logical index:", _hw.core_id);

    _hw.num_scopes  = 0;
    int cache_level = 0;
    /* Use CORE object to resolve caches: */
    hwloc_obj_t obj;
    for (obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, _hw.core_id);
         obj;
         obj = obj->parent) {

      if (obj->type == HWLOC_OBJ_MACHINE) { break; }
      _hw.scopes[_hw.num_scopes].scope =
        dyloc__hwloc_obj_type_to_scope(obj->type);
      _hw.scopes[_hw.num_scopes].index = obj->logical_index;
      DYLOC_LOG_TRACE("dylocxx::hwinfo.collect",
                      "hwloc: parent[", _hw.num_scopes, "](",
                      "scope:", _hw.scopes[_hw.num_scopes].scope,
                      "index:", _hw.scopes[_hw.num_scopes].index,
                      ")");
      _hw.num_scopes++;

#if HWLOC_API_VERSION < 0x00020000
      if (obj->type == HWLOC_OBJ_CACHE) {
        _hw.cache_sizes[cache_level]      = obj->attr->cache.size;
        _hw.cache_line_sizes[cache_level] = obj->attr->cache.linesize;
        _hw.cache_ids[cache_level]        = obj->logical_index;
        cache_level++;
      }
#else
      if (obj->type == HWLOC_OBJ_L1CACHE) {
        _hw.cache_sizes[0]              = obj->attr->cache.size;
        _hw.cache_line_sizes[0]         = obj->attr->cache.linesize;
        _hw.cache_ids[0]                = obj->logical_index;
        cache_level++;
      }
      else if (obj->type == HWLOC_OBJ_L2CACHE) {
        _hw.cache_sizes[1]              = obj->attr->cache.size;
        _hw.cache_line_sizes[1]         = obj->attr->cache.linesize;
        _hw.cache_ids[1]                = obj->logical_index;
        cache_level++;
      }
      else if (obj->type == HWLOC_OBJ_L3CACHE) {
        _hw.cache_sizes[2]              = obj->attr->cache.size;
        _hw.cache_line_sizes[2]         = obj->attr->cache.linesize;
        _hw.cache_ids[2]                = obj->logical_index;
        cache_level++;
      }
#endif
      else if (obj->type == HWLOC_OBJ_NODE) {
        _hw.numa_id = obj->logical_index;
      } else if (obj->type ==
#if HWLOC_API_VERSION > 0x00011000
                 HWLOC_OBJ_PACKAGE
#else
                 HWLOC_OBJ_SOCKET
#endif
                ) {
      }
    }
  }

  if (_hw.numa_id < 0) {
    hwloc_obj_t numa_obj;
    for (numa_obj =
           hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, _hw.cpu_id);
         numa_obj;
         numa_obj = numa_obj->parent) {
      if (numa_obj->type == HWLOC_OBJ_NODE) {
        _hw.numa_id = numa_obj->logical_index;
        break;
      }
    }
  }
  if (_hw.num_numa < 0) {
    int n_numa_nodes = hwloc_get_nbobjs_by_type(
                         topology, DYLOC__HWLOC_OBJ_NUMANODE);
    if (n_numa_nodes > 0) {
      _hw.num_numa = n_numa_nodes;
    }
  }
  if (_hw.num_cores < 0) {
    int n_cores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_CORE);
    if (n_cores > 0) {
      _hw.num_cores = n_cores;
    }
	}
  if (_hw.min_threads < 0 && _hw.max_threads < 0 &&
      _hw.num_cores > 0 && _hw.max_threads < 0) {
    int n_cpus     = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_PU);
    _hw.min_threads = 1;
    _hw.max_threads = n_cpus / _hw.num_cores;
  }

  if (_hw.system_memory_bytes < 0) {
    hwloc_obj_t obj;
    obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_MACHINE, 0);
    // hwloc-1:
//  _hw.system_memory_bytes = obj->memory.total_memory / MBYTES;
    // hwloc-2:
    _hw.system_memory_bytes = obj->memory_first_child->total_memory / MBYTES;
  }
  if (_hw.numa_memory_bytes < 0) {
    hwloc_obj_t obj;
    obj = hwloc_get_obj_by_type(topology, DYLOC__HWLOC_OBJ_NUMANODE, 0);
    if(obj != NULL) {
    // hwloc-1:
//    _hw.numa_memory_bytes = obj->memory.total_memory / MBYTES;
    // hwloc-2:
      _hw.numa_memory_bytes = obj->memory_first_child->total_memory / MBYTES;
    } else {
      /* No NUMA domain: */
      _hw.numa_memory_bytes = _hw.system_memory_bytes;
    }
  }

  hwloc_topology_destroy(topology);
  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "hwloc:",
                  "num_numa:",    _hw.num_numa,
                  "numa_id:",     _hw.numa_id,
                  "num_cores:",   _hw.num_cores,
                  "min.threads:", _hw.min_threads,
                  "max.threads:", _hw.max_threads,
                  "core_id:",     _hw.core_id,
                  "cpu_id:",      _hw.cpu_id);
#endif /* DYLOC_ENABLE_HWLOC */

#ifdef DYLOC_ENABLE_PAPI
  DYLOC_LOG_TRACE("dylocxx::hwinfo: using PAPI");

  const PAPI_hw_info_t * papi_hwinfo = NULL;
  if (dyloc__papi_init(&papi_hwinfo) == DYLOC_OK) {
    if (_hw.num_numa < 0) {
      _hw.num_numa    = papi_hwinfo->nnodes;
    }
    if (_hw.num_cores < 0) {
      int num_sockets      = papi_hwinfo->sockets;
      int cores_per_socket = papi_hwinfo->cores;
      _hw.num_cores        = num_sockets * cores_per_socket;
    }
    if (_hw.min_cpu_mhz < 0 || _hw.max_cpu_mhz < 0) {
      _hw.min_cpu_mhz = papi_hwinfo->cpu_min_mhz;
      _hw.max_cpu_mhz = papi_hwinfo->cpu_max_mhz;
    }
    DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", "PAPI:",
                    "num_numa:",  _hw.num_numa,
                    "num_cores:", _hw.num_cores);
  }
#endif /* DYLOC_ENABLE_PAPI */

#ifdef DYLOC__PLATFORM__LINUX
  if (_hw.cpu_id < 0) {
    _hw.cpu_id = sched_getcpu();
  }
#else
  DYLOC_THROW(
    dyloc::exception::runtime_config_error,
    "dylocxx::hwinfo: hwloc or PAPI required if not running on Linux");
#endif

#ifdef DYLOC__ARCH__IS_MIC
  /*
   * Hardware information for Intel MIC can be hard-coded as hardware
   * specs of MIC model variants are invariant:
   */
  DYLOC_LOG_TRACE("dylocxx::hwinfo: MIC architecture");

  if (_hw.num_numa    < 0) { _hw.num_numa    =  1; }
  if (_hw.num_cores   < 0) { _hw.num_cores   = 60; }
  if (_hw.min_cpu_mhz < 0 || _hw.max_cpu_mhz < 0) {
    _hw.min_cpu_mhz = 1100;
    _hw.max_cpu_mhz = 1100;
  }
  if (_hw.min_threads < 0 || _hw.max_threads < 0) {
    _hw.min_threads = 4;
    _hw.max_threads = 4;
  }
  if (_hw.numa_id < 0) {
    _hw.numa_id = 0;
  }
#endif

#ifdef DYLOC__PLATFORM__POSIX
  if (_hw.num_cores < 0) {
		/*
     * NOTE: includes hyperthreading
     */
    int posix_ret = sysconf(_SC_NPROCESSORS_ONLN);
    _hw.num_cores = (posix_ret > 0) ? posix_ret : _hw.num_cores;
		DYLOC_LOG_TRACE(
      "dylocxx::hwinfo.collect", "POSIX: _hw.num_cores:", _hw.num_cores);
  }

	if (_hw.system_memory_bytes < 0) {
	  long pages     = sysconf(_SC_AVPHYS_PAGES);
	  long page_size = sysconf(_SC_PAGE_SIZE);
    if (pages > 0 && page_size > 0) {
      _hw.system_memory_bytes = (int) ((pages * page_size) / MBYTES);
    }
  }
#endif

#ifdef DYLOC_ENABLE_NUMA
  DYLOC_LOG_TRACE("dylocxx::hwinfo: using numalib");
  if (_hw.num_numa < 0) {
    _hw.num_numa = numa_max_node() + 1;
  }
  if (_hw.numa_id < 0 && _hw.cpu_id >= 0) {
    _hw.numa_id = numa_node_of_cpu(_hw.cpu_id);
  }
#endif

  if (_hw.num_scopes < 1) {
    /* No domain hierarchy could be resolved.
     * Use flat topology, with all units assigned to domains in CORE scope: */
    _hw.num_scopes = 1;
    _hw.scopes[0].scope = DYLOC_LOCALITY_SCOPE_CORE;
    _hw.scopes[0].index = (_hw.core_id >= 0) ? _hw.core_id : _hw.cpu_id;
  }

  DYLOC_LOG_DEBUG("dylocxx::hwinfo.collect", "finished:",
                  "hostname:",    _hw.host,
                  "num_numa:",    _hw.num_numa,
                  "numa_id:",     _hw.numa_id,
                  "cpu_id:",      _hw.cpu_id,
                  "num_cores:",   _hw.num_cores,
                  "min_threads:", _hw.min_threads,
                  "max_threads:", _hw.max_threads);
  DYLOC_LOG_TRACE("dylocxx::hwinfo.collect", ">");
}

} // namespace dyloc

