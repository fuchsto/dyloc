#ifndef DYLOC__INTERNAL__LOGGING_H__INCLUDED
#define DYLOC__INTERNAL__LOGGING_H__INCLUDED

#include <dyloc/common/internal/macro.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

#include <sys/types.h>
#include <unistd.h>

#ifdef DYLOC_LOG_OUTPUT_STDOUT
#  define DYLOC_LOG_OUTPUT_TARGET std::cout
#else
#  define DYLOC_LOG_OUTPUT_TARGET std::clog
#endif

//
// Always log error and warning messages:
//
#define DYLOC_LOG_ERROR(...) \
  dyloc::internal::logging::LogWrapper(\
    "ERROR", __FILE__, __LINE__, __VA_ARGS__)

#define DYLOC_LOG_ERROR_VAR(context, var) \
  dyloc::internal::logging::LogVarWrapper(\
    "ERROR", __FILE__, __LINE__, context, #var, (var))

#define DYLOC_LOG_WARN(...) \
  dyloc::internal::logging::LogWrapper(\
    "WARN ", __FILE__, __LINE__, __VA_ARGS__)

#define DYLOC_LOG_WARN_VAR(context, var) \
  dyloc::internal::logging::LogVarWrapper(\
    "WARN ", __FILE__, __LINE__, context, #var, (var))

//
// Debug and trace log messages:
//
#if defined(DYLOC_ENABLE_LOGGING)
#  define DYLOC_LOG_DEBUG(...) \
     dyloc::internal::logging::LogWrapper(\
       "DEBUG", __FILE__, __LINE__, __VA_ARGS__)

#  define DYLOC_LOG_DEBUG_VAR(context, var) \
     dyloc::internal::logging::LogVarWrapper(\
       "DEBUG", __FILE__, __LINE__, context, #var, (var))

#  if defined(DYLOC_ENABLE_TRACE_LOGGING)

#    define DYLOC_LOG_TRACE(...) \
       dyloc::internal::logging::LogWrapper(\
         "TRACE", __FILE__, __LINE__, __VA_ARGS__)

#    define DYLOC_LOG_TRACE_VAR(context, var) \
       dyloc::internal::logging::LogVarWrapper(\
         "TRACE", __FILE__, __LINE__, context, #var, (var))

#  else  // DYLOC_ENABLE_TRACE_LOGGING
#      define DYLOC_LOG_TRACE(...) do {  } while(0)
#      define DYLOC_LOG_TRACE_VAR(context, var) do { \
                dyloc__unused(var); \
              } while(0)

#  endif // DYLOC_ENABLE_TRACE_LOGGING
#else  // DYLOC_ENABLE_LOGGING

#  define DYLOC_LOG_DEBUG(...) do {  } while(0)
#  define DYLOC_LOG_DEBUG_VAR(context, var) do { \
            dyloc__unused(var); \
          } while(0)

#  define DYLOC_LOG_TRACE(...) do {  } while(0)
#  define DYLOC_LOG_TRACE_VAR(context, var) do { \
            dyloc__unused(var); \
          } while(0)

#endif // DYLOC_ENABLE_LOGGING

namespace dyloc {
namespace internal {
namespace logging {


// Terminator
void Log_Recursive(
  const char* level,
  const char* file,
  int line,
  const char* context_tag,
  std::ostringstream & msg);

inline void Log_Line(
  const char* level,
  const char* file,
  int line,
  const char* context_tag,
  const std::string & msg)
{
  pid_t pid = getpid();
  std::stringstream buf;

  buf << "[dyloc "
      << level
      << " ] [ "
      << std::right << std::setw(5) << pid
      << " ] "
      << std::left << std::setw(25)
      << file << ":"
      << std::left << std::setw(4)
      << line << " | "
      << std::left << std::setw(45)
      << context_tag << "| "
      << msg;

  buf << "\n";

  DYLOC_LOG_OUTPUT_TARGET << buf.str();
}

// "Recursive" variadic function
template<typename T, typename... Args>
inline void Log_Recursive(
  const char         * level,
  const char         * file,
  int                  line,
  const char         * context_tag,
  std::ostringstream & msg,
  T                    value,
  const Args & ...     args)
{
  msg << value << " ";
  Log_Recursive(level, file, line, context_tag, msg, args...);
}

// Log_Recursive wrapper that creates the ostringstream
template<typename... Args>
inline void LogWrapper(
  const char *     level,
  const char *     filepath,
  int              line,
  const char *     context_tag,
  const Args & ... args)
{
  std::ostringstream msg;
  // Extract file name from path
  const char * filebase = strrchr(filepath, '/');
  const char * filename = (filebase != 0) ? filebase + 1 : filepath;
  Log_Recursive(
    level,
    filename,
    line,
    context_tag,
    msg, args...);
}

// Log_Recursive wrapper that creates the ostringstream
template<typename T, typename... Args>
inline void LogVarWrapper(
  const char* level,
  const char* filepath,
  int         line,
  const char* context_tag,
  const char* var_name,
  const T &   var_value,
  const Args & ... args)
{
  std::ostringstream msg;
  msg << "|- " << var_name << ": " << var_value;
  // Extract file name from path
  const char * filebase = strrchr(filepath, '/');
  const char * filename = (filebase != 0) ? filebase + 1 : filepath;
  Log_Recursive(
    level,
    filename,
    line,
    context_tag,
    msg);
}

} // namespace logging
} // namespace internal
} // namespace dyloc

#endif // DYLOC__INTERNAL__LOGGING_H__INCLUDED
