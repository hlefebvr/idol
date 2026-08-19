idol_add_catch_executable(test_bab_default_node_info_interface default-node-info-interface.cpp)
idol_register_catch_test(bab.default_node_info.interface test_bab_default_node_info_interface ""
        "branch-and-bound"
)

idol_add_catch_executable(test_bab_default_node_info_integrality default-node-info.test.cpp)
idol_register_catch_test(bab.default_node_info.integrality test_bab_default_node_info_integrality ""
        "branch-and-bound"
)

foreach (OPTIMIZER ${IDOL_TEST_SOLVERS})
    idol_solver_labels(${OPTIMIZER} solver_labels)

    set(basic_target test_bab_basic_${OPTIMIZER})
    idol_add_catch_executable(${basic_target} ../wrappers/basic.test.cpp)
    target_compile_definitions(${basic_target} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_register_catch_test(bab.${OPTIMIZER}.node_info ${basic_target} "[node-info]"
            "branch-and-bound;${solver_labels}"
    )
    idol_register_catch_test(bab.${OPTIMIZER}.best_estimate ${basic_target} "[best-estimate]"
            "branch-and-bound;milp;${solver_labels}"
    )
    idol_register_catch_test(bab.${OPTIMIZER}.strong_branching ${basic_target} "[strong-branching]"
            "branch-and-bound;milp;${solver_labels}"
    )

    set(knapsack_target test_bab_knapsack_${OPTIMIZER})
    idol_add_catch_executable(${knapsack_target} "knapsack.cpp;../../cartesian_product.h")
    target_compile_definitions(${knapsack_target} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_register_catch_test(bab.${OPTIMIZER}.knapsack ${knapsack_target} ""
            "branch-and-bound;milp;${solver_labels}"
    )

    set(location_target test_bab_location_${OPTIMIZER})
    idol_add_catch_executable(${location_target} "location.cpp;../../cartesian_product.h")
    target_compile_definitions(${location_target} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_register_catch_test(bab.${OPTIMIZER}.location ${location_target} ""
            "branch-and-bound;milp;${solver_labels}"
    )
endforeach ()
