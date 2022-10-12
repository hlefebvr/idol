add_executable(ex1_branch_and_bound_knapsack ex1_main.example.cpp)
target_link_libraries(ex1_branch_and_bound_knapsack PRIVATE idol)
target_include_directories(ex1_branch_and_bound_knapsack PRIVATE ../../lib/include)


add_custom_command(TARGET ex1_branch_and_bound_knapsack POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/demo.txt $<TARGET_FILE_DIR:ex1_branch_and_bound_knapsack>/demo.txt
)