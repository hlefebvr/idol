.. _cmake_options:

.. role:: bash(code)
   :language: bash

List of CMake options
=====================

Linking with external solvers
-----------------------------

For Gurobi (commercial solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To link with Gurobi, you will need to pass the :bash:`USE_GUROBI` CMake option with value :bash:`YES`.

By default, CMake will look for Gurobi inside the folder indicated by the environment variable :bash:`GUROBI_HOME`
(see `this official Gurobi page <https://www.gurobi.com/documentation/10.0/quickstart_linux/software_installation_guid.html>`_).
Note that it is also possible to pass the CMake option :bash:`GUROBI_DIR=/path/to/gurobi/install/dir` in order to specify
another directory to look for Gurobi.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with Gurobi.

    .. code-block:: bash

        cmake -DUSE_GUROBI=YES -DGUROBI_DIR=/path/to/gurobi/install/dir ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block:: bash

        set(USE_GUROBI YES)
        set(GUROBI_DIR /path/to/gurobi/install/dir)


For Mosek (commercial solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To link with Mosek, you will need to pass the :bash:`USE_MOSEK` CMake option with value :bash:`YES`.

By default, CMake will look for Mosek inside the folder indicated by the environment variable :bash:`MOSEK_HOME`.
Note that it is also possible to pass the CMake option :bash:`MOSEK_DIR=/path/to/mosek/install/dir` in order to specify
another directory to look for Mosek.

Note that :bash:`MOSEK_HOME` (or equivalently :bash:`MOSEK_DIR`) should point to the Mosek installation folder where
folders :bash:`h` and :bash:`bin` can be found. For instance, :bash:`MOSEK_HOME=/home/<MY_USERNAME>/mosek/10.0/tools/platform/linux64x86`.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with Mosek.

    .. code-block:: bash

        cmake -DUSE_MOSEK=YES -DMOSEK_DIR=/path/to/mosek/install/dir ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block:: bash

        set(USE_MOSEK YES)
        set(MOSEK_DIR /path/to/mosek/install/dir)

.. attention::

    If you intend to use Mosek for solving QPs or SOCPs, please :ref:`read this <mosek_and_socp>`.

For GLPK (open-source solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To link with GLPK, you will need to pass the :bash:`USE_GLPK` CMake option with value :bash:`YES`.

By default, CMake will look for GLPK inside the folder indicated by the environment variable :bash:`GLPK_HOME` as well
as in the default installation folders :bash:`/usr/include` and :bash:`/usr/lib`.
Note that it is also possible to pass the CMake option :bash:`GLPK_DIR=/path/to/glpk/install/dir` in order to specify
another directory to look for GLPK.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with GLPK.

    .. code-block:: bash

        cmake -DUSE_GLPK=YES -GLPK_DIR=/path/to/glpk/install/dir ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block:: bash

        set(USE_GLPK YES)
        set(GLPK_DIR /path/to/glpk/install/dir)

Building examples
-----------------

:ref:`Examples :<examples>` can be found at the root level of the idol repository, inside the directory ``examples``.

By default, CMake will not generate targets to build these example. To tell CMake to generate them, you
need to set the CMake option ``BUILD_EXAMPLE`` to ``YES``.

.. admonition:: Example

    The following will tell CMake to create targets for examples.

    .. code-block::

        cmake -DBUILD_EXAMPLES=YES ..

    For instance, we can now build the Knapsack Problem example,

    .. code-block::

        make ex_knapsack_problem

    and run it.

    .. code-block::

        ./examples/knapsack-problem/ex_knapsack_problem

Building unit and integration tests
-----------------------------------

To build tests for idol, please refer to this :ref:`developer page <tests>`.