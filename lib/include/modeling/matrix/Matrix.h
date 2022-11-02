//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_MATRIX_H
#define OPTIMIZE_MATRIX_H

#include <functional>

class MatrixCoefficientReference;
class Var;
class Ctr;
class Constant;
template<class> class LinExpr;
template<class> class Expr;
class Column;

class Matrix {
    friend class Model;

    static void apply_on_column(const Var &t_var, const std::function<void(const Ctr&, MatrixCoefficientReference&&)>& t_function);

    static void apply_on_row(const Ctr &t_ctr, const std::function<void(const Var&, MatrixCoefficientReference&&)>& t_function);

    static void add_to_obj(Expr<Var>& t_objective, const Var& t_var, Column& t_column);

    static void apply_obj(const Expr<Var>& t_objective, const std::function<void(const Var&, MatrixCoefficientReference&&)>& t_function);

    static void update_coefficient(const Var& t_var, const Ctr& t_ctr, LinExpr<Ctr> &t_column, LinExpr<Var> &t_row, Constant &&t_coefficient);
};

#endif //OPTIMIZE_MATRIX_H
