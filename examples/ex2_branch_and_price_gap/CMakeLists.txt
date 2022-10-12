add_executable(ex2_branch_and_price_gap ex2_main.cpp ../../lib/include/problems/gap/GAP_Instance.h solve_with_external_solver.h solve_with_branch_and_price.h)
target_link_libraries(ex2_branch_and_price_gap PRIVATE idol)
target_include_directories(ex2_branch_and_price_gap PRIVATE ../../lib/include)

add_custom_command(TARGET ex2_branch_and_price_gap POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/demo.txt $<TARGET_FILE_DIR:ex2_branch_and_price_gap>/demo.txt
)