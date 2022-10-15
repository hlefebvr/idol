add_executable(dev dev_main.cpp)
target_link_libraries(dev PRIVATE idol)
target_include_directories(dev PRIVATE ../lib/include)