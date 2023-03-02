.. _api_Attr_Var_Lb:

Attr::Var::Lb
=============

.. doxygenvariable:: Attr::Var::Lb

This attribute is used to access a variable's lower bound.

**Examples:**

::

    // Access current variable's lower bound
    model.get(Attr::Var::Lb, x);

    // Update current variable's lower bound
    model.set(Attr::Var::Lb, x, 1);
