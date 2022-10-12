enable_testing()

add_executable(tests main.test.cpp test_utils.h modeling/Coefficient.test.cpp modeling/Expr.test.cpp modeling/Var.test.cpp modeling/Ctr.test.cpp modeling/Model.test.cpp modeling/Param.test.cpp algorithms/LP_Toy.test.cpp algorithms/MILP_Toy.test.cpp algorithms/BranchAndBound_Toy.test.cpp algorithms/ColumnGeneration_Toy.test.cpp cartesian_product.h algorithms/BranchAndPrice_GAP.cpp main.test.cpp instances/generalized-assignment-problem/AbstractInstanceGAP.h instances/generalized-assignment-problem/InstanceGAP_1.h instances/generalized-assignment-problem/InstanceGAP_2.h instances/generalized-assignment-problem/InstanceGAP_3.h instances/generalized-assignment-problem/InstanceGAP_0.h instances/generalized-assignment-problem/InstanceGAP_4.h instances/generalized-assignment-problem/InstanceGAP_5.h instances/generalized-assignment-problem/InstanceGAP_6.h algorithms/CutGeneration_Toy.test.cpp)
target_link_libraries(tests PRIVATE idol)
target_include_directories(tests PRIVATE ../lib/include/)

if (${WITH_TEST_COVERAGE})
    target_compile_options(idol PRIVATE -fprofile-arcs -ftest-coverage -g -O0 -fno-inline-small-functions -fkeep-inline-functions -fkeep-static-functions --coverage)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CPP_COMPILER gcc)
    target_link_libraries(idol PUBLIC gcov)
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

target_link_libraries(tests PRIVATE Catch2::Catch2)
