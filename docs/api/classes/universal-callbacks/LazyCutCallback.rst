LazyCutCallback
===============

Lazy cuts are essentially constraints which are part of an optimization model but which have been omitted in the current
definition of the model. Lazy cuts typically arise in models with a large number of constraints. Instead of enumerating
then all, it may be judicious to omit some of them to get a smaller optimization model. Then, the returned solution is
checked for feasibility against the whole set of constraints. If a violated constraint is identified, we add the
constraint to the model and resolve. If not, then all the constraints are satisfied and the solution is optimal for the
original model in which all constraints materializes.

The LazyCutCallback can be used to implement lazy cut constraint generation on the fly, during the optimization process.

Consider the following optimization problem:

.. math::

    \begin{align}
        \min_x \ & c^\top x \\
        \text{s.t.} \ & x\in X, \\
        & \xi^\top x \le \xi_0 \quad (\xi_0,\xi) \in \Xi,
    \end{align}

in which assume that :math:`|\Xi|` is large (potentially infinite). The idea is to start by solving the following
relaxed master problem in which constraints associated to :math:`(\xi_0,\xi)` have been omitted.

.. math::

    \begin{align}
        \min_x \ & c^\top x \\
        \text{s.t.} \ & x\in X.
    \end{align}

If we assume that this problem is feasible and bounded, we can denote by :math:`x^*` a solution of this problem. Then,
we search for a violated constraint ":math:`\xi^\top x \le \xi_0`" for some :math:`(\xi_0,\xi)\in\Xi`. Observe that
a constraint is violated if, and only if, the following holds:

.. math::

    \left(\exists(\xi_0,\xi)\in\Xi, \ \xi^\top x^* > \xi_0\right)
    \Leftrightarrow
    \left(\max_{(\xi_0,\xi)\in\Xi} \xi_0 - \xi^\top x^* < 0\right).

Thus, the LazyCutCallback automatically solve the optimization problem on the right handside and checks for its value.
A new constraint is added if, and only if,

.. math::
    \max_{\xi\in\Xi} \xi_0 - \xi^\top x^* < -\varepsilon,

with :math:`\varepsilon` a given tolerance (by default, :code:`Tolerance::Feasibility`).


.. hint::

    You may also be interested by a tutorial showing how you can implement a simple Benders Decomposition using lazy
    cuts. :ref:`See Benders Decomposition tutorial <decomposition_benders>`.


.. doxygenclass:: idol::LazyCutCallback