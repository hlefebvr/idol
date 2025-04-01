Reading and Writing Bilevel Instances (BOBILib)
===============================================

`BOBILib <https://BOBILib.org/>`_ is a library of instances for bilevel optimization problems.
In this tutorial, we will show how to read and write bilevel instances in the format which is used by BOBILib.
The advantage is twofold. First, we can easily access a large number of bilevel instances.
Second, we can easily share our bilevel instances with others.

Let's get started!

.. contents:: Table of Contents
    :local:
    :depth: 2

The Format
----------

The instances in BOBILib are stored in two files:

- An :code:`.mps` or :code:`.lp` file containing the high-point relaxation of the bilevel problem,
- An :code:`.aux` file containing the description of the lower-level problem.

For more details, we refer to the report :cite:`BOBILib`.

Implementation
--------------

Reading Instances
^^^^^^^^^^^^^^^^^

Reading a BOBILib instance is straightforward. We can use the :code:`Bilevel::read_from_file` function.
However, it requires an underlying solver to parse the :code:`.mps` file. Here, we will use Gurobi.

.. code::

    Env env;
    auto [model, description] =
        Bilevel::read_from_file<Gurobi>(env, aux_filename);

The function returns a pair made by the high-point relaxation model and the description of the lower-level problem.

Writing Instances
^^^^^^^^^^^^^^^^^

Assume that you have a bilevel model :code:`model` and a description :code:`description` for the lower-level problem.
Writing this problem to file can be done by a single call to :code:`Bilevel::write_to_file`.
Here, no external solver is necessary.

.. code::

    /*
        the following creates two files:
        - instance-filename.mps
        - instance-filename.aux
    */
    Bilevel::write_to_file(model, description, "instance-filename");

