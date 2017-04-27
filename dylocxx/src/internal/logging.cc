
#include <dylocxx/internal/logging.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <unistd.h>

#ifdef __GNUG__
#  include <cxxabi.h>
#  include <cstdlib>
#  include <memory>
#endif


namespace dyloc {
namespace internal {
namespace logging {

void Log_Recursive(
  const char* level,
  const char* file,
  int line,
  const char* context_tag,
  std::ostringstream & msg)
{
  std::istringstream ss(msg.str());
  std::string item;
  while (std::getline(ss, item)) {
    Log_Line(level, file, line, context_tag, item);
  }
  (DYLOC_LOG_OUTPUT_TARGET).flush();
}

} // namespace logging
} // namespace internal
} // namespace dyloc
