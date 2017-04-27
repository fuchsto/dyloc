project(project_libdyloc C)

set(ENABLE_HWLOC ${ENABLE_HWLOC}
    PARENT_SCOPE)
set(ENABLE_LIBNUMA ${ENABLE_LIBNUMA}
    PARENT_SCOPE)
set(ENABLE_PAPI ${ENABLE_PAPI}
    PARENT_SCOPE)

set(ENABLE_LOGGING ${ENABLE_LOGGING}
    PARENT_SCOPE)

# ---------------------------------------------------------------------------
# Dependencies

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)

set(DYLOC_TARGET_NAME dyloc)


# ---------------------------------------------------------------------------
# Source Files

file(GLOB_RECURSE DYLOC_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DYLOC_HEADERS
     "include/*.h")

# Configure compile flags
# ---------------------------------------------------------------------------

set (ADDITIONAL_COMPILE_FLAGS
     ${ADDITIONAL_COMPILE_FLAGS} -DDYLOC)

# Logging compile flags

if (ENABLE_LOGGING)
  set (ADDITIONAL_COMPILE_FLAGS
       ${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_LOGGING)
  set (ADDITIONAL_COMPILE_FLAGS
       ${ADDITIONAL_COMPILE_FLAGS} -DDYLOC_ENABLE_LOGGING)
endif()


message (STATUS "libdyloc additional compile flags:")
set(ADDITIONAL_COMPILE_FLAGS_STR "")
foreach (ADDITIONAL_FLAG ${ADDITIONAL_COMPILE_FLAGS})
  message (STATUS "    " ${ADDITIONAL_FLAG})
  set(ADDITIONAL_COMPILE_FLAGS_STR
      "${ADDITIONAL_COMPILE_FLAGS_STR} ${ADDITIONAL_FLAG}")
endforeach()
message (STATUS "libdyloc additional libraries:")
foreach (ADDITIONAL_LIB ${ADDITIONAL_LIBRARIES})
  message (STATUS "    " ${ADDITIONAL_LIB})
endforeach()

# ---------------------------------------------------------------------------

set(DYLOC_COMMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/common/include)
set(DYLOCXX_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/dylocxx/include)

set(DYLOC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DYLOC_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DYLOC_INCLUDE_DIRS}
  ${DYLOCXX_INCLUDE_DIRS}
  ${DYLOC_COMMON_INCLUDE_DIRS}
  ${DART_INCLUDE_DIRS}
)

add_library(
  ${DYLOC_TARGET_NAME}
  ${DYLOC_HEADERS}
  ${DYLOC_SOURCES}
)

target_link_libraries(
  ${DYLOC_TARGET_NAME}
  ${ADDITIONAL_LIBRARIES}
  ${DART_LIBRARIES}
  dylocxx
)

# Compile flags
set_target_properties(
  ${DYLOC_TARGET_NAME} PROPERTIES
  COMPILE_FLAGS ${ADDITIONAL_COMPILE_FLAGS}
)

install(
  TARGETS ${DYLOC_TARGET_NAME}
  DESTINATION lib)

