project(project_libdyloccommon C)

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

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/hwloc.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/PAPI.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/NUMA.cmake)


# ---------------------------------------------------------------------------
# Source Files

file(GLOB_RECURSE DYLOC_COMMON_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DYLOC_COMMON_HEADERS
     "include/*.h")


# ---------------------------------------------------------------------------
# Linker- and Compiler Flags

set (ADDITIONAL_COMPILE_FLAGS
     "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC")

if (ENABLE_LOGGING)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_LOGGING")
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_LOGGING")
endif()


if (PAPI_FOUND AND ENABLE_PAPI)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_PAPI")
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_PAPI")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${PAPI_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${PAPI_LIBRARIES})
endif()
if (HWLOC_FOUND AND ENABLE_HWLOC)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_HWLOC")
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_HWLOC")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${HWLOC_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${HWLOC_LIBRARIES})
  if (ENABLE_HWLOC_PCI)
    set (ADDITIONAL_COMPILE_FLAGS
         "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_HWLOC_PCI")
    set (ADDITIONAL_COMPILE_FLAGS
         "${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_HWLOC_PCI")
  endif()
endif()
if (NUMA_FOUND AND ENABLE_LIBNUMA)
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_NUMA")
  set (ADDITIONAL_COMPILE_FLAGS
       "${ADDITIONAL_COMPILE_FLAGS} -DDART_ENABLE_NUMA")
  set (ADDITIONAL_INCLUDES ${ADDITIONAL_INCLUDES}
       ${NUMA_INCLUDE_DIRS})
  set (ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
       ${NUMA_LIBRARIES})
endif()


# ---------------------------------------------------------------------------

set(DYLOC_COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DYLOC_COMMON_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(DYLOC_COMMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/common/include)

include_directories(
  ${DYLOC_COMMON_INCLUDE_DIRS}
  ${DART_INCLUDE_DIRS}
  ${ADDITIONAL_INCLUDES}
)


foreach (dart_variant ${DART_IMPLEMENTATIONS_LIST})
  set(DYLOC_COMMON_LIBRARY "dyloc-common-${dart_variant}")

  add_library(
    ${DYLOC_COMMON_LIBRARY}
    ${DYLOC_COMMON_HEADERS}
    ${DYLOC_COMMON_SOURCES}
  )

  target_link_libraries(
    ${DYLOC_COMMON_LIBRARY}
    ${ADDITIONAL_LIBRARIES}
    ${DART_LIBRARIES}
  )

  set_target_properties(
    ${DYLOC_COMMON_LIBRARY} PROPERTIES
    COMPILE_FLAGS ${ADDITIONAL_COMPILE_FLAGS}
  )

  set_target_properties(
    ${DYLOC_COMMON_LIBRARY} PROPERTIES
    CXX_STANDARD 11
  )
  set_target_properties(
    ${DYLOC_COMMON_LIBRARY} PROPERTIES
    CXX_STANDARD_REQUIRED 11
  )

endforeach() # dart_variant

# ---------------------------------------------------------------------------
## Installation

# Library
install(TARGETS ${DYLOC_COMMON_LIBRARY}
        DESTINATION lib)

# Headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/dyloc
        DESTINATION include FILES_MATCHING PATTERN "*.h")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/dyloc/dyloc_common.h
        DESTINATION include)

