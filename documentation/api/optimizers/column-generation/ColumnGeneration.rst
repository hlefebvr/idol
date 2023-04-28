.. _api_ColumnGeneration:

ColumnGeneration
================

.. danger::

    Currently, the master problem must have a "convexity constraint" for the algorithm to converge.
    Problems without this property will lead to undefined behavior.

    We strongly advise you to use :ref:`DantzigWolfeDecomposition <api_DantzigWolfeDecomposition>` instead.

.. doxygenclass:: ColumnGeneration
