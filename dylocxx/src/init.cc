
#include <dylocxx/init.h>

#include <dash/dart/if/dart.h>


namespace dyloc {

static bool _is_initialized = false;

void init(int * argc, char *** argv) {
  if (!dyloc::is_initialized() &&
      dart_init(argc, argv) == DART_OK) {
    _is_initialized = true;
  }
}

void finalize() {
  if (dyloc::is_initialized() &&
      dart_exit() == DART_OK) {
    _is_initialized = false;
  }
}

bool is_initialized() {
  return _is_initialized;
}

} // namespace dyloc

