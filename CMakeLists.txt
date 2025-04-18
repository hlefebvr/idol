cmake_minimum_required(VERSION 3.22)
project(idol VERSION 0.7.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${idol_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${idol_SOURCE_DIR}/cmake/coin-or")

add_subdirectory(lib)

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

add_custom_target(doc COMMAND ./build_docs.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts)
