# Configure test environment

enable_testing()

if (${WITH_TEST_COVERAGE})
    target_compile_options(idol PRIVATE -fprofile-arcs -ftest-coverage -g -O0 --coverage)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CPP_COMPILER g++)
    target_link_libraries(idol PUBLIC gcov)
endif()

find_package(Catch2 QUIET)

if (NOT ${Catch2_FOUND})

    Include(FetchContent)

    set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.3.2 # or a later release
            GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(Catch2)

endif()

# Define tests
add_subdirectory(external-solvers)