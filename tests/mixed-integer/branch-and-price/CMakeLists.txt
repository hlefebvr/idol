foreach (OPTIMIZER ${IDOL_TEST_SOLVERS})
    set(target_name test_bap_assignment_${OPTIMIZER})
    idol_add_catch_executable(${target_name} "assignment.cpp;../../cartesian_product.h")
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_solver_labels(${OPTIMIZER} solver_labels)
    idol_register_catch_test(bap.${OPTIMIZER}.assignment ${target_name} ""
            "branch-and-price;milp;${solver_labels}"
    )
endforeach ()
