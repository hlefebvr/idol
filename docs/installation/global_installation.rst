.. _basics_installation:

.. role:: bash(code)
   :language: bash

Global Installation
===================

The installation process is managed by the CMake build system. You will therefore need to have it
installed on your computer (at least version 3.22 is required but it may work with earlier versions).

.. hint::

    If you simply want to "try out" idol, we advise you to opt for a :ref:`local installation <installation_local>`.

Creating the :bash:`install` Target
-----------------------------------

The first step is to create a :bash:`build` directory.
This is where the library will be compiled before being installed (i.e., copied) on your system.

.. code-block:: bash

    mkdir build
    cd build

Then, you should call CMake in order to create some targets (in particular, the :bash:`idol` and the :bash:`install` target).
To do so, we recommend you to specify some options in order to link with external solvers that you may have
already installed on your computer.

.. attention::

    By default, (i.e., if no options are given to CMake) idol will not be linked with any external solver.
    See the last section on this page for more details.

.. code-block:: bash

    cmake <YOUR_CMAKE_OPTIONS_SHOULD_GO_HERE> ..

.. admonition:: Example for Gurobi

    Here is an example of possible CMake command which will create the necessary targets for installing idol with Gurobi.

    .. code-block:: bash

        cmake -DUSE_GUROBI=YES ..

    Note that this example assumes that your environment variable :bash:`GUROBI_HOME` has been correctly configured
    (see `this official Gurobi page <https://www.gurobi.com/documentation/10.0/quickstart_linux/software_installation_guid.html>`_).

Building
--------

Now that targets have been created, we can build idol by running the following command.

.. code-block:: bash

    make idol

This will compile the idol C++ library and make it ready to be installed (i.e., copied) on your computer.

Installing
----------

If you want to install idol on your computer, run the following command.

.. code-block:: bash

    sudo make install

.. hint::

    Here, you need superuser rights in order to install idol globally. If you want to install idol on a specific folder,
    you may also use the :bash:`CMAKE_INSTALL_PREFIX` CMake option to change the destination folder.

    .. code-block:: bash

        cmake -CMAKE_INSTALL_PREFIX=/my/custom/installation/folder <YOUR_CMAKE_OPTIONS_SHOULD_GO_HERE> ..
        make install

Linking with External Solvers
------------------------------

To link idol with external solvers, specific options need to be passed to CMake.
A detailed list of such options can be found on :ref:`this page <cmake_options>`.

.. hint::

    Here is a quick guide on how to pass options to CMake. Say you want
    to pass the option :bash:`MY_OPTION` to CMake with the value :bash:`MY_VALUE`. Then, you should run the following
    command (inside :bash:`build/`).

    .. code-block:: bash

        cmake -DMY_OPTION=MY_VALUE ..

