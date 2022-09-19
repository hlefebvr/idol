add_subdirectory(ex1_branch_and_bound_knapsack)

add_executable(dev main.cpp)
target_link_libraries(dev PRIVATE optimize)