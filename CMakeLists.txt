cmake_minimum_required(VERSION 3.22)
project(idol VERSION 0.6.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${idol_SOURCE_DIR}/cmake")

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

add_custom_target(doc COMMAND sphinx-build -M html . _build WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)

if (${BUILD_PYTHON})
    add_subdirectory(python)
endif()