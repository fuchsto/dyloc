project(project_libdyloc CXX)

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
set(ENABLE_ASSERTIONS ${ENABLE_ASSERTIONS}
    PARENT_SCOPE)

# ---------------------------------------------------------------------------
# Dependencies

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallHelpers.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/DART.cmake)
include(${CMAKE_SOURCE_DIR}/CMakeExt/hwloc.cmake)

set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_MULTITHREADED  OFF)
set(Boost_USE_STATIC_RUNTIME  ON)

find_package(Boost 1.53.0 REQUIRED)

message(STATUS "Boost include directories: "
        ${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries: "
        ${Boost_LIBRARIES})


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
  ${Boost_INCLUDE_DIRS}
  ${ADDITIONAL_INCLUDES}
)

foreach (dart_variant ${DART_IMPLEMENTATIONS_LIST})

  set(DYLOC_LIBRARY        "dyloc-${dart_variant}")
  set(DYLOCXX_LIBRARY      "dylocxx-${dart_variant}")
  set(DYLOC_COMMON_LIBRARY "dyloc-common-${dart_variant}")

  add_library(
    ${DYLOC_LIBRARY}
    ${DYLOC_HEADERS}
    ${DYLOC_SOURCES}
  )

  target_link_libraries(
    ${DYLOC_LIBRARY}
    ${ADDITIONAL_LIBRARIES}
    ${DART_LIBRARIES}
    ${DYLOCXX_LIBRARY}
    ${DYLOC_COMMON_LIBRARY}
  )

  # Compile flags
  set_target_properties(
    ${DYLOC_LIBRARY} PROPERTIES
    COMPILE_FLAGS ${ADDITIONAL_COMPILE_FLAGS}
  )

  install(
    TARGETS ${DYLOC_LIBRARY}
    DESTINATION lib
    EXPORT "${DYLOC_LIBRARY}-targets")

  # Exports
  install(
    EXPORT "${DYLOC_LIBRARY}-targets"
    DESTINATION share/cmake)

  if(${CMAKE_VERSION} VERSION_GREATER 3.0.0 )
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
      "dyloc-config.cmake.in"
      "${DYLOC_LIBRARY}-config.cmake"
      INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake"
      PATH_VARS CMAKE_INSTALL_PREFIX)

    # Install custom config
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DYLOC_LIBRARY}-config.cmake"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake")
  endif()

endforeach() # dart_variant
