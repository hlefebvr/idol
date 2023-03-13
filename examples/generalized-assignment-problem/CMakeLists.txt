add_executable(ex_generalized_assignment_problem main.cpp)
target_link_libraries(ex_generalized_assignment_problem PUBLIC idol)


add_custom_command(TARGET ex_generalized_assignment_problem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/instance.txt $<TARGET_FILE_DIR:ex_generalized_assignment_problem>/instance.txt
)
