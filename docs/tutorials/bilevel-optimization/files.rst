Reading and Writing Bilevel Instances (BOBiLib)
===============================================



.. contents:: Table of Contents
    :local:
    :depth: 2

Implementation
--------------

.. code::

    Env env;
    auto [model, description] =
        Bilevel::read_from_file<Gurobi>(env, aux_filename);


