
add_example(mibs)

add_executable(example_kkt kkt.example.cpp KKT.h KKT.cpp)
target_link_libraries(example_kkt PRIVATE idol)