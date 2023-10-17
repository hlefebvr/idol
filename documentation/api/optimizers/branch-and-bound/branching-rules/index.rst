.. _api_branching_rules:

Branching rules
===============

.. mermaid::

   classDiagram
       VariableBranching <|-- MostInfeasible
       VariableBranching <|-- LeastInfeasible
       VariableBranching <|-- FirstInfeasibleFound
       VariableBranching <|-- StrongBranching
       VariableBranching <|-- UniformlyRandom

.. toctree::
    :maxdepth: 2
    :glob:

    MostInfeasible
    LeastInfeasible
    FirstInfeasibleFound
    StrongBranching
    UniformlyRandom
