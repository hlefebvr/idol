enable_testing()

add_executable(tests test_utils.h modeling/Coefficient.test.cpp modeling/Expr.test.cpp modeling/Var.test.cpp modeling/Ctr.test.cpp modeling/Model.test.cpp modeling/Param.test.cpp solvers/LP.test.cpp solvers/MILP.test.cpp algorithms/BranchAndBound.test.cpp algorithms/ColumnGeneration.test.cpp cartesian_product.h algorithms/GeneralizedAssignementProblem.cpp)
target_link_libraries(tests PRIVATE optimize)

if (${WITH_TEST_COVERAGE})
    target_compile_options(optimize PRIVATE -fprofile-arcs -ftest-coverage -g -O0 -fno-inline-small-functions -fkeep-inline-functions -fkeep-static-functions --coverage)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CPP_COMPILER gcc)
    target_link_libraries(optimize PUBLIC gcov)
endif()

add_test(tests tests)

find_package(Catch2 QUIET)

if (NOT ${Catch2_FOUND})

    Include(FetchContent)

    set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.1.0 # or a later release
            GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(Catch2)

endif()

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
