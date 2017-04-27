project(project_libdylocxx_test CXX)

set(DASH_PREFIX ${DASH_PREFIX}
    PARENT_SCOPE)

set(DART_IMPLEMENTATIONS_LIST ${DART_IMPLEMENTATIONS_LIST}
    PARENT_SCOPE)

set(ENABLE_HWLOC      ${ENABLE_HWLOC}
    PARENT_SCOPE)
set(ENABLE_LIBNUMA    ${ENABLE_LIBNUMA}
    PARENT_SCOPE)
set(ENABLE_PAPI       ${ENABLE_PAPI}
    PARENT_SCOPE)
set(ENABLE_LOGGING    ${ENABLE_LOGGING}
    PARENT_SCOPE)

# ---------------------------------------------------------------------------
# Dependencies

# find_package("DART-MPI" REQUIRED HINTS ${DART_PREFIX})

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/GoogleTest.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/MPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/hwloc.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/PAPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/NUMA.cmake)

set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_MULTITHREADED  OFF)
set(Boost_USE_STATIC_RUNTIME  ON)

find_package(Boost 1.58.0 REQUIRED
             COMPONENTS filesystem)

message(STATUS "Boost include directories: "
        ${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries: "
        ${Boost_LIBRARIES})

# ---------------------------------------------------------------------------
# Linker- and Compiler Flags

if (ENABLE_LOGGING)
  set (ADDITIONAL_COMPILE_FLAGS
       ${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_LOGGING)
  set (ADDITIONAL_COMPILE_FLAGS
       ${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_LOGGING)
endif()


set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} rt)

if (PAPI_FOUND AND ENABLE_PAPI)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_PAPI")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${PAPI_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${PAPI_LIBRARIES})
endif()
if (HWLOC_FOUND AND ENABLE_HWLOC)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_HWLOC")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${HWLOC_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${HWLOC_LIBRARIES})
  if (ENABLE_HWLOC_PCI)
    set (ADDITIONAL_COMPILE_FLAGS
         "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_HWLOC_PCI")
  endif()
endif()
if (NUMA_FOUND AND ENABLE_LIBNUMA)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_NUMA")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${NUMA_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${NUMA_LIBRARIES})
endif()

# ---------------------------------------------------------------------------
# Source Files

file(GLOB_RECURSE DYLOCXX_TEST_SOURCES
     "*.h" "*.cc")

set(DYLOCXX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---------------------------------------------------------------------------

foreach (dart_variant ${DART_IMPLEMENTATIONS_LIST})
  set(DYLOCXX_TEST    "dylocxx-test-${dart_variant}")
  set(DYLOCXX_LIBRARY "dylocxx-${dart_variant}")

  message(EMPH "DART implementation: ${dart_variant}")
  message(EMPH "DART libraries:      ${DART_LIBRARIES}")

  include_directories(
    ${GTEST_INCLUDES}
    ${CMAKE_SOURCE_DIR}/dylocxx/include
    ${CMAKE_SOURCE_DIR}/common/include
    ${DART_INCLUDE_DIRS}
    ${ADDITIONAL_INCLUDES}
  )
  add_executable(
    ${DYLOCXX_TEST}
    ${DYLOCXX_TEST_SOURCES}
  )
  target_link_libraries(
    ${DYLOCXX_TEST}
    GTest
    ${DYLOCXX_LIBRARY}
    ${DART_LIBRARIES}
    ${ADDITIONAL_LIBRARIES}
  )

  if (${dart_variant} STREQUAL "mpi")
    include_directories(
      ${MPI_INCLUDE_PATH})
    target_link_libraries(
      ${DYLOCXX_TEST}
      ${MPI_C_LIBRARIES}
      ${MPI_CXX_LIBRARIES})
  endif()

  set_target_properties(
    ${DYLOCXX_TEST} PROPERTIES
    COMPILE_FLAGS
    "${VARIANT_ADDITIONAL_COMPILE_FLAGS} -Wno-unused -Wno-sign-compare"
  )
  set_target_properties(
    ${DYLOCXX_TEST} PROPERTIES
    CXX_STANDARD 11
  )

  set_target_properties(
    ${DYLOCXX_TEST} PROPERTIES
    CXX_STANDARD_REQUIRED 11
  )

  DeployBinary(${DYLOCXX_TEST})
  install(
    TARGETS ${DYLOCXX_TEST}
    DESTINATION bin/dylocxx/test
  )

endforeach() # dart_variant
