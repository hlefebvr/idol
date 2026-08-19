idol_add_catch_executable(test_modeling_interface
        "expressions.test.cpp;variables.test.cpp;linear-constraints.test.cpp;quadratic-constraints.test.cpp;objective.test.cpp"
)
idol_register_catch_test(modeling.expressions test_modeling_interface "[expressions]" "modeling")
idol_register_catch_test(modeling.variables test_modeling_interface "[variables]" "modeling")
idol_register_catch_test(modeling.linear_constraints test_modeling_interface "[linear-constraints]" "modeling")
idol_register_catch_test(modeling.quadratic_constraints test_modeling_interface "[quadratic-constraints]" "modeling")
idol_register_catch_test(modeling.objective test_modeling_interface "[objective]" "modeling")

idol_add_catch_executable(test_modeling_basic basic.test.cpp)
idol_register_catch_test(modeling.basic.expressions test_modeling_basic "[basic][expressions]" "modeling")
idol_register_catch_test(modeling.basic.variables test_modeling_basic "[basic][variables]" "modeling")
idol_register_catch_test(modeling.basic.constraints test_modeling_basic "[basic][constraints]" "modeling")
idol_register_catch_test(modeling.basic.objective test_modeling_basic "[basic][objective]" "modeling")
idol_register_catch_test(modeling.basic.model_copy test_modeling_basic "[basic][model-copy]" "modeling")

idol_add_catch_executable(test_modeling_presolve presolve.test.cpp)
idol_register_catch_test(modeling.presolve test_modeling_presolve "" "modeling;presolve")

foreach (OPTIMIZER ${IDOL_TEST_SOLVERS})
    set(target_name test_modeling_dualizer_${OPTIMIZER})
    idol_add_catch_executable(${target_name} dualizer.test.cpp)
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_solver_labels(${OPTIMIZER} solver_labels)
    idol_register_catch_test(modeling.dualizer.${OPTIMIZER} ${target_name} "" "modeling;${solver_labels}")
endforeach ()
