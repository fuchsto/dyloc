project(project_libdyloc C)

# ---------------------------------------------------------------------------
# Dependencies

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)

set(DYLOC_TARGET_NAME libdyloc)


# ---------------------------------------------------------------------------
# Source Files

file(GLOB_RECURSE DYLOC_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DYLOC_HEADERS
     "include/*.h")


# ---------------------------------------------------------------------------

set(DYLOC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DYLOC_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DYLOC_INCLUDE_DIRS}
)

add_library(
  ${DYLOC_TARGET_NAME}
  ${DYLOC_HEADERS}
  ${DYLOC_SOURCES}
)

target_link_libraries(
  ${DYLOC_TARGET_NAME}
  ${ADDITIONAL_LIBRARIES}
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

