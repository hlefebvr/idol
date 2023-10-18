.. _api_branching_rules:

Branching rules
===============

.. mermaid::

   classDiagram
       VariableBranching <|-- MostInfeasible
       VariableBranching <|-- LeastInfeasible
       VariableBranching <|-- FirstInfeasibleFound
       VariableBranching <|-- StrongBranching
       VariableBranching <|-- PseudoCost
       VariableBranching <|-- UniformlyRandom

.. toctree::
    :maxdepth: 2
    :glob:

    MostInfeasible
    LeastInfeasible
    FirstInfeasibleFound
    PseudoCost
    StrongBranching
    UniformlyRandom
