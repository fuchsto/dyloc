
#include <dylocxx/init.h>
#include <dylocxx/internal/logging.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

static bool _is_initialized = false;

void init(int * argc, char *** argv) {
  DYLOC_LOG_DEBUG("dylocxx.init", "()");
  if (!dyloc::is_initialized() &&
      dart_init(argc, argv) == DART_OK) {
    _is_initialized = true;
  }
  DYLOC_LOG_DEBUG("dylocxx.init", ">");
}

void finalize() {
  DYLOC_LOG_DEBUG("dylocxx.finalize", "()");
  if (dyloc::is_initialized() &&
      dart_exit() == DART_OK) {
    _is_initialized = false;
  }
  DYLOC_LOG_DEBUG("dylocxx.finalize", ">");
}

bool is_initialized() {
  return _is_initialized;
}

} // namespace dyloc

