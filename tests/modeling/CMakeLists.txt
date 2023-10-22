add_executable(test_modeling_interface
        Expr.operators-type.cpp
        Model.add-by-column.cpp
        Model.add-by-row.cpp
        Model.update-variable.cpp
        QuadExpr.rotated-cone.cpp
)

target_compile_definitions(test_modeling_interface PRIVATE OPTIMIZER=${OPTIMIZER})
target_link_libraries(test_modeling_interface PRIVATE idol Catch2::Catch2WithMain)