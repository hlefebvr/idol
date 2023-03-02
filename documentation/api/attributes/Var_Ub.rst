.. _api_Attr_Var_Ub:

Attr::Var::Ub
=============

.. doxygenvariable:: Attr::Var::Ub

This attribute is used to access a variable's upper bound.

**Examples:**

::

    // Access current variable's upper bound
    model.get(Attr::Var::Ub, x);

    // Update current variable's upper bound
    model.set(Attr::Var::Ub, x, 0);
