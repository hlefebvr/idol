add_subdirectory(ex1_branch_and_bound_knapsack)
add_subdirectory(ex2_branch_and_price_gap)

add_executable(dev main.cpp)
target_link_libraries(dev PRIVATE idol)
target_include_directories(dev PRIVATE ../lib/include)