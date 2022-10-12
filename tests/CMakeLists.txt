enable_testing()

add_executable(tests main.test.cpp test_utils.h modeling/0_Constant.test.cpp modeling/4_Expr.test.cpp modeling/1_Var.test.cpp modeling/2_Ctr.test.cpp modeling/5_Model.test.cpp modeling/3_Param.test.cpp algorithms/6_LP_Toy.test.cpp algorithms/7_MILP_Toy.test.cpp algorithms/8_BranchAndBound_Toy.test.cpp algorithms/10_ColumnGeneration_Toy.test.cpp cartesian_product.h algorithms/11_BranchAndPrice_GAP.cpp main.test.cpp algorithms/12_RowGeneration_Toy.test.cpp algorithms/9_BranchAndBound_KP.test.cpp)
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


add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/instances/ $<TARGET_FILE_DIR:tests>/instances/
)
