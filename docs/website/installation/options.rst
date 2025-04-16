.. _cmake_options:

.. role:: bash(code)
   :language: bash

List of CMake Options
=====================

This page contains a list of all CMake options that can be used to configure idol. In particular, this is useful to
link idol with external solvers.

.. contents:: Table of Contents
    :local:
    :depth: 2

Linking with External Solvers
-----------------------------

Gurobi (Commercial Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^

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


Mosek (Commercial Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^

To link with Mosek, you will need to pass the :bash:`USE_MOSEK` CMake option with value :bash:`YES`.

By default, CMake will look for Mosek inside the folder indicated by the environment variable :bash:`MOSEK_HOME`.
Note that it is also possible to pass the CMake option :bash:`MOSEK_DIR=/path/to/mosek/install/dir` in order to specify
another directory to look for Mosek.

Note that :bash:`MOSEK_HOME` (or equivalently :bash:`MOSEK_DIR`) should point to the Mosek installation folder where
folders :bash:`h` and :bash:`bin` can be found. For instance, :bash:`MOSEK_HOME=~/mosek/10.0/tools/platform/linux64x86`.

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

GLPK (Open-Source Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^

To link with GLPK, you will need to pass the :bash:`USE_GLPK` CMake option with value :bash:`YES`.

By default, CMake will look for GLPK inside the folder indicated by the environment variable :bash:`GLPK_HOME` as well
as in the default installation folders :bash:`/usr/include` and :bash:`/usr/lib`.
Note that it is also possible to pass the CMake option :bash:`GLPK_DIR=/path/to/glpk/install/dir` in order to specify
another directory to look for GLPK.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with GLPK.

    .. code-block:: bash

        cmake -DUSE_GLPK=YES -DGLPK_DIR=/path/to/glpk/install/dir ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block:: bash

        set(USE_GLPK YES)
        set(GLPK_DIR /path/to/glpk/install/dir)


HiGHS (Open-Source Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^

To link with GLPK, you will need to pass the :bash:`USE_HIGHS` CMake option with value :bash:`YES`.

By default, CMake will look for HIGHS inside the default installation folders :bash:`/usr/include` and :bash:`/usr/lib`.
Note that it is also possible to pass the CMake option :bash:`HIGHS_DIR=/path/to/highs/install/dir` in order to specify
another directory to look for GLPK.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with GLPK.

    .. code-block:: bash

        cmake -DUSE_HIGHS=YES -DHIGHS_DIR=/path/to/glpk/install/dir ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block:: bash

        set(USE_HIGHS YES)
        set(HIGHS_DIR /path/to/glpk/install/dir)

coin-or (Open-Source Solvers)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

idol can be linked with some of the coin-or libraries.
Hereafter, we give a list of such libraries. For all the :code:`USE_*` options, you will need to define the environment
variable :code:`COIN_OR_HOME` or to set the CMake option :code:`COIN_OR_DIR`. It must point to the :code:`dist` directory
of coin-or.

.. admonition:: Example

    The following will prepare CMake for linking against coin-or libraries.

    .. code-block::

        cmake -DCOIN_OR_DIR=/path/to/coin_or/dist <OTHER_OPTIONS_GO_HERE> ..

    If you are doing a *local installation*, then the same can be achieved as follows.

    .. code-block::

        set(COIN_OR_DIR /path/to/coin_or/dist)

We now give the list of options which can be used in relation with coin-or libraries.

- :code:`USE_OSI=YES`: use the Osi solver.
- :code:`USE_SYMPHONY=YES`: use the Symphony solver (can only be used through Osi).
- :code:`USE_CBC=YES`: use the Cbc solver (can only be used through Osi).
- :code:`USE_CLP=YES`: use the Clp solver (can only be used through Osi).
- :code:`USE_CPLEX=YES`: use the Cplex solver (can only be used through Osi).
- :code:`USE_MIBS=YES`: use the MibS solver (will automatically search for Alps, Bcps, Blis, and Cgl).

Building Examples
-----------------

:ref:`Examples :<examples>` can be found at the root level of the idol repository, inside the directory ``examples``.

By default, CMake will not generate targets to build these example. To tell CMake to generate these, you
need to set the CMake option ``BUILD_EXAMPLE`` to ``YES``.

.. admonition:: Example

    The following will tell CMake to create targets for examples.

    .. code-block::

        cmake -DBUILD_EXAMPLES=YES ..

    For instance, we can now build the Knapsack Problem example,

    .. code-block::

        make example_knapsack

    and run it.

    .. code-block::

        cd examples && ./example_knapsack

Other optional dependencies
---------------------------

Using martinus/robin-hood-hashing hash map
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Internally, idol uses unordered maps (also called hash maps).
Unfortunately, the default :code:`std` implementation is not the best choice when
it comes to performance. To avoid this, if found relevant, users can tell idol to use the hash-map implementation of
`martinus/robin-hood-hashing <https://github.com/martinus/robin-hood-hashing>`_. It's actually a header-only library, so
all you have to do is to tell CMake where the :code:`robin_hood.hpp` file is. This is done through the CMake option
:code:`ROBINHOOD_DIR=/path/to/robin_hood/folder` together with :code:`USE_ROBINHOOD=YES`.

Note that, if :code:`ROBINHOOD_DIR` is not specified, :code:`robin_hood.hpp` is searched for in :code:`/usr/include`,
:code:`/usr/include/robin_hood/`, :code:`/usr/local/include/`, :code:`/usr/local/include/robin_hood/` and in the path
stored in the environment variable :code:`ROBINHOOD_HOME`.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with martinus's robin_hood hash map
    implementation.

    .. code-block::

        cmake -DUSE_ROBINHOOD=YES -DROBINHOOD_DIR=/path/to/robin_hood/folder ..

Using Eigen
^^^^^^^^^^^

Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
It is internally used by idol for some of its functionalities. In particular, this is *necessary* for solving QPs and SOCPs
with the Mosek solver.

Since Eigen is a header-only library, all you have to do is to tell CMake where the :code:`Eigen` folder is. This is done
through the CMake option :code:`EIGEN_DIR=/path/to/eigen/folder` together with :code:`USE_EIGEN=YES`.

.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with Eigen.

    .. code-block::

        cmake -DUSE_EIGEN=YES -DEIGEN_DIR=/path/to/eigen/folder ..

Building Unit and Integration Tests
-----------------------------------

To build tests for idol, please refer to this :ref:`developer page <tests>`.