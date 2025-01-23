The :code:`Model` class
-----------------------

The :ref:`Model <api_Model>` class is used to represent a (standard) optimization model of the form

.. math::
    :label: eq:mip

    \begin{align}
        \min_{x} \quad & c^\top x + x^\top D x + c_0 \\
        \text{s.t.} \quad & a_{i\cdot}^\top x + x^\top Q^i x \le b_i, \quad i = 1, \ldots, m, \\
        & x_j \in \mathbb{Z}, \quad j \in I \subseteq \{ 1, \dotsc, n \}.
    \end{align}

Here, :math:`x` are the decision variables, while :math:`c`, :math:`D`, :math:`c_0`, :math:`a_i`, :math:`Q^i`, and :math:`b_i` are given data.
Some of the decision variables are required to be integer-valued and are indexed by :math:`I`.

In what follows, we will dive into the :cpp:`Model` class and see how to create, read, and write models to files.


.. contents:: Table of Contents
    :local:
    :depth: 2

Creating a Model
^^^^^^^^^^^^^^^^

A Model is created by calling the constructor of the :cpp:`Model` class. Note that it is necessary to pass an environment
to the constructor.

The following code creates a new optimization model.

.. code-block:: cpp

    Env env;
    Model model(env);

We can now define our decision variables and constraints, and add an objective function to our model.
This is detailed in the :ref:`Variables <api_variables>`, :ref:`Constraints <api_constraints>` and :ref:`Objective <api_objective_functions>` sections, respectively.

Reading a Model from a File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A Model can also be created by reading a model from a file (typically, a :code:`.mps` or an :code:`.lp` file).
To do this, idol relies on an external solver. The following code reads a model from a file using Gurobi.

.. code-block:: cpp

    Env env;
    auto model = Gurobi::read_from_file(env, "/path/to/model.mps");
    // auto model = GLPK::read_from_file(env, "/path/to/model.mps");

Writing a Model to a File
^^^^^^^^^^^^^^^^^^^^^^^^^

To write an optimization model to a file, the easiest way is again to rely on an external solver.
This is done by attaching a solver to a model, then by calling the :code:`Model::write` method.

.. code::

    Env env;
    Model model(env);

    // Add variables and constraints HERE...

    model.use(Gurobi()); // Set the optimizer as Gurobi

    model.write("instance.lp"); // Creates a new file "instance.lp" storing the model

Iterating over the Variables and Constraints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :ref:`Model <api_Model>` class provides methods to iterate over the variables and constraints of the model.
See for instance, the following example.

.. code-block:: cpp

    for (const auto& var : model.vars()) {
        std::cout << "Variable " << var.name() << " belongs to the model." << std::endl;
    }

    for (const auto& ctr : model.ctrs()) {
        std::cout << "Constraint " << ctr.name() << " belongs to the model." << std::endl;
    }

The number of variables and constraints can be obtained by doing the following.

.. code:: cpp

    std::cout << "Nb. of variables: " << model.vars().size() << std::endl;
    std::cout << "Nb. of constraints: " << model.ctrs().size() << std::endl;
