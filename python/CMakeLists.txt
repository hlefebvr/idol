find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

pybind11_add_module(pydol MODULE pydol.cpp)

target_link_libraries(pydol PRIVATE idol)
