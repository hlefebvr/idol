LinExpr
=======

This class is used to represent linear expressions in the modeling interface of idol.

It consists in a set of linear terms which represent products of keys (typically, variables) and scalars.

.. admonition:: Example

    In this example, we create a linear expression representing the following linear expression.

    .. math::

        3 x_0 + 2 x_1

    .. code-block:: cpp

        auto x = model.add_vars(Dim<1>(2), 0, Inf, Continuous, "x");

        LinExpr lin_expr = 3 * x[0] + 2 * x[1];

.. doxygenclass:: LinExpr
