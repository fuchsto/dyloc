project(project_dylocxx CXX)

# ---------------------------------------------------------------------------
# Dependencies

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/hwloc.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/PAPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/NUMA.cmake)

set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost 1.58.0 REQUIRED
             COMPONENTS filesystem regex)

message(STATUS "Boost include directories: "
        ${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries: "
        ${Boost_LIBRARIES})


set(DYLOCXX_TARGET_NAME dnants)

# ---------------------------------------------------------------------------
# Source Files

file(GLOB_RECURSE DYLOCXX_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DYLOCXX_HEADERS
     "include/*.h")
file(GLOB_RECURSE DYLOCXX_RESOURCES
     "res/*.")


# ---------------------------------------------------------------------------
# Linker- and Compiler Flags

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

set(DYLOCXX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DYLOCXX_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DYLOCXX_INCLUDE_DIRS}
)

add_executable(
  ${DYLOCXX_TARGET_NAME}
  ${DYLOCXX_HEADERS}
  ${DYLOCXX_SOURCES}
)

target_link_libraries(
  ${DYLOCXX_TARGET_NAME}
  ${ADDITIONAL_LIBRARIES}
)

# Compile flags
set_target_properties(
  ${DYLOCXX_TARGET_NAME} PROPERTIES
  COMPILE_FLAGS ${ADDITIONAL_COMPILE_FLAGS}
)

set_target_properties(
  ${exampletarget} PROPERTIES
  CXX_STANDARD 11
)
set_target_properties(
  ${exampletarget} PROPERTIES
  CXX_STANDARD_REQUIRED 11
)

install(
  TARGETS ${DYLOCXX_TARGET_NAME}
  DESTINATION bin)

