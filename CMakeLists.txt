cmake_minimum_required(VERSION 3.23)
project(optimize)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if (${USE_GUROBI})
    find_package(GUROBI REQUIRED)
endif()

if (${USE_LPSOLVE})
    find_package(LPSOLVE REQUIRED)
endif()

add_subdirectory(lib)

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

if (${BUILD_TESTS})
    add_subdirectory(tests)
elseif(${TEST_COVERAGE})
    message(FATAL "Must have BUILD_TESTS=YES in order to build code coverage")
endif()
