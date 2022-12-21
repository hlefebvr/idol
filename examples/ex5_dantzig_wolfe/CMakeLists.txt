add_executable(ex5_dantzig_wolfe ex5_dantzig_wolfe_main.cpp)
target_link_libraries(ex5_dantzig_wolfe PRIVATE idol)
target_include_directories(ex5_dantzig_wolfe PRIVATE ../../lib/include)
