# cmake/CompilerFlags.cmake

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0074 NEW)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Default build type
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "No build type specified. Defaulting to ReleaseWithDebugInfo.")
        set(CMAKE_BUILD_TYPE "ReleaseWithDebugInfo" CACHE STRING "Choose build type: Debug, DebugAddreess, ReleaseWithDebugInfo, or Release" FORCE)
    endif()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug DebugAddreess ReleaseWithDebugInfo Release)

    set(FLAGS_DEBUG "-g -O0 -DDEBUG_MODE")
    set(FLAGS_DEBUG_ADDR "-g -O0 -DDEBUG_MODE -fsanitize=address")
    set(FLAGS_DEBUG_INFO "-g -O2 -DDEBUG_INFO")
    set(FLAGS_RELEASE "-O3 -DNO_DEBUG")

    # Set flags for different build types
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Using Debug build flags")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_DEBUG}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "DebugAddreess")
        message(STATUS "Using Debug build flags with Address Sanitizer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_DEBUG_ADDR}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "ReleaseWithDebugInfo")
        message(STATUS "Using ReleaseWithDebugInfo build flags")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_DEBUG_INFO}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Using Release build flags")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_RELEASE}")
    else()
        message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}. Defaulting to ReleaseWithDebugInfo.")
        set(CMAKE_BUILD_TYPE "ReleaseWithDebugInfo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_DEBUG_INFO}")
    endif()
endif()

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_OBJECT_PATH_MAX 4096)  # Optional: avoid too-long paths on some OS

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
