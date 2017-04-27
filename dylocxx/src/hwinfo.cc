
#include <dyloc/hwinfo.h>


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


