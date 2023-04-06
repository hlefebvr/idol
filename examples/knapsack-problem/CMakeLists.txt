add_executable(ex_knapsack_problem main.cpp)
target_link_libraries(ex_knapsack_problem PUBLIC idol)


add_custom_command(TARGET ex_knapsack_problem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/instance.txt $<TARGET_FILE_DIR:ex_knapsack_problem>/instance.txt
)
