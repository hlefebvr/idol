add_executable(ex4_quadratic ex4_main.example.cpp ex4_main.example.cpp)
target_link_libraries(ex4_quadratic PRIVATE idol)
target_include_directories(ex4_quadratic PRIVATE ../../lib/include)
