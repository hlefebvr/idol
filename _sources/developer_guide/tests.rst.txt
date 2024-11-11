.. _tests:

Unit and integration tests
==========================

This page explains how to build the unit and integration tests on a local machine.

Dependencies
------------

Tests are managed by the C++ unit testing framework `Catch2 <https://github.com/catchorg/Catch2>`_.

When CMake is told to create test targets, it will automatically look for an installation of Catch2 on your
machine. If it cannot be found, CMake will attempt to download Catch2 from its GitHub repository.


Installation guide (Ubuntu)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: shell

    git clone https://github.com/catchorg/Catch2.git # Clone Catch2 repository

    cd Catch2 # go inside the new directory

    cmake -Bbuild -H. -DBUILD_TESTING=OFF # Create install target

    sudo cmake --build build/ --target install # Build and install


Build instructions
------------------

By default, CMake will not create test targets nor look for Catch2.  To tell CMake to generate them, you
need to set the CMake option ``BUILD_TESTS`` to ``YES``.


.. admonition:: Example

    The following will tell CMake to create targets for tests.

    .. code-block::

        cmake -DBUILD_TESTS=YES ..

    For instance, we can now build all the existing tests

    .. code-block::

        make tests

    and run them.

    .. code-block::

        ./tests/tests

Code coverage options
---------------------

Code coverage is done via ``gcov``.

If you want to active code coverage options, you shall set the CMake option ``WITH_TEST_COVERAGE`` to ``YES``.
Note that this only has an effect when ``BUILD_TESTS`` is also set to ``YES``.

.. admonition:: Example

    The following command will tell CMake to generate targets for tests with code coverage options.

    .. code-block::

        cmake -DBUILD_TESTS=YES -DWITH_CODE_COVERAGE=YES ..

    Then, you should build and run the tests.

    .. code-block::

        make tests && ./tests/tests

    Once you are there, you may generate a report to a file called ``coverage.info`` by using ``gcov``.

    .. code-block::

        lcov --directory . --capture --output-file coverage.info

Also note that this forces ``g++`` to be used for compiling.

.. tip::

    Code coverage reports are also automatically generated at every push action on idol's GitHub repository and
    are available on `codecov <https://app.codecov.io/gh/hlefebvr/idol>`_.
