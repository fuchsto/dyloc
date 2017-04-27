#ifndef DYLOC__COMMON__INTERNAL__PAPI_H__INCLUDED
#define DYLOC__COMMON__INTERNAL__PAPI_H__INCLUDED

#ifdef DYLOC_ENABLE_PAPI

#include <dyloc/common/types.h>
#include <papi.h>


void dyloc__papi_handle_error(
  int papi_ret);

dyloc_ret_t dyloc__papi_init(
  const PAPI_hw_info_t ** hwinfo);

#endif /* DYLOC_ENABLE_PAPI */

#endif /* DYLOC__COMMON__INTERNAL__PAPI_H__INCLUDED */
