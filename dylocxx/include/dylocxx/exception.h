#ifndef DYLOCXX__EXCEPTION_H__INCLUDED
#define DYLOCXX__EXCEPTION_H__INCLUDED

#include <dylocxx/internal/logging.h>

#include <dyloc/common/internal/macro.h>

#include <stdexcept>
#include <string>
#include <sstream>


#define DYLOC_THROW(excep_type, msg_stream) do { \
    ::std::ostringstream os; \
    os << msg_stream; \
    DYLOC_LOG_ERROR(dyloc__toxstr(excep_type), os.str()); \
    throw(excep_type(os.str())); \
  } while(0)

namespace dyloc {
namespace exception {

class invalid_argument : public ::std::invalid_argument {
public:
  invalid_argument(const ::std::string & message)
  : ::std::invalid_argument(message) {
  }
};

class runtime_config_error : public ::std::runtime_error {
public:
  runtime_config_error(const ::std::string & message)
  : ::std::runtime_error(message) {
  }
};

} // namespace exception
} // namespace dyloc


#endif // DYLOCXX__EXCEPTION_H__INCLUDED
