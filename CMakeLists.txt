project(project_dyloc)

cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeExt)


if(POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)
endif(POLICY CMP0054)
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 OLD)
endif(POLICY CMP0053)
if(POLICY CMP0007)
  cmake_policy(SET CMP0007 OLD)
endif(POLICY CMP0007)
if(POLICY CMP0003)
  cmake_policy(SET CMP0003 OLD)
endif(POLICY CMP0003)
if(POLICY CMP0060)
  cmake_policy(SET CMP0060 OLD)
endif(POLICY CMP0060)
if(POLICY CMP0004)
  cmake_policy(SET CMP0004 OLD)
endif(POLICY CMP0004)
if(POLICY CMP0016)
  cmake_policy(SET CMP0016 OLD)
endif(POLICY CMP0016)


## Build options
option(ENABLE_LOGGING
       "Specify whether logging should be enabled" off)
option(ENABLE_ASSERTIONS
       "Specify whether run-time assertions should be enabled" on)

option(BUILD_SHARED_LIBS
       "Specify whether libraries should be built as shared objects" off)

option(ENABLE_LIBNUMA
       "Specify whether libnuma features are enabled" on)
option(ENABLE_HWLOC
       "Specify whether hwloc features are enabled" on)
option(ENABLE_HWLOC_PCI
       "Specify whether hwloc PCI features are enabled" on)
option(ENABLE_PAPI
       "Specify whether PAPI features are enabled" on)

include(${CMAKE_SOURCE_DIR}/CMakeExt/MessageColor.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)

include(${CMAKE_SOURCE_DIR}/CMakeExt/hwloc.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/MPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/PAPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/NUMA.cmake)

# ------------------------------------------------------------------------
# Version

set(DYLOC_VERSION_MAJOR 0 CACHE STRING "dyloc major version number.")
set(DYLOC_VERSION_MINOR 3 CACHE STRING "dyloc minor version number.")
set(DYLOC_VERSION_PATCH 0 CACHE STRING "dyloc patch version number.")

mark_as_advanced(
  DYLOC_VERSION_MAJOR
  DYLOC_VERSION_MINOR
  DYLOC_VERSION_PATCH)

set(DYLOC_VERSION
    "${DYLOC_VERSION_MAJOR}.${DYLOC_VERSION_MINOR}.${DYLOC_VERSION_PATCH}"
    CACHE STRING INTERNAL FORCE)
set(DYLOC_VERSIONED_PROJECT_NAME
    "dyloc-${DYLOC_VERSION_MAJOR}.${DYLOC_VERSION_MINOR}.${DYLOC_VERSION_PATCH}"
    CACHE STRING INTERNAL FORCE)



set(CMAKE_RULE_MESSAGES    ON)
set(CMAKE_COLOR_MAKEFILE   ON)

## Install path
if (INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX "./install/")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

## Set compiler flags (depend on CMake options)
include(${CMAKE_SOURCE_DIR}/CMakeExt/CompilerFlags.cmake)

## Build results output directories:
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shared)


set(
  DART_IMPLEMENTATIONS
  "mpi" CACHE STRING
  "Comma-separated list of DASH runtime implementations to use.
   Default is 'mpi'"
)
string(
  REPLACE "," ";"
  DART_IMPLEMENTATIONS_LIST
  ${DART_IMPLEMENTATIONS}
)


# ------------------------------------------------------------------------
# Build Targets

add_subdirectory(common)
add_subdirectory(dylocxx)
add_subdirectory(dyloc)

# ------------------------------------------------------------------------
# Build Configuration Banner

message(EMPH "dyloc library version ${DYLOC_VERSION}")
message(EMPH "-----------------------------------------------------------")

message(EMPH "Install prefix:           (INSTALL_PREFIX)                 "
        ${CMAKE_INSTALL_PREFIX})
message(EMPH "Build type:               (CMAKE_BUILD_TYPE)               "
        ${CMAKE_BUILD_TYPE})
message(EMPH "DART implementation:      (DART_IMPLEMENTATIONS)           "
        ${DART_IMPLEMENTATIONS})

message(INFO "libnuma support:          (ENABLE_LIBNUMA)                 "
        ${ENABLE_LIBNUMA})
message(INFO "hwloc support:            (ENABLE_HWLOC)                   "
        ${ENABLE_HWLOC})
message(INFO "hwloc PCI support:        (ENABLE_HWLOC_PCI)               "
        ${ENABLE_HWLOC_PCI})
message(INFO "PAPI support:             (ENABLE_PAPI)                    "
        ${ENABLE_PAPI})

message(INFO "C   compiler id:          ${CMAKE_C_COMPILER_ID}")
message(INFO "C++ compiler id:          ${CMAKE_CXX_COMPILER_ID}")

if (ENABLE_PAPI)
  if (PAPI_FOUND)
    message(INFO "PAPI                      enabled")
  else()
    message(NOTE "PAPI                      not found")
  endif()
else()
  message(NOTE "PAPI                      disabled")
endif()

if (ENABLE_HWLOC)
  if (HWLOC_FOUND)
    message(INFO "hwloc                     enabled")
  else()
    message(NOTE "hwloc                     not found")
  endif()
else()
  message(NOTE "hwloc                     disabled")
endif()

if (ENABLE_LIBNUMA)
  if (NUMA_FOUND)
    message(INFO "libnuma                   enabled")
  else()
    message(NOTE "libnuma                   not found")
  endif()
else()
  message(NOTE "libnuma                   disabled")
endif()

