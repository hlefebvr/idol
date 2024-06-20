function(ADD_EXAMPLE NAME)
    add_executable(example_${NAME} ${NAME}.example.cpp)
    target_link_libraries(example_${NAME} PRIVATE idol)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.data.txt)
        add_custom_command(TARGET example_${NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.data.txt $<TARGET_FILE_DIR:example_${NAME}>)
    endif()
endfunction()

add_subdirectory(mixed-integer-optimization)
