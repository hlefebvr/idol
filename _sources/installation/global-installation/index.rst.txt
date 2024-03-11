.. _basics_installation:

.. role:: bash(code)
   :language: bash

Global Installation
===================

This page is dedicated to the local installation of idol. It is thought for more advanced users who want to install
idol globally on their computer (e.g., in `/usr/lib`).
If you simply want to "try out" idol, we advise you to opt for a :ref:`local installation <installation_local>`.
The installation process is managed by the CMake build system. You will therefore need to have it
installed on your computer.

The installation process follows the classical CMake build process "configure, build, install".

Configure
---------

The first step is to create a :bash:`build` directory.
This is where the library will be compiled before being it is installed (i.e., copied) on your system.

.. code-block:: bash

    mkdir build
    cd build

Then, you should call CMake to configure the build process.
It is at this time that we let CMake look for the necessary dependencies such as, e.g., external optimization solvers.
By default, running CMake without options will cause idol to be built without any external solver. Note that options
should be specified as follows:

.. code-block:: bash

    cmake -D<YOUR_CMAKE_OPTION>=<YOUR_VALUE> ..

A list of **all possible options** can be found on :ref:`this page <cmake_options>`.

.. admonition:: Example for Gurobi

    Here is an example of a possible CMake command which will create the necessary targets for configuring idol to interface Gurobi.

    .. code-block:: bash

        cmake -DUSE_GUROBI=YES ..

    Note that this example assumes that your environment variable :bash:`GUROBI_HOME` has been correctly configured
    (see `this official Gurobi page <https://www.gurobi.com/documentation/10.0/quickstart_linux/software_installation_guid.html>`_).

    If you want to specify the path to the Gurobi installation folder, you can use the :bash:`GUROBI_DIR` option.

    .. code-block:: bash

        cmake -DUSE_GUROBI=YES -DGUROBI_DIR=/path/to/gurobi/install/dir ..


Build
-----

Now that the build process has been configured, we can build idol by running the following command.

.. code-block:: bash

    make idol

This will compile the idol C++ library, and make it ready to be installed (i.e., copied) on your computer.

If you encounter any error at this stage, please, do not hesitate to contact us on our `GitHub page <https://github.com/hlefebvr/idol>`_.

Install
-------

To install idol on your computer, run the following command.

.. code-block:: bash

    sudo make install

.. hint::

    Here, you need superuser rights to install idol globally. If you want to install idol on a specific folder,
    you may also use the :bash:`CMAKE_INSTALL_PREFIX` CMake option to change the destination folder.

    .. code-block:: bash

        cmake -CMAKE_INSTALL_PREFIX=/my/custom/installation/folder <YOUR_OTHER_CMAKE_OPTIONS...> ..
        make install


Table of Contents
-----------------

.. toctree::
   :maxdepth: 1
   :glob:

   *
