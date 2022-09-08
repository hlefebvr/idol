enable_testing()

set(TEST_FILES test_utils.h modeling/Coefficient.test.cpp)

if (${WITH_TEST_COVERAGE})
    # Get optimize sources
    get_target_property(OPTIMIZE_SOURCES optimize SOURCES)
    list(TRANSFORM OPTIMIZE_SOURCES PREPEND ../lib/)

    add_executable(tests  ${OPTIMIZE_SOURCES} ${TEST_FILES})
    target_include_directories(tests PRIVATE ../lib/include)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0 -fno-inline-small-functions -fkeep-inline-functions -fkeep-static-functions --coverage" )
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CPP_COMPILER gcc)
    target_link_libraries(tests PRIVATE gcov)
    #target_compile_definitions(tests PRIVATE COVERAGE=true)
else()
    add_executable(tests Types.test.cpp ${TEST_FILES})
    target_link_libraries(tests PRIVATE optimize)
endif()

add_test(tests tests)

Include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.1.0 # or a later release
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
