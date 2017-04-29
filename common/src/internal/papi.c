
#include <dyloc/common/types.h>
#include <dash/dart/if/dart_types.h>

#ifdef DYLOC_ENABLE_PAPI
#include <dyloc/common/internal/papi.h>

#include <papi.h>

#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


void dyloc__papi_handle_error(
  int papi_ret)
{
  /*
   * PAPI_EINVAL   papi.h is different from the version used to compile the
   *               PAPI library.
   * PAPI_ENOMEM   Insufficient memory to complete the operation.
   * PAPI_ECMP     This component does not support the underlying hardware.
   * PAPI_ESYS     A system or C library call failed inside PAPI, see the
   *               errno variable.
   */
  switch (papi_ret) {
    case PAPI_EINVAL:
         fprintf(stderr, "dyloc::common: PAPI_EINVAL: "
                         "papi.h is different from the version used to "
                         "compile the PAPI library.");
         break;
    case PAPI_ENOMEM:
         fprintf(stderr, "dyloc::common: PAPI_ENOMEM: "
                         "insufficient memory to complete the operation.");
         break;
    case PAPI_ECMP:
         fprintf(stderr, "dyloc::common: PAPI_ENOMEM: "
                         "this component does not support the underlying "
                         "hardware.");
         break;
    case PAPI_ESYS:
         fprintf(stderr, "dyloc_domain_locality: PAPI_ESYS: "
                         "a system or C library call failed inside PAPI, see "
                         "the errno variable");
         fprintf(stderr, "dyloc::common: PAPI_ESYS: errno: %d", errno);
         break;
    default:
         fprintf(stderr, "dyloc::common: PAPI: unknown error: %d",
                         papi_ret);
         break;
  }
}

dyloc_ret_t dyloc__papi_init(
  const PAPI_hw_info_t ** hwinfo)
{
  int papi_ret;

  if (PAPI_is_initialized()) {
    *hwinfo = PAPI_get_hardware_info();
    return DYLOC_OK;
  }
  papi_ret = PAPI_library_init(PAPI_VER_CURRENT);

  if (papi_ret != PAPI_VER_CURRENT && papi_ret > 0) {
    return DYLOC_ERR_OTHER;
  } else if (papi_ret < 0) {
    dyloc__papi_handle_error(papi_ret);
    return DYLOC_ERR_OTHER;
  } else {
    papi_ret = PAPI_is_initialized();
    if (papi_ret != PAPI_LOW_LEVEL_INITED) {
      dyloc__papi_handle_error(papi_ret);
      return DYLOC_ERR_OTHER;
    }
  }

#if 0
  papi_ret = PAPI_thread_init(pthread_self);
  if (papi_ret != PAPI_OK) {
    fprintf(stderr, "dyloc::common: PAPI: PAPI_thread_init failed");
    return DYLOC_ERR_OTHER;
  }
#endif
  *hwinfo = PAPI_get_hardware_info();
  if (*hwinfo == NULL) {
    return DYLOC_ERR_OTHER;
  }
  return DYLOC_OK;
}

#endif /* DYLOC_ENABLE_PAPI */
