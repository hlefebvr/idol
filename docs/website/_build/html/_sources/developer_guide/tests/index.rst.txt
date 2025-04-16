.. _functional_tests:

Functional Testing
===================

Functional tests are designed to test the functionality of the application as a whole.
They simulate real user interactions and verify that the application behaves as expected.

This is also a good way to make sure that the feature you want to use is properly tested and stable.

.. warning:: Limitations

    A current limitation is that commerical solvers like Gurobi and CPLEX are not tested on GitHub Actions.
    This is due to the fact that they are not open source and require a license to run.
    Hence, the tests are not run on the CI/CD pipeline.

.. toctree::
   :maxdepth: 2
   :glob:

   mixed-integer-optimization