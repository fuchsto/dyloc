# Try to find DART libraries and headers.
#
# Usage of this module:
#
#  find_package(DART)
#
# Variables defined by this module:
#
#  DART_FOUND              System has DART libraries and headers
#  DART_LIBRARIES          The DART library
#  DART_INCLUDE_DIRS       The location of DART headers

find_path(
  DART_PREFIX
  NAMES include/dart.h
)

if (NOT DART_PREFIX AND NOT $ENV{DART_BASE} STREQUAL "")
  set(DART_PREFIX $ENV{DART_BASE})
endif()

if (NOT DART_PREFIX AND NOT $ENV{DASH_BASE} STREQUAL "")
  set(DART_PREFIX $ENV{DASH_BASE})
endif()

message(STATUS "Searching for DART library in DART_BASE: " ${DART_PREFIX})

find_library(
  DART_LIBRARIES
  NAMES dart-mpi dart-base
  HINTS ${DART_PREFIX}/lib
)

find_path(
  DART_INCLUDE_DIRS
  NAMES dart.h
  HINTS ${DART_PREFIX}/include
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  DART DEFAULT_MSG
  DART_LIBRARIES
  DART_INCLUDE_DIRS
)

mark_as_advanced(
  DART_LIBRARIES
  DART_INCLUDE_DIRS
)

if (DART_FOUND)
  if (NOT $ENV{DART_LIB} STREQUAL "")
#   set(DART_LIBRARIES "$ENV{DART_LIB}")
  endif()
  message(STATUS "DART includes:  " ${DART_INCLUDE_DIRS})
  message(STATUS "DART libraries: " ${DART_LIBRARIES})
endif()

