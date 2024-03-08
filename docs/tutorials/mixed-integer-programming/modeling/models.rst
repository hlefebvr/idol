Models
------

The :ref:`Model <api_Model>` class is used to represent a (standard) optimization model, i.e., a mathematical program of the form

.. math::

    \begin{align}
        \min_x \ & c^\top x + x^\top D x + c_0 \\
        \text{s.t.} \ & a_{(i)}^\top x + x^\top Q^i x \le b_i \quad i=1,\dotsc,m \\
        & x_j \in \mathbb Z \quad j=1,\dotsc,p
    \end{align}

Here, :math:`x_j` are the decision variables of this optimization problem while matrices :math:`c, D, A` and :math:`Q^i` are given
input parameters for this model. Additionally, variables :math:`x_j` having :math:`j\in\{ 1,\dotsc,p \}` have to be integer-valued.


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
This is detailed in the :ref:`Variables <api_variables>`, :ref:`Constraints <api_constraints>` and :ref:`Objective <api_objective_functions>` sections.

Reading a Model from a File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A Model can also be created by reading a model from a file (typically, an MPS or an LP file).
To do this, idol relies on an external solver. The following code reads a model from a file using Gurobi.

.. code-block:: cpp

    Env env;
    auto model = Gurobi::read_from_file(env, "/path/to/model.mps");
    // auto model = GLPK::read_from_file(env, "/path/to/model.mps");

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
