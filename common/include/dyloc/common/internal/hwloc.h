#ifndef DYLOC__COMMON__INTERNAL__HWLOC_H_INCLUDED
#define DYLOC__COMMON__INTERNAL__HWLOC_H_INCLUDED

#ifdef DYLOC_ENABLE_HWLOC

#include <dyloc/common/types.h>

#include <hwloc.h>
#include <hwloc/helper.h>


#ifdef __cplusplus
extern "C" {
#endif

#if HWLOC_API_VERSION < 0x00011100
#  define DYLOC__HWLOC_OBJ_NUMANODE HWLOC_OBJ_NODE
#else
#  define DYLOC__HWLOC_OBJ_NUMANODE HWLOC_OBJ_NUMANODE
#endif

static inline dyloc_locality_scope_t dyloc__hwloc_obj_type_to_scope(
  int hwloc_obj_type)
{
  switch (hwloc_obj_type) {
    case HWLOC_OBJ_MACHINE         : return DYLOC_LOCALITY_SCOPE_NODE;
    case DYLOC__HWLOC_OBJ_NUMANODE : return DYLOC_LOCALITY_SCOPE_NUMA;
    case HWLOC_OBJ_CORE            : return DYLOC_LOCALITY_SCOPE_CORE;
#if HWLOC_API_VERSION > 0x00011000
    case HWLOC_OBJ_PACKAGE         : return DYLOC_LOCALITY_SCOPE_PACKAGE;
#else
    case HWLOC_OBJ_SOCKET          : return DYLOC_LOCALITY_SCOPE_PACKAGE;
#endif
    case HWLOC_OBJ_PU              : return DYLOC_LOCALITY_SCOPE_CPU;
#if HWLOC_API_VERSION < 0x00020000
    case HWLOC_OBJ_CACHE           : return DYLOC_LOCALITY_SCOPE_CACHE;
#else
    case HWLOC_OBJ_L1CACHE         : /* fall-through */
    case HWLOC_OBJ_L2CACHE         : /* fall-through */
    case HWLOC_OBJ_L3CACHE         : return DYLOC_LOCALITY_SCOPE_CACHE;
#endif
    case HWLOC_OBJ_PCI_DEVICE      : return DYLOC_LOCALITY_SCOPE_MODULE;
    default : return DYLOC_LOCALITY_SCOPE_UNDEFINED;
  }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DYLOC_ENABLE_HWLOC */

#endif /* DYLOC__COMMON__INTERNAL__HWLOC_H_INCLUDED */
