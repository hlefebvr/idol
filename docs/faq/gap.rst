.. _how_gap:

How is the Optimality Gap Computed?
===================================

To assess global optimality, we use two different optimality gaps.

The relative optimality gap is computed as

.. math::

    \begin{equation}
        \gamma_{\text{rel}} := \frac{ UB - LB }{ 10^{-10} + |UB| }.
    \end{equation}

The absolute optimality gap is computed as

.. math::

    \begin{equation}
        \gamma_{\text{abs}} :=  UB - LB.
    \end{equation}

See also :ref:`Tolerances <api_tolerances>`.