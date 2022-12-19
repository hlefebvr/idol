.. _basics_installation:

.. role:: bash(code)
   :language: bash

Global installation guidelines
=======================

Global installation
-------------------

The installation process is managed by CMake. You will therefore need to have
installed the CMake build system (at least version 3.22 is required - not that
it may work with earlier versions but has not been tested).

Build directory
^^^^^^^^^^^^^^

Then, create a :bash:`build` directory. This is where the library will be compiled before being installed (i.e., moved) on your system.

.. code-block:: bash

    mkdir build
    cd build

Then, you should call CMake in order to create an :bash:`install` target.
Running CMake without any option will cause to create a target for installing
the library without external solvers. This is done as follows (inside :bash:`build/`).

.. code-block:: bash

    cmake ..

If you want idol to be installed with an external solver (which is recommended) you should use
the following options.

Options
^^^^^^^

For Gurobi (commercial solver)
""""""""""""""""

* :bash:`USE_GUROBI=YES` will tell CMake to search and link with Gurobi.
* | :bash:`GUROBI_DIR=/path/to/gurobi/install/dir` will tell CMake where to find Gurobi.
  | This option is *not mandatory* (by default, the environment variable :bash:`GUROBI_HOME` is used).

The following will create an :bash:`install` target which will install idol with Gurobi.

.. code-block:: bash

    cmake -DUSE_GUROBI=YES -DGUROBI_DIR=/path/to/gurobi/install/dir ..

For GLPK (open-source solver)
""""""""""""""""

* :bash:`USE_GLPK=YES` will tell CMake to search and link with GLPK.
* | :bash:`GLPK_DIR=/path/to/glpk/install/dir` will tell CMae where to find GLPK.
  | This option is *not mandatory* (the default GLPK install folders will be used when not specified.
  | Alternatively, :bash:`GLPK_HOME` can be defined as environment variable.).

The following will create an :bash:`install` target which will install idol with GLPK.

.. code-block:: bash

    cmake -DUSE_GLPK=YES -DGLPK_DIR=/path/to/glpk/install/dir ..

Compiling and installing
^^^^^^^^^^^^^^^^^^^^^^^^

Then, idol is compiled and installed as follows.

.. code-block:: bash

    make && sudo make install

Linking with idol
-----------------

Idol is built with and for CMake (even though it is still possible to link using :bash:`Makefile` or pure :bash:`gcc, clang, ...`).

If you are using CMake, here is a minimal :bash:`CMakeLists.txt` which creates a new executable target linked with idol.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    project(my_project)

    set(CMAKE_CXX_STANDARD 17)

    add_executable(my_target main.cpp)

    find_package(idol REQUIRED)

    target_link_library(my_target PUBLIC idol)

Note that this will also work when idol is linked with an external solver like Gurobi or GLPK.

The following :bash:`main.cpp` is then a minimal example.

.. code-block:: cpp

    #include <iostream>
    #include <idol/modeling.h>

    int main(int t_argc, const char** t_argv) {

        Model model;

        auto x = model.add_var(0., 1., Continuous, 0., "x");

        // ...

        return 0;
    }
