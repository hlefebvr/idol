.. _dev:

Developers
==========

Functional Testing
------------------

The stable features of idol are tested using :code:`Catch2`.
Currently, the level of testing is not sufficient, but we are working on it.

Here is a grid plot of the code coverage:

.. image:: https://codecov.io/gh/hlefebvr/idol/graphs/tree.svg?token=BWMH5522QP
    :align: center

*Each block represents a single file in the project. The size and color of each block is represented by the number of statements and the coverage, respectively.* (`see codecov <https://app.codecov.io>`_)

For more information on how to run the tests, please refer to the :ref:`tests` page.

To see what feature is currently tested, you can look at our :ref:`functional tests page <functional_tests>`.

Building the Documentation
--------------------------

The documentation is built using `doxygen <https://www.doxygen.nl/>`_ and a bunch of other tools. For more information, please refer to the :ref:`dev_build_the_doc` page.

Related Pages
-------------

.. toctree::
   :maxdepth: 2
   :glob:

   build-the-doc
   tests
   tests/index
