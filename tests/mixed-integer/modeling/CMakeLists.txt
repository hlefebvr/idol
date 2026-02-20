add_executable(test_modeling_interface
        expressions.test.cpp
        variables.test.cpp
        linear-constraints.test.cpp
        quadratic-constraints.test.cpp
        objective.test.cpp
)

target_link_libraries(test_modeling_interface PRIVATE idol Catch2::Catch2WithMain)
