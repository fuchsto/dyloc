
#include <dylocxx/init.h>
#include <dylocxx/runtime.h>

#include <dylocxx/internal/logging.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

static runtime rt;

static bool _is_initialized = false;

void init(int * argc, char *** argv) {
  DYLOC_LOG_DEBUG("dylocxx.init", "()");
  if (!dyloc::is_initialized() && dart_init(argc, argv) == DART_OK) {
    rt.initialize_locality(DART_TEAM_ALL);
    _is_initialized = true;
  }
  DYLOC_LOG_DEBUG("dylocxx.init", ">");
}

void finalize() {
  DYLOC_LOG_DEBUG("dylocxx.finalize", "()");
  if (dyloc::is_initialized() && dart_exit() == DART_OK) {
    rt.finalize_locality(DART_TEAM_ALL);
    _is_initialized = false;
  }
  DYLOC_LOG_DEBUG("dylocxx.finalize", ">");
}

bool is_initialized() {
  return _is_initialized;
}

const dyloc_unit_locality_t & query_unit_locality(
        dart_global_unit_t u) {
  rt.unit_locality(u);
}

const dyloc_unit_locality_t & query_unit_locality(
        dart_team_t t,
        dart_team_unit_t u) {
  rt.unit_locality(t, u);
}

} // namespace dyloc

