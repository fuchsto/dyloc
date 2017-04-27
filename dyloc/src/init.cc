
#include <dyloc/init.h>

#include <dyloc/common/types.h>

#include <dylocxx/init.h>


dyloc_ret_t dyloc_init(int * argc, char *** argv) {
  dyloc::init(argc, argv);
  return DYLOC_OK;
}

dyloc_ret_t dyloc_finalize() {
  dyloc::finalize();
  return DYLOC_OK;
}

int dyloc_is_initialized() {
  return dyloc::is_initialized() ? 1 : 0;
}
