.. _installation_local:

.. role:: bash(code)
   :language: bash

Local installation
==================

This page is dedicated to local installations of idol. This is the easiest way to get your project working with
idol without installing it globally on your computer.

.. hint::

    If you want to install idol globally, please read our :ref:`global installation guide <basics_installation>`.

The idea behind local installations is to let CMake download and manage the installation of idol in a sub-folder of your
project. This is done by using the CMake :bash:`FetchContent_*` directives.

Using this method, you can also explicitly specify the desired version of idol which you want to use.

We provide here a minimal :bash:`CMakeLists.txt`.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.23)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    include(FetchContent)

    # Define your CMake options here
    set(USE_GUROBI YES) # For instance, tell CMake to link idol with Gurobi

    # Tell CMake which version of idol you desire
    FetchContent_Declare(
            idol
            GIT_REPOSITORY https://github.com/hlefebvr/idol.git
            GIT_TAG        origin/main # You may also use a tag, e.g., v0.2.4-alpha
    )

    # Ask CMake to download idol and install it to a sub-folder
    FetchContent_MakeAvailable(idol)

    # Normal executable definition
    add_executable(my_target main.cpp)

    # Link your CMake target with idol as classically done
    target_link_libraries(my_target PUBLIC idol)

.. hint::

    As you have seen, some options must be set in CMake in order to tell it to link idol with external solvers.
    This is done with the :bash:`set` CMake function. Say you want
    to set the option :bash:`MY_OPTION` to the value :bash:`MY_VALUE`. Then, you should add the following
    function call.

    .. code-block:: bash

        set(MY_OPTION MY_VALUE)

.. seealso::

    A list of all possible options which can be set can be found on the :ref:`global installation guide <basics_installation>`.
    In particular, you will find options to link with Gurobi, Mosek and GLPK.

Then, here is a starting :bash:`main.cpp`.

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

