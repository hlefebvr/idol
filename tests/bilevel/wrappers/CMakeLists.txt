foreach (OPTIMIZER ${AVAILABLE_BILEVEL_SOLVERS})

    set(target_name "test_wrapper_${OPTIMIZER}")

    add_executable(${target_name}
            solve_bilevel.test.cpp
    )

    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    target_link_libraries(${target_name} PRIVATE idol Catch2::Catch2WithMain)

endforeach ()
