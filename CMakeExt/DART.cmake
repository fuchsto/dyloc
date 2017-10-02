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

if (NOT DART_FOUND)

include(${CMAKE_SOURCE_DIR}/CMakeExt/MPI.cmake)

if (NOT DART_PREFIX AND NOT ${DART_BASE} STREQUAL "")
  set(DART_PREFIX ${DART_BASE})
endif()

if (NOT DART_PREFIX AND NOT $ENV{DART_BASE} STREQUAL "")
  set(DART_PREFIX $ENV{DART_BASE})
endif()

if (NOT DART_PREFIX AND NOT $ENV{DASH_BASE} STREQUAL "")
  set(DART_PREFIX $ENV{DASH_BASE})
endif()

find_path(
  DART_PREFIX
  NAMES include/dart.h
)

message(STATUS "Searching for DART library in DART_BASE: " ${DART_PREFIX})

find_library(
  DART_BASE_LIBRARY
  NAMES dart-base
  HINTS ${DART_PREFIX}/lib
)

find_library(
  DART_MPI_LIBRARY
  NAMES dart-mpi
  HINTS ${DART_PREFIX}/lib
)

if (NOT DART_INCLUDE_DIRS)
  find_path(
    DART_INCLUDE_DIRS
    NAMES dash/dart/if/dart.h
    HINTS ${DART_PREFIX}/include
  )
endif()

set  (DART_LIBRARIES "")
list (APPEND DART_LIBRARIES    ${DART_BASE_LIBRARY})
list (APPEND DART_LIBRARIES    ${DART_MPI_LIBRARY})
list (APPEND DART_LIBRARIES    ${MPI_C_LIBRARIES})
list (APPEND DART_LIBRARIES    ${MPI_CXX_LIBRARIES})
list (APPEND DART_LIBRARIES    dart-mpi)
list (APPEND DART_LIBRARIES    dart-base)

list (APPEND DART_INCLUDE_DIRS ${MPI_INCLUDE_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  DART
  DEFAULT_MSG
  DART_LIBRARIES
  DART_INCLUDE_DIRS
)

mark_as_advanced(
  DART_LIBRARIES
  DART_INCLUDE_DIRS
)

if (DART_FOUND)
  message(STATUS "DART includes:")
  foreach (_DART_INC ${DART_INCLUDE_DIRS})
    message(STATUS "    " ${_DART_INC})
  endforeach()

  message(STATUS "DART libraries:")
  foreach (_DART_LIB ${DART_LIBRARIES})
    message(STATUS "    " ${_DART_LIB})
  endforeach()
endif()

endif() # DART_FOUND
