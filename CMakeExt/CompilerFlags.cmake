
set (CXX_STD_FLAG "--std=c++11"
     CACHE STRING "C++ compiler std flag")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  set (CXX_GDB_FLAG "-g"
       CACHE STRING "C++ compiler GDB debug symbols flag")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set (CXX_GDB_FLAG "-ggdb3 -rdynamic"
       CACHE STRING "C++ compiler GDB debug symbols flag")
endif()

message(STATUS "CC  flags (Debug):   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CXX flags (Debug):   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CC  flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")

