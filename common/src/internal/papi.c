
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
         DYLOC_LOG_ERROR("dyloc::common: PAPI_EINVAL: "
                        "papi.h is different from the version used to "
                        "compile the PAPI library.");
         break;
    case PAPI_ENOMEM:
         DYLOC_LOG_ERROR("dyloc::common: PAPI_ENOMEM: "
                        "insufficient memory to complete the operation.");
         break;
    case PAPI_ECMP:
         DYLOC_LOG_ERROR("dyloc::common: PAPI_ENOMEM: "
                        "this component does not support the underlying "
                        "hardware.");
         break;
    case PAPI_ESYS:
         DYLOC_LOG_ERROR("dyloc_domain_locality: PAPI_ESYS: "
                        "a system or C library call failed inside PAPI, see "
                        "the errno variable");
         DYLOC_LOG_ERROR("dyloc::common: PAPI_ESYS: errno: %d", errno);
         break;
    default:
         DYLOC_LOG_ERROR("dyloc::common: PAPI: unknown error: %d",
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
  DYLOC_LOG_DEBUG("dyloc::common: PAPI: init");

  papi_ret = PAPI_library_init(PAPI_VER_CURRENT);

  if (papi_ret != PAPI_VER_CURRENT && papi_ret > 0) {
    DYLOC_LOG_ERROR("dyloc::common: PAPI: version mismatch");
    return DYLOC_ERR_OTHER;
  } else if (papi_ret < 0) {
    DYLOC_LOG_ERROR("dyloc::common: PAPI: init failed, returned %d",
                   papi_ret);
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
    DYLOC_LOG_ERROR("dyloc::common: PAPI: PAPI_thread_init failed");
    return DYLOC_ERR_OTHER;
  }
#endif
  *hwinfo = PAPI_get_hardware_info();
  if (*hwinfo == NULL) {
    DYLOC_LOG_ERROR("dyloc::common: PAPI: get hardware info failed");
    return DYLOC_ERR_OTHER;
  }

  DYLOC_LOG_DEBUG("dyloc::common: PAPI: initialized");
  return DYLOC_OK;
}

#endif /* DYLOC_ENABLE_PAPI */
