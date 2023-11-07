.. _cmake_options:

.. role:: bash(code)
   :language: bash

List of CMake Options
=====================

Linking with External Solvers
-----------------------------

For Gurobi (Commercial Solver)
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


For Mosek (Commercial Solver)
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

For GLPK (Open-Source Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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


For HiGHS (Open-Source Solver)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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


Building Examples
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

        make example_knapsack

    and run it.

    .. code-block::

        cd examples && ./example_knapsack


Linking with martinus/robin-hood-hashing
----------------------------------------

Internally, idol uses unordered map. Unfortunately, the default :code:`std` implementation is not the best choice when
it comes to performance. To avoid this, if found relevant, users can link idol with the hash map implementation of
`martinus/robin-hood-hashing <https://github.com/martinus/robin-hood-hashing>`_. It's actually a header-only library, so
all you have to do is to tell us where the :code:`robin_hood.hpp` file is. This is done thourgh the CMake option
:code:`ROBINHOOD_DIR=/path/to/robin_hood/folder` together with :code:`USE_ROBINHOOD=YES`.

Note that, if :code:`ROBINHOOD_DIR` is not specified, :code:`robin_hood.hpp` is searched for in :code:`/usr/include`,
:code:`/usr/include/robin_hood/`, :code:`/usr/local/include/`, :code:`/usr/local/include/robin_hood/` and in the path
stored in the environment variable :code:`ROBINHOOD_HOME`.


.. admonition:: Example

    The following will create an :bash:`install` target which will install idol with martinus's robin_hood hash map
    implementation.

    .. code-block::

        cmake -DUSE_ROBINhOOD=YES -DROBINHOOD_DIR=/path/to/robin_hood/folder ..


Building Unit and Integration Tests
-----------------------------------

To build tests for idol, please refer to this :ref:`developer page <tests>`.