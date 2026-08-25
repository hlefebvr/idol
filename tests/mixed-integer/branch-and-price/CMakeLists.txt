foreach (OPTIMIZER ${IDOL_TEST_SOLVERS})
    set(target_name test_bap_assignment_${OPTIMIZER})
    idol_add_catch_executable(${target_name} "assignment.cpp;../../cartesian_product.h")
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_solver_labels(${OPTIMIZER} solver_labels)
    foreach (INSTANCE RANGE 0 2)
        foreach (RELAXATION_SOLVER RANGE 0 2)
            set(test_name bap.${OPTIMIZER}.assignment.${INSTANCE}.${RELAXATION_SOLVER})
            idol_register_catch_test(${test_name} ${target_name} ""
                    "branch-and-price;milp;${solver_labels}"
            )
            set_tests_properties(${test_name} PROPERTIES ENVIRONMENT
                    "IDOL_BAP_INSTANCE=${INSTANCE};IDOL_BAP_SOLVER=${RELAXATION_SOLVER}"
            )
        endforeach ()
    endforeach ()
endforeach ()

if ("Gurobi" IN_LIST IDOL_TEST_SOLVERS)
    idol_add_catch_executable(test_bap_recession_rays_Gurobi recession-rays.cpp)
    idol_register_catch_test(bap.Gurobi.recession_rays test_bap_recession_rays_Gurobi ""
            "branch-and-price;ray;Gurobi;commercial-solver"
    )
endif ()
