
set (CC_WARN_FLAG  "${CC_WARN_FLAG} -Wall -Wextra -Wpedantic")
set (CXX_WARN_FLAG "${CXX_WARN_FLAG} -Wall -Wextra -Wpedantic")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  set (CXX_GDB_FLAG "-g"
       CACHE STRING "C++ compiler GDB debug symbols flag")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set (CXX_GDB_FLAG "-ggdb3 -rdynamic"
       CACHE STRING "C++ compiler GDB debug symbols flag")
endif()

# Set C++ compiler flags:
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  # using Clang
  set (CXX_STD_FLAG "--std=c++14"
       CACHE STRING "C++ compiler std flag")
  set (CXX_GDB_FLAG "-g"
       CACHE STRING "C++ compiler (clang++) debug symbols flag")
  set (CXX_OMP_FLAG ${OpenMP_CXX_FLAGS})
  set (CC_OMP_FLAG  ${OpenMP_CC_FLAGS})

  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.8.0")
    message(FATAL_ERROR "Insufficient Clang version (< 3.8.0)")
  endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  # using GCC
  set (CXX_STD_FLAG "--std=c++14"
       CACHE STRING "C++ compiler std flag")
  set (CXX_GDB_FLAG "-ggdb3 -rdynamic"
       CACHE STRING "C++ compiler GDB debug symbols flag")
  set (CXX_OMP_FLAG ${OpenMP_CXX_FLAGS})
  set (CC_OMP_FLAG  ${OpenMP_CC_FLAGS})
  if(ENABLE_LT_OPTIMIZATION)
    set (CXX_LTO_FLAG "-flto -fwhole-program -fno-use-linker-plugin")
  endif()

  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.1")
    message(FATAL_ERROR "Insufficient GCC version (< 4.8.1)")
  endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
  # using Intel C++
  set (CXX_STD_FLAG "-std=c++14"
       CACHE STRING "C++ compiler std flag")

  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0.0")
    message(FATAL_ERROR "Insufficient Intel compiler version (< 15.0.0)")
  endif()
endif()


# Set C compiler flags:
if ("${CMAKE_C_COMPILER_ID}" MATCHES ".*Clang")
  # using Clang
  set (CC_STD_FLAG "--std=c99"
       CACHE STRING "C compiler std flag")
  set (CC_GDB_FLAG "-g"
       CACHE STRING "C compiler (clang) debug symbols flag")
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
  # using GCC
  set (CC_STD_FLAG "--std=c99"
       CACHE STRING "C compiler std flag")
  set (CC_GDB_FLAG "-ggdb3"
       CACHE STRING "C compiler GDB debug symbols flag")
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "Intel")
  # using Intel C++
  set (CC_STD_FLAG "-std=c99"
       CACHE STRING "C compiler std flag")
elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "Cray")
  # using Cray
  set (CC_STD_FLAG "-h c99"
       CACHE STRING "C compiler std flag")
endif()

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${CC_ENV_SETUP_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_ENV_SETUP_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${CC_ENV_SETUP_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_ENV_SETUP_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -Og")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_WARN_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_STD_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_GDB_FLAG}")

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_STD_FLAG}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_WARN_FLAG}")


set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -Og")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${CC_WARN_FLAG}")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${CC_STD_FLAG}")

set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} -Ofast")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${CC_WARN_FLAG}")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${CC_STD_FLAG}")


message(STATUS "CC  flags (Debug):   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CXX flags (Debug):   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CC  flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")

