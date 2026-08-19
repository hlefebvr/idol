add_executable(test_bab_default_node_info_interface
        branch-and-bound/default-node-info-interface.cpp
)
target_link_libraries(test_bab_default_node_info_interface PRIVATE idol Catch2::Catch2WithMain)
add_test(NAME test_bab_default_node_info_interface COMMAND test_bab_default_node_info_interface)

add_executable(test_basic_modeling
        basic/modeling.test.cpp
        basic/default-node-info.test.cpp
)
target_link_libraries(test_basic_modeling PRIVATE idol Catch2::Catch2WithMain)
add_test(NAME test_basic_modeling COMMAND test_basic_modeling)
set_tests_properties(test_basic_modeling PROPERTIES LABELS "basic;modeling")

set(BASIC_MILP_SOLVERS GLPK HiGHS Gurobi)
if (USE_CPLEX)
    list(APPEND BASIC_MILP_SOLVERS Cplex)
endif ()
foreach (OPTIMIZER ${BASIC_MILP_SOLVERS})
    set(target_name "test_basic_solver_${OPTIMIZER}")
    add_executable(${target_name} basic/solver.test.cpp)
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    target_link_libraries(${target_name} PRIVATE idol Catch2::Catch2WithMain)

    if (OPTIMIZER STREQUAL "GLPK" OR OPTIMIZER STREQUAL "HiGHS")
        add_test(NAME ${target_name} COMMAND ${target_name})
        set_tests_properties(${target_name} PROPERTIES LABELS "basic;solver;free-solver")
    endif ()
endforeach ()

#add_subdirectory(branch-and-bound)
#add_subdirectory(branch-and-price)
add_subdirectory(modeling)
add_subdirectory(wrappers)
#add_subdirectory(dualizer)
