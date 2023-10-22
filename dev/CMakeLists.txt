add_executable(dev main.cpp
        ../unit-tests/external-solvers/all/toy_milp.cpp
)
target_link_libraries(dev PRIVATE idol)
target_include_directories(dev PRIVATE ../lib/include)