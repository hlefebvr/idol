.. _linking:

.. role:: bash(code)
   :language: bash

Linking Your C++ Project
========================

If you have installed idol globally, you can link your C++ project with idol using CMake.
Note that if idol was locally installed, you should refer to the :ref:`local installation guideline <installation_local>`,
this page is **only for global installation**.

Idol is built with and for CMake (even though it is still possible to link using :bash:`Makefile` or pure :bash:`gcc`, :bash:`clang`, ...).

Here is a minimal :bash:`CMakeLists.txt` which creates a new executable target linked with idol.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    add_executable(my_target main.cpp)

    find_package(idol REQUIRED) ## Here, we search for the installed library idol

    target_link_library(my_target PUBLIC idol) ## Here, we ask CMake to link with idol

Note that you do not need to link with any external solver if idol was already linked with one.

The following :bash:`main.cpp` is then a minimal example.

.. code-block:: cpp

    #include <iostream>
    #include <idol/modeling.h>

    int main(int t_argc, const char** t_argv) {

        using namespace idol;

        Env env;

        Model model(env);

        const auto x = model.add_var(0., 1., Binary, "x");

        // ...

        return 0;
    }

