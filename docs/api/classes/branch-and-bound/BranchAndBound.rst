BranchAndBound
==============

This class is an optimizer factory which creates a new branch-and-bound algorithm.
It can be used to create customized branch-and-bound algorithms with a large degree of freedom.

.. seealso::

    If you are not familiar with optimizers and optimizer factories, please refer to :ref:`this page <api_optimizers>`.

.. admonition:: Example

    Here, we create a simple branch-and-bound algorithm where branching is done on integer variables which are being
    relaxed. Each node is solved by the external solver GLPK. Nodes are selected according to the "best-bound"
    rule while variables are selected according to the "most-infeasible" branching rule.

    The created algorithm also incorporates sub-tree exploration of maximum depth 2.

    .. code-block::

        model.use(
            BranchAndBound()
                .with_node_optimizer( GLPK::ContinuousRelaxation() )
                .with_branching_rule( MostInfeasible() )
                .with_node_selection_rule( BestBound() )
                .with_subtree_depth(2)
        );

.. doxygenclass:: idol::BranchAndBound
