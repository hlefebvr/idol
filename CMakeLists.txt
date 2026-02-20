cmake_minimum_required(VERSION 3.22)
project(idol VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${idol_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${idol_SOURCE_DIR}/cmake/coin-or")

include(GNUInstallDirs)

add_subdirectory(lib)
add_subdirectory(bin)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build the example targets")
set(BUILD_TESTS OFF CACHE BOOL "Build the test targets")
set(TEST_COVERAGE OFF CACHE BOOL "Build in test coverage mode (must have BUILD_TESTS=ON)")

if (${BUILD_DEV})
    add_subdirectory(dev)
endif()

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

if (${BUILD_TESTS})
    add_subdirectory(tests)
elseif(${TEST_COVERAGE})
    message(FATAL "Must have BUILD_TESTS=YES in order to build code coverage")
endif()

set(BUILD_DEV OFF CACHE BOOL "Build the development target (advanced users only)")

add_custom_target(doc COMMAND ./build_docs.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts)
