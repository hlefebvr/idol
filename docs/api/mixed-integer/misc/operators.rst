.. _api_operators:

Operator Overloading
====================

Building expressions in idol can be done in a very natural way using the
C++ operator overloading. The following tables summarize the operators
that can be used with the different expression types.

The tables show
the return type of the operator when applied to the different
expression types. The first column of the table shows the operator
applied to the expression type in the first row. The first row of the
table shows the expression type of the operator applied to the
expression type in the first column. The return type of the operator
is shown in the cell at the intersection of the row and column.

Operator: +
----------

.. list-table::
   :header-rows: 1
   :stub-columns: 1
   

   * - operator+
     - double
     - Var
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - double
     - double
     - AffExpr
     - AffExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - Var
     - AffExpr
     - LinExpr
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - LinExpr
     - AffExpr
     - LinExpr
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - AffExpr
     - AffExpr
     - AffExpr
     - AffExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - -
   * - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - -
     - GenerationPattern<Ctr>

Operator: -
----------

.. list-table::
   :header-rows: 1
   :stub-columns: 1
   

   * - operator-
     - double
     - Var
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - double
     - double
     - AffExpr
     - AffExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - Var
     - AffExpr
     - LinExpr
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - LinExpr
     - AffExpr
     - LinExpr
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - AffExpr
     - AffExpr
     - AffExpr
     - AffExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - -
   * - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - -
     - GenerationPattern<Ctr>

Operator: *
----------

.. list-table::
   :header-rows: 1
   :stub-columns: 1
   

   * - operator*
     - double
     - Var
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - double
     - double
     - LinExpr
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - Var
     - LinExpr
     - LinExpr
     - QuadExpr
     - QuadExpr
     - -
     - -
   * - LinExpr
     - LinExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - -
     - -
   * - AffExpr
     - AffExpr
     - QuadExpr
     - QuadExpr
     - QuadExpr
     - -
     - -
   * - QuadExpr
     - QuadExpr
     - -
     - -
     - -
     - -
     - -
   * - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>*
     - -
     - -
     - -
     - -

Note: * GenerationPattern<Ctr> multiplication only works if the GenerationPattern<Ctr> is a constant.
Otherwise, an exception is thrown.

Operator: /
----------

.. list-table::
   :header-rows: 1
   :stub-columns: 1
   

   * - operator/
     - double
     - Var
     - LinExpr
     - AffExpr
     - QuadExpr
     - GenerationPattern<Ctr>
   * - double
     - double
     - -
     - -
     - -
     - -
     - -
   * - Var
     - LinExpr
     - -
     - -
     - -
     - -
     - -
   * - LinExpr
     - LinExpr
     - -
     - -
     - -
     - -
     - -
   * - AffExpr
     - AffExpr
     - -
     - -
     - -
     - -
     - -
   * - QuadExpr
     - QuadExpr
     - -
     - -
     - -
     - -
     - -
   * - GenerationPattern<Ctr>
     - GenerationPattern<Ctr>
     - -
     - -
     - -
     - -
     - -
