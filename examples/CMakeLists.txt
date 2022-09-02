add_executable(example main.cpp ../tests/Types.test.cpp)
target_link_libraries(example PRIVATE optimize)