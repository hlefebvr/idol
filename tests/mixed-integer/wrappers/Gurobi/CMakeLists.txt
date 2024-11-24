add_executable(test_wrapper_Gurobi_specific
        Gurobi.create-constraint-with-backend.cpp
        Gurobi.create-constraint-without-backend.cpp
        Gurobi.create-variable-and-constraint-with-backend.cpp
        Gurobi.create-variable-with-backend.cpp
        Gurobi.create-variable-without-backend.cpp
        Gurobi.update-constraint-with-backend.cpp
        Gurobi.update-objective-with-backend.cpp
        Gurobi.update-variable-with-backend.cpp
)

target_compile_definitions(test_wrapper_Gurobi_specific PRIVATE OPTIMIZER=${OPTIMIZER})
target_link_libraries(test_wrapper_Gurobi_specific PRIVATE idol Catch2::Catch2WithMain)