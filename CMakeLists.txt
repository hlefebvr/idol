cmake_minimum_required(VERSION 3.23)
project(idol VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(lib)
add_subdirectory(dev)

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

if (${BUILD_TESTS})
    add_subdirectory(tests)
elseif(${TEST_COVERAGE})
    message(FATAL "Must have BUILD_TESTS=YES in order to build code coverage")
endif()

add_custom_target(doc COMMAND sphinx-build -M html . _build WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/documentation)

