set(WRAPPER_TEST_SOLVERS GLPK HiGHS)
if (DEFINED ENV{GUROBI_HOME} AND NOT "$ENV{GUROBI_HOME}" STREQUAL "")
    list(APPEND WRAPPER_TEST_SOLVERS Gurobi)
endif ()
if (USE_CPLEX)
    list(APPEND WRAPPER_TEST_SOLVERS Cplex)
endif ()
if ("OsiClp" IN_LIST IDOL_TEST_SOLVERS)
    list(APPEND WRAPPER_TEST_SOLVERS OsiClp)
endif ()
if ("OsiSymphony" IN_LIST IDOL_TEST_SOLVERS)
    list(APPEND WRAPPER_TEST_SOLVERS OsiSymphony)
endif ()

foreach (OPTIMIZER ${WRAPPER_TEST_SOLVERS})
    set(target_name test_wrapper_${OPTIMIZER})
    idol_add_catch_executable(${target_name}
            "solve_lp.test.cpp;solve_milp.test.cpp;universal-callbacks.test.cpp"
    )
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER} OPTIMIZER_IS_${OPTIMIZER})
    if (OPTIMIZER STREQUAL "GLPK" OR OPTIMIZER STREQUAL "HiGHS" OR OPTIMIZER STREQUAL "Gurobi")
        target_compile_definitions(${target_name} PRIVATE WRAPPER_HAS_FARKAS)
    endif ()
    idol_solver_labels(${OPTIMIZER} solver_labels)
    idol_register_catch_test(wrapper.${OPTIMIZER}.lp ${target_name} "[LP]" "wrapper;lp;${solver_labels}")
    idol_register_catch_test(wrapper.${OPTIMIZER}.milp ${target_name} "[MILP]" "wrapper;milp;${solver_labels}")
    idol_register_catch_test(wrapper.${OPTIMIZER}.callbacks ${target_name} "[Callbacks]" "wrapper;callbacks;${solver_labels}")
endforeach ()

foreach (OPTIMIZER ${IDOL_TEST_SOLVERS})
    set(target_name test_wrapper_basic_${OPTIMIZER})
    idol_add_catch_executable(${target_name} basic.test.cpp)
    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    idol_solver_labels(${OPTIMIZER} solver_labels)
    idol_register_catch_test(wrapper.${OPTIMIZER}.basic ${target_name}
            "[basic][solver]~[branch-and-bound]" "wrapper;lp;milp;${solver_labels}"
    )
endforeach ()

idol_add_catch_executable(test_wrapper_file_import_GLPK file-import.test.cpp)
target_compile_definitions(test_wrapper_file_import_GLPK PRIVATE
        TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)
idol_register_catch_test(wrapper.GLPK.file_import test_wrapper_file_import_GLPK ""
        "wrapper;file-import;GLPK;free-solver"
)

if (DEFINED ENV{GUROBI_HOME} AND NOT "$ENV{GUROBI_HOME}" STREQUAL "")
    idol_add_catch_executable(test_wrapper_file_import_Gurobi file-import.test.cpp)
    target_compile_definitions(test_wrapper_file_import_Gurobi PRIVATE
            TEST_GUROBI_IMPORT
            TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )
    idol_register_catch_test(wrapper.Gurobi.file_import test_wrapper_file_import_Gurobi ""
            "wrapper;file-import;Gurobi;commercial-solver"
    )
endif ()

if (USE_OSI OR USE_MIBS OR USE_CGL OR USE_OSI_CLP OR USE_OSI_SYMPHONY)
    idol_add_catch_executable(test_wrapper_osi_objective_sense osi-objective-sense.test.cpp)
    idol_register_catch_test(wrapper.Osi.objective_sense test_wrapper_osi_objective_sense "" "wrapper")
endif ()
