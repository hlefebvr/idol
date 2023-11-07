.. _linking:

.. role:: bash(code)
   :language: bash

Linking Your C++ Project with Idol
==================================

Idol is built with and for CMake (even though it is still possible to link using :bash:`Makefile` or pure :bash:`gcc, clang, ...`).

If you are using CMake, here is a minimal :bash:`CMakeLists.txt` which creates a new executable target linked with idol.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    add_executable(my_target main.cpp)

    find_package(idol REQUIRED) ## Here, we search for the installed library idol

    target_link_library(my_target PUBLIC idol) ## Here, we ask CMake to link with idol

Note that this will also work when idol is linked with an external solver like Gurobi, Mosek or GLPK.

The following :bash:`main.cpp` is then a minimal example.

.. code-block:: cpp

    #include <iostream>
    #include <idol/modeling.h>

    int main(int t_argc, const char** t_argv) {

        Env env;

        Model model(env);

        Var x(env, 0., 1., Binary, "x");
        model.add(x);

        // ...

        return 0;
    }

