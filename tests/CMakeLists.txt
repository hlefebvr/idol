enable_testing()

add_executable(tests main.test.cpp test_utils.h main.test.cpp unit/modeling/Model_AddByRow.test.cpp unit/modeling/Model_AddByColumn.test.cpp unit/modeling/Model_UpdateVariables.test.cpp unit/modeling/Expr_OperatorsTypeDeduction.test.cpp unit/backends/Gurobi_CreateVariableWithBackend.test.cpp unit/backends/Gurobi_CreateVariableWithoutBackend.cpp unit/backends/Gurobi_CreateConstraintWithBackend.test.cpp unit/backends/Gurobi_CreateConstraintWithoutBackend.test.cpp unit/backends/Gurobi_UpdateVariableWithBackend.test.cpp unit/backends/Gurobi_UpdateConstraintWithBackend.test.cpp unit/backends/Gurobi_CreateVariableAndConstraintWithBackend.test.cpp)
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

add_subdirectory(unit)