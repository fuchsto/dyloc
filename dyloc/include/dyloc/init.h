#ifndef DYLOC__INIT_H__INCLUDED
#define DYLOC__INIT_H__INCLUDED

#include <dyloc/common/types.h>

#include <dylocxx/init.h>


#ifdef __cplusplus
extern "C" {
#endif


dyloc_ret_t dyloc_init();

dyloc_ret_t dyloc_finalize();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DYLOC__INIT_H__INCLUDED */
