
add_example(mibs)

add_executable(example_kkt kkt.example.cpp)
target_link_libraries(example_kkt PRIVATE idol)