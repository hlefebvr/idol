.. _basics_local_installation:

.. role:: bash(code)
   :language: bash

Local installation guidelines
=======================

Another possibility to start building your application with idol is to install it "locally" using
the CMake :bash:`FetchContent_*` directives. This will tell CMake to automatically download (from the GitHub repository)
the desired version of idol and to make it available for your project.

We provide here a minimal :bash:`CMakeLists.txt`.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.23)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    include(FetchContent)

    set(USE_GUROBI ON) # Tell idol to link with Gurobi

    FetchContent_Declare(
            idol
            GIT_REPOSITORY https://github.com/hlefebvr/idol.git
            GIT_TAG        origin/main # You may also use a tag, e.g., v0.0.2-alpha
    )

    FetchContent_MakeAvailable(idol)

    add_executable(my_target main.cpp)

    target_link_library(my_target PUBLIC idol)

Then, here is a starting :bash:`main.cpp`.

.. code-block:: cpp

    #include <iostream>
    #include <modeling.h> // Note that we do not include <idol/modeling.h>

    int main(int t_argc, const char** t_argv) {

        Env env;

        Model model(env);

        Var x(env, 0., 1., Binary, "x");
        model.add(x);

        // ...

        return 0;
    }

