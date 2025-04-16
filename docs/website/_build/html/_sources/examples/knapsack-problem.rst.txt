Solving a knapsack problem with HiGHS
=====================================

Problem Definition
------------------

Given a set of :math:`n` items, the goal is to select of subset of items to be put in a knapsack of limited capacity.
Each item has a weight (the amount of space it takes in the knapsack) and a profit (the value of the item).
The goal is to maximize the total profit of the items in the knapsack, while not exceeding the capacity.

For each item :math:`j\in\{1,\dotsc,n\}`, we let :math:`w_j` denote its weight and :math:`p_j` be its profit.
The capacity of the knapsack is noted :math:`C`.

The knapsack problem can be formulated as the following binary linear problem:

.. math::

    \begin{align*}
        \max_{x} \quad & \sum_{j=1}^n p_j x_j \\
        \text{s.t.} \quad & \sum_{j=1}^n w_j x_j \le C, \\
                    & x \in \{0,1\}^n.
    \end{align*}

Instance
--------

We will use an instance stored in a file called `knapsack.data.txt`. This file reads

.. literalinclude:: ../../examples/mixed-integer/knapsack.data.txt

The first line contains the number of items :math:`n`.
Then, the following line contains the profits of each item, :math:`p_j`.
The third line contains the weights of each item, :math:`w_j`.
Finally, the last line contains the capacity of the knapsack, :math:`C`.

Implementation
--------------

.. literalinclude:: ../../examples/mixed-integer/knapsack.example.cpp
    :language: cpp
