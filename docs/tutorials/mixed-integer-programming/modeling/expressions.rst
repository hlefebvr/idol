Expressions with the :code:`QuadExpr`, :code:`AffExpr` and :code:`LinExpr` Classes
==================================================================================

An expression generically refers to any mathematical expression involving variables or constraints. In idol, expressions
are represented by the :code:`QuadExpr`, :code:`AffExpr` and :code:`LinExpr` classes. These classes are used to represent
quadratic, affine and linear expressions, respectively.

An expression can be created by adding, subtracting or multiplying variables together. For instance,
the following code creates the mathematical expression :math:`1 + 3 x_0 + x_1 + x_0 + 2 x_0 x_1`.

.. code:: cpp

    const AffExpr expr = 1 + 3 * x[0] + x[1] + x[0] + 2 * x[0] * x[1];

    std::cout << expr << std::endl;


.. contents:: Table of Contents
    :local:
    :depth: 2

:code:`LinExpr`: Linear Expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :code:`LinExpr` class is used to represent linear expressions. A linear expression is a mathematical expression
that is linear in the variables. For instance, the following code creates the mathematical expression :math:`1 + 3 x_0 + x_1`.

.. code:: cpp

    const LinExpr expr = 1 + 3 * x[0] + x[1];

    std::cout << expr << std::endl; // "1 + 3 * x[0] + 1 * x[1]"

.. admonition:: Linear in What?

    Actually, :code:`LinExpr` is a template class with one parameter, which is the type of the "variables" in the linear expression.
    For instance, it is possible de create a linear expression of :code:`Ctr` objects, which are constraints. The following code
    creates the mathematical expression :math:`1 + 3 c_0 + c_1`.

    .. code:: cpp

        const LinExpr<Ctr> expr = 1 + 3 * c[0] + c[1];

        std::cout << expr << std::endl; // "1 + 3 * c[0] + 1 * c[1]"

It is possible to iterate over the terms in the expression as follows.

.. code::

    for (const auto& [var, constant] : expr) {
        std::cout << constant << " multiplied by " << var << std::endl;
    }

:code:`AffExpr`: Affine Expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :code:`AffExpr` class is used to represent affine expressions. An affine expression is a mathematical expression
that is linear in the variables and has a constant term.

The linear part of the expression is accessed by the :code:`AffExpr::linear()` method, and the constant term is accessed
by the :code:`AffExpr::constant()` method.

:code:`QuadExpr`: Quadratic Expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :code:`QuadExpr` class is used to represent quadratic expressions. A quadratic expression is a mathematical expression
which contains an affine part and a quadratic part. The affine part can be accessed by the :code:`QuadExpr::affine()` method.

It is possible to iterate over the terms in the quadratic part of the expression as follows.

.. code::

    for (const auto& [pair, constant] : expr) {
        std::cout << constant << " multiplied by " << pair.first << " and " << pair.second << std::endl;
    }

    for (const auto& [var, constant] : expr.affine().linear()) {
        std::cout << constant << " multiplied by " << var << std::endl;
    }
