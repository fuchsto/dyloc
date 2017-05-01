
#include <dylocxx/init.h>
#include <dylocxx/runtime.h>
#include <dylocxx/domain_graph.h>

#include <dylocxx/internal/logging.h>
#include <dylocxx/internal/assert.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

static runtime rt;

static bool _is_initialized         = false;
static bool _dyloc_initialized_dart = false;

void init(int * argc, char *** argv) {
  DYLOC_LOG_DEBUG("dylocxx.init", "()");
  if (!dyloc::is_initialized()) {
    if (!dart_initialized()) {
      DYLOC_ASSERT_RETURNS(
        dart_init(argc, argv),
        DART_OK);
      _dyloc_initialized_dart = true;
    }
    rt.initialize();
    _is_initialized = true;
  }
  DYLOC_LOG_DEBUG("dylocxx.init", ">");
}

void finalize() {
  DYLOC_LOG_DEBUG("dylocxx.finalize", "()");
  if (dyloc::is_initialized()) {
    if (_dyloc_initialized_dart && dart_initialized()) {
      DYLOC_ASSERT_RETURNS(
        dart_exit(),
        DART_OK);
    }
    rt.finalize();
    _is_initialized         = false;
    _dyloc_initialized_dart = false;
  }
  DYLOC_LOG_DEBUG("dylocxx.finalize", ">");
}

bool is_initialized() {
  return _is_initialized;
}

const dyloc_unit_locality_t & query_unit_locality(
  dart_global_unit_t u) {
  return rt.unit_locality(u);
}

const dyloc_unit_locality_t & query_unit_locality(
  dart_team_t t,
  dart_team_unit_t u) {
  return rt.unit_locality(t, u);
}

domain_graph & query_locality_graph(
  dart_team_t t = DART_TEAM_ALL) {
  return rt.locality_graph(t);
}

} // namespace dyloc

