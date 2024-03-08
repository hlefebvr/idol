.. _installation_local:

.. role:: bash(code)
   :language: bash

Local Installation
==================

This page is dedicated to the local installation of idol. This is the easiest way to get your project working with
idol without installing it globally on your computer.
If you want to install idol globally, please read our :ref:`global installation guide <basics_installation>`.

The idea behind local installation is to let CMake download and manage the installation of idol in a sub-folder of your
project. This is done by using the CMake :bash:`FetchContent_*` directives.

We provide here a minimal :bash:`CMakeLists.txt`.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.23)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    include(FetchContent)

    # Define your idol CMake options here
    set(USE_GUROBI YES) # For instance, here, we specify that Gurobi will be used by idol

    # Tell CMake which version of idol you desire
    FetchContent_Declare(
            idol
            GIT_REPOSITORY https://github.com/hlefebvr/idol.git
            GIT_TAG        origin/main
    )

    # Ask CMake to download idol and install it to a sub-folder
    FetchContent_MakeAvailable(idol)

    # Normal executable definition
    add_executable(my_target main.cpp)

    # Link your CMake target with idol as classically done
    target_link_libraries(my_target PUBLIC idol)

By default, CMake will download the latest version of idol. However, you can also explicitly specify the desired version
of idol you want to use by setting the :bash:`GIT_TAG` variable in the :bash:`FetchContent_Declare` function to the specific
version, e.g., :bash:`v0.2.4-alpha`.

.. hint::

    As you can see in the above example, some options must be set to tell CMake to look for external solvers. Here, we
    specify that Gurobi will be used by idol. You can, of course, add other solvers such as Mosek or GLPK.

    This is done with the :bash:`set` CMake function. For instance, to set the option :bash:`MY_OPTION` to the
    value :bash:`MY_VALUE`. You should add :code:`set(MY_OPTION MY_VALUE)` to your :bash:`CMakeLists.txt` file.

    A list of **all possible options** can be found on :ref:`this page <cmake_options>`.

Then, here is a starting :bash:`main.cpp`.

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

