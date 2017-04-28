#ifndef DYLOC__INTERNAL__ASSERT_H__INCLUDED
#define DYLOC__INTERNAL__ASSERT_H__INCLUDED

#include <stdlib.h>

#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_initialization.h>

#include <dylocxx/internal/logging.h>

#include <dyloc/common/internal/macro.h>


#ifdef DYLOC_ENABLE_ASSERTIONS

#define DYLOC_ASSERT(expr) do { \
  if (dyloc__unlikely(!(expr))) { \
    DYLOC_LOG_ERROR("Assertion failed: %s", dyloc__tostr(expr)); \
    dart_abort(DYLOC_EXIT_ASSERT); \
  } \
} while(0)

#define DYLOC_ASSERT_MSG(expr, msg) do { \
  if (dyloc__unlikely(!(expr))) { \
    DYLOC_LOG_ERROR("Assertion failed: %s: %s", dyloc__tostr(expr), (msg)); \
    dart_abort(DYLOC_EXIT_ASSERT); \
  } \
} while(0)

#define DYLOC_ASSERT_RETURNS(expr, exp_value) do { \
  if (dyloc__unlikely((expr) != (exp_value))) { \
    DYLOC_LOG_ERROR("Assertion failed: %s -- Expected return value %d", \
                    dyloc__tostr(expr), (exp_value)); \
    dart_abort(DYLOC_EXIT_ASSERT); \
  } \
} while(0)

#else /* DYLOC_ENABLE_ASSERTIONS */

#define DYLOC_ASSERT(...) do { } while (0)
#define DYLOC_ASSERT_MSG(...) do { } while (0)
#define DYLOC_ASSERT_RETURNS(expr, exp_value) do { \
          (expr); \
          dyloc__unused(exp_value); \
        } while(0)

#endif /* DYLOC_ENABLE_ASSERTIONS */

#endif /* DYLOC__INTERNAL__ASSERT_H__INCLUDED */
