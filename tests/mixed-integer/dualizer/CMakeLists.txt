foreach (OPTIMIZER ${AVAILABLE_MILP_SOLVERS})

    set(target_name "test_dualizer_location_${OPTIMIZER}")

    add_executable(${target_name}
            location.cpp
            ../../cartesian_product.h
    )

    target_compile_definitions(${target_name} PRIVATE OPTIMIZER=${OPTIMIZER})
    target_link_libraries(${target_name} PRIVATE idol Catch2::Catch2WithMain)

endforeach ()
